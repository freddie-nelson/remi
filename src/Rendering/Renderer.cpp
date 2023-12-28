#include "../../include/Rendering/Renderer.h"
#include "../../include/Rendering/Shader/MeshShader.h"
#include "../../include/Rendering/Shader/InstancedMeshShader.h"
#include "../../include/Rendering/Shader/BatchedMeshShader.h"
#include "../../include/Rendering/Utility/OpenGLHelpers.h"
#include "../../include/Config.h"
#include "../../include/Core/BoundingCircle.h"
#include "../../include/Rendering/Utility/Timestep.h"
#include "../../include/Rendering/Renderable.h"

#include "../../include/externals/glad/gl.h"
#include <iostream>
#include <memory>
#include <math.h>
#include <glm/gtc/type_ptr.hpp>
#include <utility>

Rendering::Renderer::Renderer(GLFWwindow *glfwWindow, int width, int height) : glfwWindow(glfwWindow), width(width), height(height), camera(Camera(width, height))
{
    setSize(width, height);
}

Rendering::Renderer::~Renderer()
{
    delete this;
}

void Rendering::Renderer::init()
{
    enableDepthTest(true);
    enableAlphaBlending(false);

    meshShader = Shader();
    if (!meshShader.loadFromSource(meshVertexShader, meshFragShader))
    {
        throw std::runtime_error("Failed to load mesh shader.");
    }

    instancedMeshShader = Shader();
    if (!instancedMeshShader.loadFromSource(instancedMeshVertexShader, meshFragShader))
    {
        throw std::runtime_error("Failed to load instanced mesh shader.");
    }

    batchedMeshShader = Shader();
    if (!batchedMeshShader.loadFromSource(batchedMeshVertexShader, meshFragShader))
    {
        throw std::runtime_error("Failed to load batched mesh shader.");
    }
}

void Rendering::Renderer::update(const ECS::Registry &registry, float dt)
{
    auto &entities = registry.view<Mesh2D, Core::Transform, Material, Renderable>();

    auto now = Rendering::timeSinceEpochMillisec();

    auto cameraAabb = camera.getAABB();
    auto cameraBoundingCircle = Core::BoundingCircle(cameraAabb);
    auto fatCameraAabb = Core::AABB(cameraBoundingCircle.getCentre(), cameraBoundingCircle.getRadius());

    for (auto &e : entities)
    {
        auto &renderable = registry.get<Renderable>(e);
        if (!renderable.isVisible)
        {
            continue;
        }

        if (!renderable.isStatic)
        {
            // check if entity was previously static
            if (staticRenderables.contains(e))
            {
                staticRenderablesTree.remove(e);

                delete staticRenderables[e];
                staticRenderables.erase(e);
            }

            continue;
        }

        // update entity in tree
        if (staticRenderables.contains(e))
        {
            // auto &aabb = staticRenderables[e];
            // staticRenderablesTree.update(e, aabb);
        }
        // add entity to tree
        else
        {
            auto &transform = registry.get<Core::Transform>(e);
            auto &mesh = registry.get<Mesh2D>(e);

            auto boundingCircle = Core::BoundingCircle(mesh.getAABB(), transform);
            auto aabb = new Core::AABB(boundingCircle.getCentre(), boundingCircle.getRadius());

            staticRenderablesTree.insert(e, *aabb);
            staticRenderables[e] = aabb;
        }
    }

    auto renderables = staticRenderablesTree.query(fatCameraAabb);

    std::cout << "cull time: " << Rendering::timeSinceEpochMillisec() - now << std::endl;

    batch(registry, renderables);
}

void Rendering::Renderer::clear(bool clearColorBuffer, bool clearDepthBuffer, bool clearStencilBuffer) const
{
    glClearColor(clearColor.r(), clearColor.g(), clearColor.b(), clearColor.a());

    int clearBits = 0;
    if (clearColorBuffer)
    {
        clearBits |= GL_COLOR_BUFFER_BIT;
    }
    if (clearDepthBuffer)
    {
        clearBits |= GL_DEPTH_BUFFER_BIT;
    }
    if (clearStencilBuffer)
    {
        clearBits |= GL_STENCIL_BUFFER_BIT;
    }

    glClear(clearBits);
}

void Rendering::Renderer::present() const
{
    glfwSwapBuffers(glfwWindow);
}

void Rendering::Renderer::entity(const ECS::Registry &registry, ECS::Entity &entity)
{
    // mesh data
    auto &mesh = registry.get<Mesh2D>(entity);

    const std::vector<glm::vec2> &vertices = mesh.getVertices();
    float *verticesPointer = (float *)glm::value_ptr(vertices[0]);

    const std::vector<unsigned int> &indices = mesh.getIndices();

    const std::vector<glm::vec2> &uvs = mesh.getUvs();
    float *uvsPointer = (float *)glm::value_ptr(uvs[0]);

    // material data
    auto &material = registry.get<Material>(entity);
    auto color = material.getColor().getColor();

    auto texture = material.getTexture();
    auto boundTexture = textureManager.bind(texture);
    auto &textures = textureManager.getTexturesUniform();

    // transform
    auto &transform = registry.get<Core::Transform>(entity);
    auto &transformMatrix = transform.getTransformationMatrix();

    meshShader.use();

    meshShader.setUniform("uViewProjectionMatrix", &viewProjectionMatrix);

    // texture uniforms
    meshShader.setUniform("uColor", &color);

    meshShader.setUniformArray("uTextures", const_cast<int *>(&textures[0]), textures.size());

    meshShader.setUniform("uTextureUnit", &boundTexture.textureUnit);
    meshShader.setUniform("uTextureSize", &boundTexture.textureSize);
    meshShader.setUniform("uTextureAtlasPos", &boundTexture.posInAtlas);
    meshShader.setUniform("uTextureAtlasSize", &boundTexture.atlasSize);

    // transform
    meshShader.setUniform("uMeshTransform", const_cast<glm::mat4 *>(&transformMatrix));

    // mesh data
    meshShader.setAttrib("aPos", verticesPointer, vertices.size() * 2, 2, GL_FLOAT, false, 0, GL_DYNAMIC_DRAW);
    meshShader.setIndices("aPos", &indices[0], indices.size(), GL_DYNAMIC_DRAW);

    meshShader.setAttrib("aTexCoord", uvsPointer, uvs.size() * 2, 2, GL_FLOAT, false, 0, GL_DYNAMIC_DRAW);

    // draw
    meshShader.draw(GL_TRIANGLES, indices.size());
    meshShader.unbind();
};

void Rendering::Renderer::instance(const ECS::Registry &registry, const Mesh2D &m, const std::vector<ECS::Entity> &instances)
{
    const auto instanceCount = instances.size();

    // mesh data
    const std::vector<glm::vec2> &vertices = m.getVertices();
    float *vertexArr = (float *)glm::value_ptr(vertices[0]);

    const std::vector<unsigned int> &indices = m.getIndices();

    const std::vector<glm::vec2> &uvs = m.getUvs();
    float *uvArr = (float *)glm::value_ptr(uvs[0]);

    // create transforms and materials arrays
    // vectors used so that stack overflow does not occur when instance count is too high
    // vector allocates items on heap
    std::vector<glm::mat4> transform(instanceCount);
    std::vector<glm::vec2> textureAtlasPos(instanceCount);
    std::vector<unsigned int> textureUnit(instanceCount);
    std::vector<glm::vec2> textureSize(instanceCount);
    std::vector<glm::vec4> color(instanceCount);

    for (int i = 0; i < instanceCount; i++)
    {
        auto &t = registry.get<Core::Transform>(instances[i]);
        auto &material = registry.get<Material>(instances[i]);

        transform[i] = t.getTransformationMatrix();

        auto texture = material.getTexture();
        auto boundTexture = textureManager.bind(texture);

        textureAtlasPos[i] = boundTexture.posInAtlas;
        textureUnit[i] = boundTexture.textureUnit;
        textureSize[i] = boundTexture.textureSize;
        color[i] = material.getColor().getColor();
    }

    instancedMeshShader.use();

    instancedMeshShader.setUniform("uViewProjectionMatrix", &viewProjectionMatrix);

    auto atlasSize = glm::vec2(TextureAtlas::getAtlasSize());
    instancedMeshShader.setUniform("uTextureAtlasSize", &atlasSize);

    // instanced arrays
    float *atlasPosArr = (float *)glm::value_ptr(textureAtlasPos[0]);
    instancedMeshShader.setAttrib("aTextureAtlasPos", atlasPosArr, instanceCount * 2, 2, GL_FLOAT, false, 0, GL_DYNAMIC_DRAW, 1);

    unsigned int *textureUnitArr = &textureUnit[0];
    instancedMeshShader.setAttrib("aTextureUnit", textureUnitArr, instanceCount, 1, GL_UNSIGNED_INT, false, 0, GL_DYNAMIC_DRAW, 1);

    float *textureSizeArr = (float *)glm::value_ptr(textureSize[0]);
    instancedMeshShader.setAttrib("aTextureSize", textureSizeArr, instanceCount * 2, 2, GL_FLOAT, false, 0, GL_DYNAMIC_DRAW, 1);

    float *colorArr = (float *)glm::value_ptr(color[0]);
    instancedMeshShader.setAttrib("aColor", colorArr, instanceCount * 4, 4, GL_FLOAT, false, 0, GL_DYNAMIC_DRAW, 1);

    float *transformArr = (float *)glm::value_ptr(transform[0]);
    instancedMeshShader.setAttrib("aTransform", transformArr, instanceCount * 16, 16, GL_FLOAT, false, 0, GL_DYNAMIC_DRAW, 1, 4);

    // vertices and indices
    instancedMeshShader.setAttrib("aPos", vertexArr, vertices.size() * 2, 2, GL_FLOAT, false, 0, GL_DYNAMIC_DRAW);
    instancedMeshShader.setIndices("aPos", &indices[0], indices.size(), GL_DYNAMIC_DRAW);

    instancedMeshShader.setAttrib("aTexCoord", uvArr, uvs.size() * 2, 2, GL_FLOAT, false, 0, GL_DYNAMIC_DRAW);

    instancedMeshShader.drawInstanced(instanceCount, GL_TRIANGLES, indices.size());
    instancedMeshShader.unbind();
};

void Rendering::Renderer::batch(const ECS::Registry &registry, const std::vector<ECS::Entity> &renderables)
{
    auto now = Rendering::timeSinceEpochMillisec();

    // get number of vertices and indices
    size_t verticesCount = 0;
    size_t indicesCount = 0;

    for (auto &e : renderables)
    {
        auto &m = registry.get<Mesh2D>(e);

        verticesCount += m.getVertices().size();
        indicesCount += m.getIndices().size();
    }

    // create vertices and indices arrays
    // vectors used so that stack overflow does not occur when instance count is too high
    // vector allocates items on heap
    std::vector<glm::vec4> batchedVertices(verticesCount);
    std::vector<unsigned int> batchedIndices(indicesCount);

    std::vector<glm::vec2> batchedAtlasPos(verticesCount);

    std::vector<unsigned int> batchedTextureUnits(verticesCount);
    std::vector<glm::vec2> batchedTextureSizes(verticesCount);
    std::vector<glm::vec2> batchedTexCoords(verticesCount);

    std::vector<glm::vec4> batchedColors(verticesCount);

    int verticesOffset = 0;
    int indicesOffset = 0;

    // fill arrays with mesh and material data
    for (auto &e : renderables)
    {
        const auto &mesh = registry.get<Mesh2D>(e);

        const auto &transform = registry.get<Core::Transform>(e);
        auto &transformMatrix = transform.getTransformationMatrix();

        const auto &material = registry.get<Material>(e);
        const auto color = material.getColor().getColor();

        const auto texture = material.getTexture();
        const auto boundTexture = textureManager.bind(texture);

        const std::vector<glm::vec2> &vertices = mesh.getVertices();
        const std::vector<unsigned int> &indices = mesh.getIndices();
        const std::vector<glm::vec2> &uvs = mesh.getUvs();

        for (int i = 0; i < vertices.size(); i++)
        {
            batchedVertices[verticesOffset + i] = transformMatrix * glm::vec4(vertices[i], 0.0f, 1.0f);

            batchedAtlasPos[verticesOffset + i] = boundTexture.posInAtlas;

            batchedTextureUnits[verticesOffset + i] = boundTexture.textureUnit;
            batchedTextureSizes[verticesOffset + i] = boundTexture.textureSize;
            batchedTexCoords[verticesOffset + i] = uvs[i];

            batchedColors[verticesOffset + i] = color;
        }

        for (int i = 0; i < indices.size(); i++)
        {
            batchedIndices[indicesOffset + i] = indices[i] + verticesOffset;
        }

        verticesOffset += vertices.size();
        indicesOffset += indices.size();
    }

    std::cout << "batch time: " << Rendering::timeSinceEpochMillisec() - now << std::endl;

    now = Rendering::timeSinceEpochMillisec();

    // set up shader variables
    // and draw
    batchedMeshShader.use();

    batchedMeshShader.setUniform("uViewProjectionMatrix", &viewProjectionMatrix);

    auto atlasSize = glm::vec2(TextureAtlas::getAtlasSize());
    batchedMeshShader.setUniform("uTextureAtlasSize", &atlasSize);

    batchedMeshShader.setAttrib("aTextureAtlasPos", (float *)glm::value_ptr(batchedAtlasPos[0]), verticesCount * 2, 2, GL_FLOAT, false, 0, GL_DYNAMIC_DRAW);
    batchedMeshShader.setAttrib("aTextureUnit", &batchedTextureUnits[0], verticesCount, 1, GL_UNSIGNED_INT, false, 0, GL_DYNAMIC_DRAW);
    batchedMeshShader.setAttrib("aTextureSize", (float *)glm::value_ptr(batchedTextureSizes[0]), verticesCount * 2, 2, GL_FLOAT, false, 0, GL_DYNAMIC_DRAW);
    batchedMeshShader.setAttrib("aTexCoord", (float *)glm::value_ptr(batchedTexCoords[0]), verticesCount * 2, 2, GL_FLOAT, false, 0, GL_DYNAMIC_DRAW);

    batchedMeshShader.setAttrib("aColor", (float *)glm::value_ptr(batchedColors[0]), verticesCount * 4, 4, GL_FLOAT, false, 0, GL_DYNAMIC_DRAW);

    batchedMeshShader.setAttrib("aPos", (float *)glm::value_ptr(batchedVertices[0]), verticesCount * 4, 4, GL_FLOAT, false, 0, GL_DYNAMIC_DRAW);
    batchedMeshShader.setIndices("aPos", &batchedIndices[0], indicesCount, GL_DYNAMIC_DRAW);

    batchedMeshShader.draw(GL_TRIANGLES, indicesCount);
    batchedMeshShader.unbind();

    std::cout << "draw time: " << Rendering::timeSinceEpochMillisec() - now << std::endl;
}

void Rendering::Renderer::setClearColor(const Color &color)
{
    clearColor = color;
}

Rendering::Color Rendering::Renderer::getClearColor() const
{
    return clearColor;
}

void Rendering::Renderer::enableDepthTest(bool enable)
{
    if (enable)
    {
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }
}

bool Rendering::Renderer::isDepthTestEnabled() const
{
    return glIsEnabled(GL_DEPTH_TEST);
}

void Rendering::Renderer::enableAlphaBlending(bool enable)
{
    if (enable)
    {
        glEnable(GL_BLEND);
        glBlendFunc(alphaBlendingSFactor, alphaBlendingDFactor);
    }
    else
    {
        glDisable(GL_BLEND);
    }
}

void Rendering::Renderer::setAlphaBlendingFunction(GLenum sfactor, GLenum dfactor)
{
    if (!glIsValidAlphaBlendingFunction(sfactor) || !glIsValidAlphaBlendingFunction(dfactor))
    {
        throw std::invalid_argument("Renderer::setAlphaBlendingFunction: sfactor and dfactor must be valid values.");
    }

    alphaBlendingSFactor = sfactor;
    alphaBlendingDFactor = dfactor;
    glBlendFunc(sfactor, dfactor);
}

bool Rendering::Renderer::isAlphaBlendingEnabled() const
{
    return glIsEnabled(GL_BLEND);
}

void Rendering::Renderer::setSize(int w, int h)
{
    if (w <= 0 || h <= 0)
    {
        throw std::runtime_error("Renderer::resize: width and height must be greater than 0.");
    }

    width = w;
    height = h;

    glViewport(0, 0, width, height);

    if (syncCameraSizeWithRenderer)
    {
        camera.setViewportSize(width, height);
    }
}

std::pair<int, int> Rendering::Renderer::getSize() const
{
    return std::make_pair(width, height);
}

std::pair<int, int> Rendering::Renderer::getWindowSize() const
{
    int width, height;
    glfwGetFramebufferSize(glfwWindow, &width, &height);

    return std::make_pair(width, height);
}

Rendering::Camera &Rendering::Renderer::getCamera()
{
    return camera;
}

void Rendering::Renderer::setCamera(Camera &camera)
{
    this->camera = camera;
}

void Rendering::Renderer::syncCameraSize(bool sync)
{
    syncCameraSizeWithRenderer = sync;
}

bool Rendering::Renderer::getSyncCameraSize() const
{
    return syncCameraSizeWithRenderer;
}

void Rendering::Renderer::updateViewProjectionMatrix()
{
    viewProjectionMatrix = camera.getViewProjectionMatrix();
}