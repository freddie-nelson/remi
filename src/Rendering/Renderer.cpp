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
    meshShader = Shader();
    if (!meshShader.loadFromSource(meshVertexShader, fragmentShader))
    {
        throw std::runtime_error("Failed to load mesh shader.");
    }

    instancedMeshShader = Shader();
    if (!instancedMeshShader.loadFromSource(instancedMeshVertexShader, fragmentShader))
    {
        throw std::runtime_error("Failed to load instanced mesh shader.");
    }

    batchedMeshShader = Shader();
    if (!batchedMeshShader.loadFromSource(batchedMeshVertexShader, fragmentShader))
    {
        throw std::runtime_error("Failed to load batched mesh shader.");
    }
}

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

    // init shaders
    shaders.emplace(DEFAULT_SHADER_KEY, RendererShaders(meshFragShader));

    shaders.at(DEFAULT_SHADER_KEY).batchedMeshShader.use();
}

void Rendering::Renderer::update(const ECS::Registry &registry, const Core::Timestep &timestep)
{
    if (syncRendererSizeWithWindow)
    {
        auto size = getWindowSize();
        setSize(size.x, size.y);
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
    std::vector<ECS::Entity> staticRenderables;

    // cull static renderables
    auto dynamicsCount = getRenderables(registry, entities, fatCameraAabb, false, staticRenderables);

    // cull dynamic renderables
    std::vector<ECS::Entity> renderables;

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

    // now = Rendering::timeSinceEpochMillisec();

    if (staticRenderables.size() != 0)
    {
        batchRenderables(registry, activeCamera, staticRenderables, true);
    }

    if (renderables.size() != 0)
    {
        batchRenderables(registry, activeCamera, renderables, false);
    }
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
    const std::vector<unsigned int> &indicesVec = mesh.getIndices();
    const std::vector<glm::vec2> &uvs = mesh.getUvs();

    // material data
    auto *material = getMaterial(registry, entity);
    auto color = material->getColor().getColor();

    auto texture = material->getTexture();
    auto boundTexture = textureManager.bind(texture);
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
    auto &[meshShader, instancedMeshShader, batchedMeshShader] = getShaders(registry, renderables[0]);

    batchedMeshShader.use();

    std::cout << "renderables size: " << renderables.size() << std::endl;

    batchedMeshShader.uniform(&uViewProjectionMatrix);
    batchedMeshShader.uniform(&uTextureAtlasSize);
    batchedMeshShader.uniform(&uTextures);

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

void Rendering::Renderer::sortRenderables(const ECS::Registry &registry, std::vector<ECS::Entity> &renderables)
{
    std::map<unsigned int, std::vector<ECS::Entity>> layers;

    for (auto &e : renderables)
    {
        auto &transform = registry.get<Core::Transform>(e);
        auto zIndex = transform.getZIndex();

        if (!layers.contains(zIndex))
        {
            layers.emplace(zIndex, std::vector<ECS::Entity>());
        }

        layers[zIndex].push_back(e);
    }

    renderables.clear();

    for (auto &layer : layers)
    {
        for (auto &e : layer.second)
        {
            renderables.push_back(e);
        }
    }

    // first renderable should be at lowest index and last renderable at highest index

    // auto first = renderables.front();
    // auto &firstTransform = registry.get<Core::Transform>(first);

    // auto last = renderables.back();
    // auto &lastTransform = registry.get<Core::Transform>(last);

    // std::cout << "first: " << firstTransform.getZIndex() << ", last: " << lastTransform.getZIndex() << std::endl;
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

Rendering::RendererShaders &Rendering::Renderer::getShaders()
{
    return shaders.at(DEFAULT_SHADER_KEY);
}

Rendering::RendererShaders &Rendering::Renderer::getShaders(const ShaderMaterial &material)
{
    auto key = material.getFragmentShaderKey();

    if (!shaders.contains(key))
    {
        shaders.emplace(key, RendererShaders(material.getFragmentShader()));
    }

    return shaders.at(key);
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

Rendering::Material *Rendering::Renderer::getMaterial(const ECS::Registry &registry, const ECS::Entity entity) const
{
    Material *material = nullptr;

    if (registry.has<ShaderMaterial>(entity))
    {
        material = &registry.get<ShaderMaterial>(entity);
    }
    else
    {
        material = &registry.get<Material>(entity);
    }

    return material;
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

void Rendering::Renderer::batchRenderables(const ECS::Registry &registry, const ECS::Entity activeCamera, const std::vector<ECS::Entity> &renderables, bool isStatic)
{
    // split renderables into opaque and transparent
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
            // std::cout << "binding texture: " << texture->getId() << std::endl;
            // std::cout << "width: " << texture->getWidth() << ", height: " << texture->getHeight() << std::endl;
            // std::cout << "channels: " << texture->getChannels() << std::endl;

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
