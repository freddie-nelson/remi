#include "../../include/Rendering/Renderer.h"
#include "../../include/Rendering/Shader/MeshShader.h"
#include "../../include/Rendering/Shader/InstancedMeshShader.h"
#include "../../include/Rendering/Shader/BatchedMeshShader.h"
#include "../../include/Rendering/Utility/OpenGLHelpers.h"
#include "../../include/Config.h"
#include "../../include/Core/BoundingCircle.h"
#include "../../include/Rendering/Renderable.h"
#include "../../include/Rendering/Camera/ActiveCamera.h"
#include "../../include/Rendering/Camera/Camera.h"

#include <glad/gl.h>
#include <iostream>
#include <memory>
#include <math.h>
#include <glm/gtc/type_ptr.hpp>
#include <utility>
#include <algorithm>

Rendering::Renderer::Renderer(GLFWwindow *glfwWindow, int width, int height)
{
    this->glfwWindow = glfwWindow;
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

void Rendering::Renderer::update(const ECS::Registry &registry, const Core::Timestep &timestep)
{
    if (syncRendererSizeWithWindow)
    {
        auto [width, height] = getWindowSize();
        setSize(width, height);
    }

    auto &entities = registry.view<Mesh2D, Core::Transform, Material, Renderable>();

    // exit if no entities
    if (entities.size() == 0)
    {
        return;
    }

    // auto now = Rendering::timeSinceEpochMillisec();

    auto activeCamera = getActiveCamera(registry);

    if (syncActiveCameraSizeWithRenderer)
    {
        auto &camera = registry.get<Camera>(activeCamera);
        camera.setViewportSize(width, height);
    }

    auto fatCameraAabb = getCullingAABB(registry, activeCamera);

    // cull entities outside camera's viewport to get renderables
    std::vector<ECS::Entity> renderables;

    // cull static renderables
    auto dynamicsCount = getRenderables(registry, entities, fatCameraAabb, false, renderables);

    // cull dynamic renderables
    if (dynamicsCount > 0)
        getRenderables(registry, entities, fatCameraAabb, true, renderables);

    // prune trees
    updatesSinceLastTreePrune++;
    if (updatesSinceLastTreePrune == treePruneFrequency)
    {
        pruneTrees(registry);
        updatesSinceLastTreePrune = 0;
    }

    // std::cout << "cull time: " << Rendering::timeSinceEpochMillisec() - now << std::endl;

    // exit if no renderables
    if (renderables.size() == 0)
    {
        return;
    }

    // now = Rendering::timeSinceEpochMillisec();

    // split renderables
    std::vector<ECS::Entity> opaqueRenderables;
    std::vector<ECS::Entity> transparentRenderables;

    if (isAlphaBlendingEnabled())
    {
        for (auto &e : renderables)
        {
            auto &material = registry.get<Material>(e);

            if (material.isTransparent())
            {
                transparentRenderables.push_back(e);
            }
            else
            {
                opaqueRenderables.push_back(e);
            }
        }

        // sort transparent renderables by z index (back to front)
        // for correct alpha blending
        sortRenderables(registry, transparentRenderables);
    }
    else
    {
        opaqueRenderables = renderables;
    }

    // std::cout << "sort time: " << Rendering::timeSinceEpochMillisec() - now << std::endl;

    // batch render
    if (opaqueRenderables.size() > 0)
        batch(registry, activeCamera, opaqueRenderables);

    if (transparentRenderables.size() > 0)
        batch(registry, activeCamera, transparentRenderables);
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

void Rendering::Renderer::entity(const ECS::Registry &registry, const ECS::Entity camera, ECS::Entity &entity)
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

    // view projection matrix
    auto viewProjectionMatrix = getViewProjectionMatrix(registry, camera);

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

void Rendering::Renderer::instance(const ECS::Registry &registry, const ECS::Entity camera, const Mesh2D &m, const std::vector<ECS::Entity> &instances)
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

    auto boundTextures = bindTextures(registry, instances);

    for (size_t i = 0; i < instanceCount; i++)
    {
        auto &t = registry.get<Core::Transform>(instances[i]);
        auto &material = registry.get<Material>(instances[i]);

        transform[i] = t.getTransformationMatrix();

        auto texture = material.getTexture();
        const auto &boundTexture = boundTextures[texture->getId()];

        textureAtlasPos[i] = boundTexture.posInAtlas;
        textureUnit[i] = boundTexture.textureUnit;
        textureSize[i] = boundTexture.textureSize;
        color[i] = material.getColor().getColor();
    }

    // view projection matrix
    auto viewProjectionMatrix = getViewProjectionMatrix(registry, camera);

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

void Rendering::Renderer::batch(const ECS::Registry &registry, const ECS::Entity camera, const std::vector<ECS::Entity> &renderables)
{
    // auto now = Rendering::timeSinceEpochMillisec();

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

    auto boundTextures = bindTextures(registry, renderables);

    // fill arrays with mesh and material data
    for (auto &e : renderables)
    {
        const auto &mesh = registry.get<Mesh2D>(e);

        const auto &transform = registry.get<Core::Transform>(e);
        auto &transformMatrix = transform.getTransformationMatrix();

        const auto &material = registry.get<Material>(e);
        const auto color = material.getColor().getColor();

        const auto texture = material.getTexture();
        const auto &boundTexture = boundTextures[texture->getId()];

        const std::vector<glm::vec2> &vertices = mesh.getVertices();
        const std::vector<unsigned int> &indices = mesh.getIndices();
        const std::vector<glm::vec2> &uvs = mesh.getUvs();

        for (size_t i = 0; i < vertices.size(); i++)
        {
            batchedVertices[verticesOffset + i] = transformMatrix * glm::vec4(vertices[i], 0.0f, 1.0f);

            batchedAtlasPos[verticesOffset + i] = boundTexture.posInAtlas;

            batchedTextureUnits[verticesOffset + i] = boundTexture.textureUnit;
            batchedTextureSizes[verticesOffset + i] = boundTexture.textureSize;
            batchedTexCoords[verticesOffset + i] = uvs[i];

            batchedColors[verticesOffset + i] = color;
        }

        for (size_t i = 0; i < indices.size(); i++)
        {
            batchedIndices[indicesOffset + i] = indices[i] + verticesOffset;
        }

        verticesOffset += vertices.size();
        indicesOffset += indices.size();
    }

    // view projection matrix
    auto viewProjectionMatrix = getViewProjectionMatrix(registry, camera);

    // std::cout << "batch time: " << Rendering::timeSinceEpochMillisec() - now << std::endl;

    // now = Rendering::timeSinceEpochMillisec();

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

    // std::cout << "draw time: " << Rendering::timeSinceEpochMillisec() - now << std::endl;
}

void Rendering::Renderer::sortRenderables(const ECS::Registry &registry, std::vector<ECS::Entity> &renderables)
{
    std::unordered_map<ECS::Entity, unsigned int> zIndices;
    for (auto &e : renderables)
    {
        auto &transform = registry.get<Core::Transform>(e);
        zIndices.emplace(e, transform.getZIndex());
    }

    std::sort(renderables.begin(), renderables.end(), [&zIndices](const ECS::Entity &a, const ECS::Entity &b)
              { return zIndices[a] < zIndices[b]; });
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

ECS::Entity Rendering::Renderer::getActiveCamera(const ECS::Registry &registry) const
{
    auto activeCameras = registry.view<ActiveCamera>();

    if (activeCameras.size() == 0)
    {
        throw std::runtime_error("Renderer (getActiveCamera): no active camera.");
    }
    else if (activeCameras.size() > 1)
    {
        throw std::runtime_error("Renderer (getActiveCamera): more than one active camera.");
    }

    if (registry.has<Camera>(activeCameras[0]) && registry.has<Core::Transform>(activeCameras[0]))
    {
        return activeCameras[0];
    }
    else
    {
        throw std::runtime_error("Renderer (getActiveCamera): active camera does not have a camera and/or transform component.");
    }
}

Core::AABB Rendering::Renderer::getCullingAABB(const ECS::Registry &registry, const ECS::Entity camera) const
{
    auto &cameraComponent = registry.get<Camera>(camera);
    auto &cameraTransform = registry.get<Core::Transform>(camera);

    auto cameraAabb = cameraComponent.getAABB();

    Core::AABB fatCameraAabb;

    // if camera is rotated use bounding circle aabb
    if (cameraTransform.getRotation() != 0)
    {
        auto cameraBoundingCircle = Core::BoundingCircle(cameraAabb, cameraTransform);
        fatCameraAabb = Core::AABB(cameraBoundingCircle.getCentre(), cameraBoundingCircle.getRadius());
    }
    else
    {
        // otherwise scaled and translated aabb is sufficient
        fatCameraAabb = cameraComponent.getScaledAndTranslatedAabb(cameraTransform);
    }

    return fatCameraAabb;
}

void Rendering::Renderer::syncSizeWithWindow(bool sync)
{
    syncRendererSizeWithWindow = sync;
}

bool Rendering::Renderer::getSyncSizeWithWindow() const
{
    return syncRendererSizeWithWindow;
}

void Rendering::Renderer::syncActiveCameraSize(bool sync)
{
    syncActiveCameraSizeWithRenderer = sync;
}

bool Rendering::Renderer::getSyncActiveCameraSize() const
{
    return syncActiveCameraSizeWithRenderer;
}

void Rendering::Renderer::pruneTrees(const ECS::Registry &registry)
{
    auto staticTreeEntities = staticRenderablesTree.getIds();
    for (auto &e : staticTreeEntities)
    {
        if (!registry.has<Renderable>(e))
        {
            staticRenderablesTree.remove(e);
            staticRenderables.erase(e);
        }
    }

    auto dynamicTreeEntities = dynamicRenderablesTree.getIds();
    for (auto &e : dynamicTreeEntities)
    {
        if (!registry.has<Renderable>(e))
        {
            dynamicRenderablesTree.remove(e);
            dynamicRenderables.erase(e);
        }
    }
}

size_t Rendering::Renderer::getRenderables(const ECS::Registry &registry, const std::vector<ECS::Entity> &entities, const Core::AABB &viewAabb, bool isStatic, std::vector<ECS::Entity> &renderables)
{
    size_t otherCount = 0;

    for (auto &e : entities)
    {
        auto &renderable = registry.get<Renderable>(e);
        if (!renderable.isVisible)
        {
            continue;
        }

        if (isStatic && !renderable.isStatic)
        {
            // check if entity was previously static
            if (staticRenderables.contains(e))
            {
                staticRenderablesTree.remove(e);
                staticRenderables.erase(e);
            }

            otherCount++;

            continue;
        }

        if (!isStatic && renderable.isStatic)
        {
            // check if entity was previously dynamic
            if (dynamicRenderables.contains(e))
            {
                dynamicRenderablesTree.remove(e);
                dynamicRenderables.erase(e);
            }

            otherCount++;

            continue;
        }

        // skip if already in tree and is static
        if (isStatic && staticRenderables.contains(e))
        {
            continue;
        }

        auto &transform = registry.get<Core::Transform>(e);
        auto &mesh = registry.get<Mesh2D>(e);

        auto boundingCircle = Core::BoundingCircle(mesh.getAABB(), transform);
        auto aabb = Core::AABB(boundingCircle.getCentre(), boundingCircle.getRadius());

        if (dynamicRenderablesTree.has(e))
        {
            bool updated = dynamicRenderablesTree.update(e, aabb);
            if (updated)
            {
                dynamicRenderables.emplace(e, std::move(aabb));
            }

            continue;
        }

        if (isStatic)
        {
            staticRenderables.emplace(e, std::move(aabb));
            staticRenderablesTree.insert(e, staticRenderables[e]);
        }
        else
        {
            dynamicRenderables.emplace(e, std::move(aabb));
            dynamicRenderablesTree.insert(e, dynamicRenderables[e]);
        }
    }

    // get entities in view
    if (isStatic)
    {
        staticRenderablesTree.query(viewAabb, renderables);
    }
    else
    {
        dynamicRenderablesTree.query(viewAabb, renderables);
    }

    return otherCount;
}

std::unordered_map<Rendering::TextureId, Rendering::TextureManager::BoundTexture> Rendering::Renderer::bindTextures(const ECS::Registry &registry, const std::vector<ECS::Entity> &renderables)
{
    std::unordered_map<Rendering::TextureId, Rendering::TextureManager::BoundTexture> boundTextures;

    for (auto &e : renderables)
    {
        auto &material = registry.get<Material>(e);
        auto texture = material.getTexture();

        if (texture == nullptr)
        {
            continue;
        }

        if (!boundTextures.contains(texture->getId()))
        {
            boundTextures.emplace(texture->getId(), textureManager.bind(texture));
        }
    }

    return boundTextures;
}

glm::mat4 Rendering::Renderer::getViewProjectionMatrix(const ECS::Registry &registry, const ECS::Entity camera) const
{
    auto &cameraComponent = registry.get<Camera>(camera);
    auto &cameraTransform = registry.get<Core::Transform>(camera);

    return cameraComponent.getViewProjectionMatrix(cameraTransform);
}