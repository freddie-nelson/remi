#pragma once

#include "./Material/Material.h"
#include "./Material/ShaderMaterial.h"
#include "./Mesh/Mesh.h"
#include "./Shader/Shader.h"
#include "../Core/Transform.h"
#include "./Texture/TextureManager.h"
#include "../World/World.h"
#include "../ECS/Entity.h"
#include "../Core/BoundingCircle.h"
#include "../Core/AABB/AABBTree.h"
#include "../Core/Window.h"
#include "./RenderTarget.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/vec2.hpp>
#include <string>
#include <vector>
#include <unordered_map>

#define DEFAULT_SHADER_KEY 0

namespace Rendering
{
    /**
     * Represents the shaders used by the renderer.
     */
    struct RendererShaders
    {
        /**
         * Creates a new RendererShaders instance.
         *
         * @param fragmentShader The fragment shader to use for the shaders.
         */
        RendererShaders(const std::string &fragmentShader);

        Shader meshShader;
        Shader instancedMeshShader;
        Shader batchedMeshShader;
    };

    /**
     * The projection mode of the renderer.
     *
     * STRETCH: will stretch the rendered image to fit the window's size. This does not resize the active camera's size.
     * MATCH: will match the rendered image to the window's size. This resizes the active camera's size to match the renderer's size.
     */
    enum RendererProjectionMode
    {
        STRETCH,
        MATCH,
    };

    /**
     * The renderer is responsible for drawing entities.
     *
     * The renderer does not handle the window, it only handles the rendering through OpenGL.
     *
     * The acitve camera's size can be synced to the renderer's size, or it can be set manually. By default, the active camera's size is not synced.
     *
     * Depth testing is enabled by default.
     *
     * Alpha blending is disabled by default.
     *
     * The default alpha blending function is `GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA`.
     *
     * The renderer also contains a TextureManager for managing textures.
     *
     * By default the renderer will not unbind unused textures. This can be toggled with `setUnbindUnusedTextures`. It most likely is not needed, unless you have a lot of textures.
     */
    class Renderer : public World::System
    {
    public:
        /**
         * Creates a new renderer instance.
         *
         * @param window The window to attach the renderer to.
         * @param width The width of the renderer viewport.
         * @param height The height of the renderer viewport.
         * @param pixelsPerMeter The number of pixels per meter.
         * @param projectionMode The projection mode of the renderer.
         */
        Renderer(Core::Window *window, unsigned int width, unsigned int height, unsigned int pixelsPerMeter, RendererProjectionMode projectionMode = RendererProjectionMode::STRETCH);

        /**
         * Destroys the renderer.
         */
        virtual ~Renderer();

        /**
         * Updates the renderer.
         *
         * This will prune the AABB trees every `treePruneFrequency` updates.
         *
         * This will also update the active camera's size to match the renderer's size if `syncActiveCameraSizeWithRenderer` is true.
         *
         * This will also update the renderer's size to match the window size if `syncRendererSizeWithWindow` is true.
         *
         * This will also update the renderer's render target to match the renderer's size if the render target is not null.
         *
         * @param world The world to update from.
         * @param timestep The timestep since the last update.
         */
        void update(World::World &world, const Core::Timestep &timestep) override;

        /**
         * Clears the screen framebuffer and render target framebuffer.
         */
        void clear(bool clearColorBuffer = true, bool clearDepthBuffer = true, bool clearStencilBuffer = true) const;

        /**
         * Swaps the front and back buffers of the attached window, presenting the rendered image to the screen.
         */
        void present() const;

        /**
         * Renders the given entity to the screen.
         *
         * This does not cull entities outside the camera's view.
         *
         * @param world The world the entity belongs to.
         * @param camera The camera to use for rendering.
         * @param entity The entity to render, this entity must have atleast a Rendering::Mesh2D, Core::Transform and a Rendering::Material component.
         */
        void entity(const World::World &world, const ECS::Entity camera, const ECS::Entity &entity) const;

        /**
         * Renders the given entities using instanced rendering.
         *
         * The instances do not require a Rendering::Mesh2D component.
         *
         * This does not cull entities outside the camera's view.
         *
         * Uses the shaders from the first entity (`instances[0]`).
         *
         * @param world The world the entities belong to.
         * @param camera The camera to use for rendering.
         * @param mesh The mesh to use for each entity.
         * @param instances The entities to render, these entities must have atleast a Core::Transform and a Rendering::Material component.
         */
        void instance(const World::World &world, const ECS::Entity camera, const Rendering::Mesh2D &mesh, const std::vector<ECS::Entity> &instances) const;

        /**
         * Batches the given entities and draws them to the screen.
         *
         * This does not cull entities outside the camera's view.
         *
         * Uses the shaders from the first entity (`renderables[0]`).
         *
         * @param world The world the entities belong to.
         * @param camera The camera to use for rendering.
         * @param renderables The entities to render, these entities must have atleast a Rendering::Mesh2D, Core::Transform and a Rendering::Material component.
         */
        void batch(const World::World &world, const ECS::Entity camera, const std::vector<ECS::Entity> &renderables) const;

        /**
         * Sets the clear color.
         *
         * @param color The new clear color.
         */
        void setClearColor(const Color &color);

        /**
         * Returns the current clear color.
         *
         * @returns The clear color.
         */
        Color getClearColor() const;

        /**
         * Enables or disables depth testing.
         *
         * Depth testing is enabled by default.
         *
         * When depth testing is enabled, the renderer will only render the closest fragment.
         *
         * @param enable Whether to enable or disable depth testing.
         */
        void enableDepthTest(bool enable);

        /**
         * Returns whether depth testing is enabled.
         *
         * @returns whether depth testing is enabled.
         */
        bool isDepthTestEnabled() const;

        /**
         * Enables or disables writing to the depth buffer.
         *
         * Depth writing is enabled by default.
         *
         * When depth writing is enabled, the renderer will write the depth of the fragment to the depth buffer.
         *
         * @param enable Whether to enable or disable depth writing.
         */
        void enableDepthWrite(bool enable);

        /**
         * Returns whether depth writing is enabled.
         *
         * @returns whether depth writing is enabled.
         */
        bool isDepthWriteEnabled() const;

        /**
         * Enables or disables alpha blending.
         *
         * Alpha blending is disabled by default.
         *
         * When alpha blending is enabled, the renderer will blend the fragment color with the color already in the framebuffer.
         *
         * The correct sorting of objects is not handled by the renderer, it is up to the user to sort objects correctly.
         *
         * @param enable Whether to enable or disable alpha blending.
         */
        void enableAlphaBlending(bool enable);

        /**
         * Sets the alpha blending function.
         *
         * This is the sfactor and dfactor used in `glBlendFunc(sfactor, dfactor)`.
         *
         * By default, this is `GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA`.
         *
         * This does not enable alpha blending, use `enableAlphaBlending` to enable alpha blending.
         *
         * @param sfactor The sfactor to use in `glBlendFunc(sfactor, dfactor)`.
         * @param dfactor The dfactor to use in `glBlendFunc(sfactor, dfactor)`.
         *
         * @throws std::invalid_argument if sfactor or dfactor is not a valid value.
         */
        void setAlphaBlendingFunction(GLenum sfactor, GLenum dfactor);

        /**
         * Returns whether alpha blending is enabled.
         *
         * @returns whether alpha blending is enabled.
         */
        bool isAlphaBlendingEnabled() const;

        /**
         * Sets the width of the renderer.
         *
         * This will also set the glViewport.
         *
         * @param w The width of the renderer.
         */
        void setWidth(unsigned int w);

        /**
         * Returns the width of the renderer.
         *
         * @returns The width of the renderer.
         */
        unsigned int getWidth() const;

        /**
         * Sets the height of the renderer.
         *
         * This will also set the glViewport.
         *
         * @param h The height of the renderer.
         */
        void setHeight(unsigned int h);

        /**
         * Returns the height of the renderer.
         *
         * @returns The height of the renderer.
         */
        unsigned int getHeight() const;

        /**
         * Sets the width and height of the renderer.
         *
         * This will also set the glViewport.
         *
         * @param w The width of the renderer
         * @param h The height of the renderer
         */
        void setSize(unsigned int w, unsigned int h);

        /**
         * Sets the width and height of the renderer.
         *
         * This will also set the glViewport.
         *
         * @param size The width and height of the renderer.
         */
        void setSize(const glm::uvec2 &size);

        /**
         * Returns the width and height of the renderer.
         *
         * @returns The width and height of the renderer.
         */
        glm::uvec2 getSize() const;

        /**
         * Returns the width and height of the attached window.
         *
         * @returns The width and height of the attached window.
         */
        glm::uvec2 getWindowSize() const;

        /**
         * Gets the active camera entity.
         *
         * The active camera is used to render the scene in `update`.
         *
         * @param registry The registry to get the camera from.
         *
         * @returns The active camera entity.
         *
         * @throws std::runtime_error if there is no active camera.
         * @throws std::runtime_error if there is more than one active camera.
         * @throws std::runtime_error if the active camera does not have a Camera component.
         * @throws std::runtime_error if the active camera does not have a Transform component.
         */
        ECS::Entity getActiveCamera(const ECS::Registry &registry) const;

        /**
         * Whether the renderer should unbind unused textures.
         *
         * By default, this is true.
         *
         * This will call `textureManager.unbindUnusedTextures()` every update.
         *
         * @param unbind Whether to unbind unused textures.
         */
        void setUnbindUnusedTextures(bool unbind);

        /**
         * Returns whether the renderer is unbinding unused textures.
         *
         * @returns Whether the renderer is unbinding unused textures.
         */
        bool getUnbindUnusedTextures() const;

        /**
         * Sets the render target of the renderer.
         *
         * The renderer will output to the given target instead of the screen.
         *
         * The render target's size will be set to the renderer's size each update.
         *
         * The render target can be removed by setting the render target to nullptr.
         *
         * @param target The render target to render to.
         */
        void setRenderTarget(RenderTarget *texture);

        /**
         * Returns the render target of the renderer.
         *
         * @returns The render target of the renderer.
         */
        const RenderTarget *const getRenderTarget() const;

        /**
         * Returns the texture manager of the renderer.
         *
         * @returns The texture manager of the renderer.
         */
        TextureManager &getTextureManager();

        /**
         * Returns the projection mode of the renderer.
         *
         * @returns The projection mode of the renderer.
         */
        RendererProjectionMode getProjectionMode() const;

        /**
         * Sets the projection mode of the renderer.
         *
         * @param mode The new projection mode of the renderer.
         */
        void setProjectionMode(RendererProjectionMode mode);

    private:
        unsigned int width = 0;
        unsigned int height = 0;
        unsigned int pixelsPerMeter;

        RendererProjectionMode projectionMode = RendererProjectionMode::STRETCH;

        bool unbindUnusedTextures = false;
        mutable TextureManager textureManager;

        GLenum alphaBlendingSFactor = GL_SRC_ALPHA;
        GLenum alphaBlendingDFactor = GL_ONE_MINUS_SRC_ALPHA;

        Color clearColor = Color(0.0f);

        GLenum bufferDrawType = GL_STREAM_DRAW;

        Core::Window *window;

        bool ownsRenderTarget = true;
        RenderTarget *renderTarget;

        /**
         * The shaders used by the renderer.
         *
         * Key 0 is the default shaders.
         *
         * Other keys are the shaders for ShaderMaterials.
         */
        mutable std::unordered_map<ShaderMaterial::FragShaderKey, RendererShaders *> shaders;

        /**
         * Gets the default shaders.
         *
         * @returns The default shaders.
         *
         * @throws std::out_of_range if the default shaders do not exist.
         */
        RendererShaders &getShaders() const;

        /**
         * Gets the shaders for the given material.
         *
         * If the shaders do not exist, they will be created.
         *
         * @param material The material to get the shaders for.
         *
         * @returns The shaders for the given material.
         */
        RendererShaders &getShaders(const ShaderMaterial &material) const;

        /**
         * Gets the shaders for the given entity.
         *
         * Assumes the entity has a Material or ShaderMaterial component.
         *
         * If the entity has a ShaderMaterial component, the shaders for the ShaderMaterial will be returned.
         *
         * Otherwise the default shaders will be returned.
         *
         * @param registry The registry to read components from.
         * @param entity The entity to get the shaders for.
         *
         * @returns The shaders for the given entity.
         */
        RendererShaders &getShaders(const ECS::Registry &registry, const ECS::Entity entity) const;

        /**
         * Binds the textures of the given renderables.
         *
         * This function may error if all the textures cannot fit into the atlases.
         *
         * This function will avoid binding the same texture multiple times.
         *
         * @param registry The registry to read components from.
         * @param renderables The renderables to bind textures for.
         *
         * @returns A map of the texture ids to the bound textures.
         */
        std::unordered_map<TextureId, TextureManager::BoundTexture> bindTextures(const ECS::Registry &registry, const std::vector<ECS::Entity> &renderables) const;

        /**
         * Gets the view projection matrix for the given camera.
         *
         * Assumes the camera has a Camera and Transform component.
         *
         * @param world The world the camera belongs to.
         * @param camera The camera to get the view projection matrix for.
         *
         * @returns The view projection matrix for the given camera.
         */
        glm::mat4 getViewProjectionMatrix(const World::World &world, const ECS::Entity camera) const;
    };
}