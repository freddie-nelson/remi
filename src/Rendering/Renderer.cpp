#include "../../include/Rendering/Renderer.h"
#include "../../include/Rendering/Shader/MeshShader.h"
#include "../../include/Rendering/Shader/InstancedMeshShader.h"
#include "../../include/Rendering/Shader/BatchedMeshShader.h"
#include "../../include/Rendering/Utility/OpenGLHelpers.h"
#include "../../include/Config.h"
#include "../../include/Core/BoundingCircle.h"
#include "../../include/Rendering/Renderable.h"
#include "../../include/Rendering/Material/MaterialHelpers.h"
#include "../../include/Rendering/Camera/ActiveCamera.h"
#include "../../include/Rendering/Camera/Camera.h"
#include "../../include/gl.h"
#include "../../include/Rendering/Shader/Shader.h"

#include <iostream>
#include <memory>
#include <math.h>
#include <glm/gtc/type_ptr.hpp>
#include <utility>
#include <algorithm>
#include <map>

// ! TODO: don't rebuffer data for static meshes

Rendering::RendererShaders::RendererShaders(const std::string &fragmentShader)
{
    if (!meshShader.loadFromSource(meshVertexShader, fragmentShader))
    {
        throw std::runtime_error("Failed to load mesh shader. Fragment shader: \n\n" + fragmentShader);
    }

    if (!instancedMeshShader.loadFromSource(instancedMeshVertexShader, fragmentShader))
    {
        throw std::runtime_error("Failed to load instanced mesh shader. Fragment shader: \n\n" + fragmentShader);
    }

    if (!batchedMeshShader.loadFromSource(batchedMeshVertexShader, fragmentShader))
    {
        throw std::runtime_error("Failed to load batched mesh shader. Fragment shader: \n\n" + fragmentShader);
    }
}

Rendering::Renderer::Renderer(GLFWwindow *glfwWindow, int width, int height)
{
    this->glfwWindow = glfwWindow;
    setSize(width, height);

    ownsRenderTarget = true;
    renderTarget = new RenderTarget(width, height);
}

Rendering::Renderer::~Renderer()
{
    delete this;
}

void Rendering::Renderer::init()
{
    enableDepthTest(true);
    enableDepthWrite(true);
    enableAlphaBlending(false);

    // init shaders
    shaders.emplace(DEFAULT_SHADER_KEY, new RendererShaders(meshFragShader));
}

void Rendering::Renderer::update(const ECS::Registry &registry, const Core::Timestep &timestep)
{
    if (syncRendererSizeWithWindow)
    {
        auto size = getWindowSize();
        setSize(size.x, size.y);
    }

    auto activeCamera = getActiveCamera(registry);
    if (syncActiveCameraSizeWithRenderer)
    {
        auto &camera = registry.get<Camera>(activeCamera);
        camera.setViewportSize(width, height);
    }

    if (renderTarget != nullptr)
    {
        renderTarget->resize(getSize());
    }

    if (unbindUnusedTextures)
        textureManager.unbindUnusedTextures();
}

void Rendering::Renderer::clear(bool clearColorBuffer, bool clearDepthBuffer, bool clearStencilBuffer) const
{
    glClearWithColor(clearColor, clearColorBuffer, clearDepthBuffer, clearStencilBuffer);

    if (renderTarget != nullptr)
    {
        renderTarget->clear(clearColor, clearColorBuffer, clearDepthBuffer, clearStencilBuffer);
    }
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
    const std::vector<unsigned int> &indicesVec = mesh.getIndices();
    const std::vector<glm::vec2> &uvs = mesh.getUvs();

    // material data
    auto *material = getMaterial(registry, entity);
    auto color = material->getColor().getColor();

    auto boundTexture = bindTextures(registry, {entity})[0];
    auto &texturesUniform = textureManager.getTexturesUniform();

    // transform
    auto &transform = registry.get<Core::Transform>(entity);
    auto &transformMatrix = transform.getTransformationMatrix();

    // view projection matrix
    auto viewProjectionMatrix = getViewProjectionMatrix(registry, camera);

    // uniforms
    Uniform uViewProjectionMatrix("uViewProjectionMatrix", viewProjectionMatrix);
    Uniform uColor("uColor", color);
    Uniform uTextures("uTextures", texturesUniform, true, texturesUniform.size(), GL_SAMPLER_2D);

    Uniform uTextureUnit("uTextureUnit", boundTexture.textureUnit);
    Uniform uTextureSize("uTextureSize", boundTexture.textureSize);
    Uniform uTextureAtlasPos("uTextureAtlasPos", boundTexture.posInAtlas);
    Uniform uTextureAtlasSize("uTextureAtlasSize", boundTexture.atlasSize);

    Uniform uMeshTransform("uMeshTransform", transformMatrix);

    // attribs
    VertexAttrib<glm::vec2> aPos("aPos", vertices);
    VertexAttrib<glm::vec2> aTexCoord("aTexCoord", uvs);

    // indices
    VertexIndices indices(indicesVec);

    // setup shader
    auto &[meshShader, instancedMeshShader, batchedMeshShader] = getShaders(registry, entity);

    meshShader.use();

    meshShader.uniform(&uViewProjectionMatrix);
    meshShader.uniform(&uColor);

    meshShader.uniform(&uTextures);
    meshShader.uniform(&uTextureUnit);
    meshShader.uniform(&uTextureSize);
    meshShader.uniform(&uTextureAtlasPos);
    meshShader.uniform(&uTextureAtlasSize);

    meshShader.uniform(&uMeshTransform);

    if (registry.has<ShaderMaterial>(entity))
    {
        auto &shaderMaterial = registry.get<ShaderMaterial>(entity);
        meshShader.uniform(shaderMaterial.getUniforms());
    }

    meshShader.attrib(&aPos);
    meshShader.attrib(&aTexCoord);

    meshShader.indices(&indices);

    // draw
    meshShader.draw(GL_TRIANGLES, indices.size());
    meshShader.unbind();
};

void Rendering::Renderer::instance(const ECS::Registry &registry, const ECS::Entity camera, const Mesh2D &m, const std::vector<ECS::Entity> &instances)
{
    const auto instanceCount = instances.size();

    // mesh data
    const std::vector<glm::vec2> &vertices = m.getVertices();
    const std::vector<unsigned int> &indicesVec = m.getIndices();
    const std::vector<glm::vec2> &uvs = m.getUvs();

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
        auto material = getMaterial(registry, instances[i]);

        transform[i] = t.getTransformationMatrix();

        auto texture = material->getTexture();
        const auto &boundTexture = boundTextures[texture->getId()];

        textureAtlasPos[i] = boundTexture.posInAtlas;
        textureUnit[i] = boundTexture.textureUnit;
        textureSize[i] = boundTexture.textureSize;
        color[i] = material->getColor().getColor();
    }

    auto atlasSize = glm::vec2(TextureAtlas::getAtlasSize());

    auto &textures = textureManager.getTexturesUniform();

    // view projection matrix
    auto viewProjectionMatrix = getViewProjectionMatrix(registry, camera);

    // uniforms
    Uniform uViewProjectionMatrix("uViewProjectionMatrix", viewProjectionMatrix);
    Uniform uTextureAtlasSize("uTextureAtlasSize", atlasSize);
    Uniform uTextures("uTextures", textures, true, textures.size(), GL_SAMPLER_2D);

    // attribs
    VertexAttrib aPos("aPos", vertices);
    VertexAttrib aTexCoord("aTexCoord", uvs);
    VertexAttrib aTextureAtlasPos("aTextureAtlasPos", textureAtlasPos);
    VertexAttrib aTextureUnit("aTextureUnit", textureUnit);
    VertexAttrib aTextureSize("aTextureSize", textureSize);
    VertexAttrib aColor("aColor", color);
    VertexAttrib aTransform("aTransform", transform);

    std::vector<VertexAttribBase *> attribs = {&aPos, &aTexCoord, &aTextureAtlasPos, &aTextureUnit, &aTextureSize, &aColor, &aTransform};

    for (auto &a : attribs)
    {
        a->setDivisor(1);
    }

    // indices
    VertexIndices indices(indicesVec);

    // setup shader
    auto &[meshShader, instancedMeshShader, batchedMeshShader] = getShaders(registry, instances[0]);

    instancedMeshShader.use();

    instancedMeshShader.uniform(&uViewProjectionMatrix);
    instancedMeshShader.uniform(&uTextureAtlasSize);
    instancedMeshShader.uniform(&uTextures);

    if (registry.has<ShaderMaterial>(instances[0]))
    {
        auto &shaderMaterial = registry.get<ShaderMaterial>(instances[0]);
        instancedMeshShader.uniform(shaderMaterial.getUniforms());
    }

    instancedMeshShader.attrib(attribs);

    instancedMeshShader.indices(&indices);

    // draw
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

        const auto material = getMaterial(registry, e);
        const auto color = material->getColor().getColor();

        const auto texture = material->getTexture();
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

    auto atlasSize = glm::vec2(TextureAtlas::getAtlasSize());

    auto &textures = textureManager.getTexturesUniform();

    // view projection matrix
    auto viewProjectionMatrix = getViewProjectionMatrix(registry, camera);

    // uniforms
    Uniform uViewProjectionMatrix("uViewProjectionMatrix", viewProjectionMatrix);
    Uniform uTextureAtlasSize("uTextureAtlasSize", atlasSize);
    Uniform uTextures("uTextures", textures, true, textures.size(), GL_SAMPLER_2D);

    // attribs
    VertexAttrib aPos("aPos", batchedVertices);
    VertexAttrib aTexCoord("aTexCoord", batchedTexCoords);
    VertexAttrib aTextureAtlasPos("aTextureAtlasPos", batchedAtlasPos);
    VertexAttrib aTextureUnit("aTextureUnit", batchedTextureUnits);
    VertexAttrib aTextureSize("aTextureSize", batchedTextureSizes);
    VertexAttrib aColor("aColor", batchedColors);

    // indices
    VertexIndices indices(batchedIndices);

    // set up shader
    auto &shaders = getShaders(registry, renderables[0]);
    auto &batchedMeshShader = shaders.batchedMeshShader;

    batchedMeshShader.use();

    batchedMeshShader.uniform(&uViewProjectionMatrix);
    batchedMeshShader.uniform(&uTextureAtlasSize);
    batchedMeshShader.uniform(&uTextures);

    if (registry.has<ShaderMaterial>(renderables[0]))
    {
        auto &shaderMaterial = registry.get<ShaderMaterial>(renderables[0]);
        batchedMeshShader.uniform(shaderMaterial.getUniforms());

        if (shaderMaterial.getUniforms().contains("uTime"))
            std::cout << *static_cast<float *>(shaderMaterial.getUniforms().at("uTime")->getValuePointer()) << std::endl;
    }

    batchedMeshShader.attrib(&aPos);
    batchedMeshShader.attrib(&aTexCoord);
    batchedMeshShader.attrib(&aTextureAtlasPos);
    batchedMeshShader.attrib(&aTextureUnit);
    batchedMeshShader.attrib(&aTextureSize);
    batchedMeshShader.attrib(&aColor);

    batchedMeshShader.indices(&indices);

    // draw
    batchedMeshShader.draw(GL_TRIANGLES, indicesCount);
    batchedMeshShader.unbind();

    // std::cout << "draw time: " << Rendering::timeSinceEpochMillisec() - now << std::endl;
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

void Rendering::Renderer::enableDepthWrite(bool enable)
{
    if (enable)
    {
        glDepthMask(GL_TRUE);
    }
    else
    {
        glDepthMask(GL_FALSE);
    }
}

bool Rendering::Renderer::isDepthWriteEnabled() const
{
    GLboolean enabled;
    glGetBooleanv(GL_DEPTH_WRITEMASK, &enabled);

    return enabled == GL_TRUE;
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

glm::uvec2 Rendering::Renderer::getSize() const
{
    return glm::uvec2(width, height);
}

glm::uvec2 Rendering::Renderer::getWindowSize() const
{
    int width, height;
    glfwGetFramebufferSize(glfwWindow, &width, &height);

    return glm::uvec2(width, height);
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

void Rendering::Renderer::setUnbindUnusedTextures(bool unbind)
{
    unbindUnusedTextures = unbind;
}

bool Rendering::Renderer::getUnbindUnusedTextures() const
{
    return unbindUnusedTextures;
}

void Rendering::Renderer::setRenderTarget(RenderTarget *target)
{
    if (ownsRenderTarget)
    {
        delete renderTarget;
    }

    if (target == nullptr)
    {
        ownsRenderTarget = true;
        renderTarget = new RenderTarget(width, height);
        return;
    }

    ownsRenderTarget = target == renderTarget;
    renderTarget = target;
}

const Rendering::RenderTarget *const Rendering::Renderer::getRenderTarget() const
{
    return renderTarget;
}

Rendering::TextureManager &Rendering::Renderer::getTextureManager()
{
    return textureManager;
}

Rendering::RendererShaders &Rendering::Renderer::getShaders()
{
    return *shaders.at(DEFAULT_SHADER_KEY);
}

Rendering::RendererShaders &Rendering::Renderer::getShaders(const ShaderMaterial &material)
{
    auto key = material.getFragmentShaderKey();

    if (!shaders.contains(key))
    {
        shaders.emplace(key, new RendererShaders(material.getFragmentShader()));
    }

    return *shaders.at(key);
}

Rendering::RendererShaders &Rendering::Renderer::getShaders(const ECS::Registry &registry, const ECS::Entity entity)
{
    if (registry.has<ShaderMaterial>(entity))
    {
        auto &material = registry.get<ShaderMaterial>(entity);
        return getShaders(material);
    }
    else
    {
        return getShaders();
    }
}

std::unordered_map<Rendering::TextureId, Rendering::TextureManager::BoundTexture> Rendering::Renderer::bindTextures(const ECS::Registry &registry, const std::vector<ECS::Entity> &renderables)
{
    std::unordered_map<Rendering::TextureId, Rendering::TextureManager::BoundTexture> boundTextures;

    for (auto &e : renderables)
    {
        auto material = getMaterial(registry, e);

        auto texture = material->getTexture();

        if (!boundTextures.contains(texture->getId()))
        {
            // std::cout << "binding texture: " << texture->getId() << std::endl;
            // std::cout << "width: " << texture->getWidth() << ", height: " << texture->getHeight() << std::endl;
            // std::cout << "channels: " << texture->getChannels() << std::endl;

            if (material->isAnimated())
            {
                auto animatedTexture = material->getAnimatedTexture();
                auto &frames = animatedTexture->getFrames();

                for (auto &frame : frames)
                {
                    boundTextures.emplace(frame->getId(), textureManager.bind(frame));
                }
            }
            else
            {
                boundTextures.emplace(texture->getId(), textureManager.bind(texture));
            }
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
