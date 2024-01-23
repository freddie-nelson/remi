#pragma once

#include "./Material/Material.h"
#include "./Mesh/Mesh.h"
#include "./Shader/Shader.h"
#include "../Core/Transform.h"
#include "./Texture/TextureManager.h"
#include "../ECS/System.h"
#include "../ECS/Entity.h"
#include "../Core/BoundingCircle.h"
#include "../Core/AABB/AABBTree.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/vec2.hpp>
#include <string>
#include <vector>
#include <unordered_map>

namespace Rendering
{

    /**
     * The renderer is responsible for rendering entities to the screen.
     *
     * It is also responsible for clearing the screen and swapping the front and back buffers.
     *
     * The renderer does not handle the window, it only handles the rendering through OpenGL.
     *
     * The renderer also handles the camera, by default it will create a camera with the same size as the renderer.
     * The camera's size can be synced to the renderer's size, or it can be set manually. By default, the camera's size is not synced.
     *
     * Depth testing is enabled by default.
     *
     * Alpha blending is disabled by default.
     *
     * The default alpha blending function is `GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA`.
     *
     * The renderer also contains a TextureManager for managing textures.
     */
    class Renderer : public ECS::System
    {
    public:
        /**
         * Creates a new renderer attached to the given GLFWwindow.
         *
         * Will also create a default camera with the given width and height, and centre at (0, 0).
         *
         * @param glfwWindow The GLFWwindow to attach to.
         * @param width The width of the renderer viewport.
         * @param height The height of the renderer viewport.
         */
        Renderer(GLFWwindow *glfwWindow, int width, int height);

        /**
         * Destroys the renderer.
         */
        virtual ~Renderer();

        /**
         * Initializes the renderer.
         */
        void init();

        /**
         * Updates the renderer.
         *
         * This renders all entities in the registry with a Mesh2D, Transform, Material and Renderable component.
         *
         * Culls entities outside the camera's view.
         *
         * This also prunes the AABB trees.
         *
         * This also sorts the renderables by their z index, when alpha blending is enabled.
         *
         * @param registry The registry to render from.
         * @param timestep The timestep since the last update.
         */
        void update(const ECS::Registry &registry, const Core::Timestep &timestep) override;

        /**
         * Clears the render buffers.
         */
        void clear(bool clearColorBuffer = true, bool clearDepthBuffer = true, bool clearStencilBuffer = true) const;

        /**
         * Swaps the front and back buffers, presenting the rendered image to the screen.
         */
        void present() const;

        /**
         * Renders the given entity to the screen.
         *
         * This does not cull entities outside the camera's view.
         *
         * @param registry The registry to read components from.
         * @param camera The camera to use for rendering.
         * @param entity The entity to render, this entity must have atleast a Rendering::Mesh2D, Core::Transform and a Rendering::Material component.
         */
        void entity(const ECS::Registry &registry, const ECS::Entity camera, ECS::Entity &entity);

        /**
         * Renders the given entities using instanced rendering.
         *
         * The instances do not require a Rendering::Mesh2D component.
         *
         * This does not cull entities outside the camera's view.
         *
         * @param registry The registry to read components from.
         * @param camera The camera to use for rendering.
         * @param mesh The mesh to use for each entity.
         * @param instances The entities to render, these entities must have atleast a Core::Transform and a Rendering::Material component.
         */
        void instance(const ECS::Registry &registry, const ECS::Entity camera, const Rendering::Mesh2D &mesh, const std::vector<ECS::Entity> &instances);

        /**
         * Batches the given entities and draws them to the screen.
         *
         * This does not cull entities outside the camera's view.
         *
         * @param registry The registry to read components from.
         * @param camera The camera to use for rendering.
         * @param renderables The entities to render, these entities must have atleast a Rendering::Mesh2D, Core::Transform and a Rendering::Material component.
         */
        void batch(const ECS::Registry &registry, const ECS::Entity camera, const std::vector<ECS::Entity> &renderables);

        /**
         * Sorts the given renderables by their z index.
         *
         * The renderables are sorted in ascending order, i.e. the renderable with the lowest z index will be first in the vector.
         *
         * This insures that the renderables work correctly with alpha blending.
         *
         * The renderables are sorted in place.
         *
         * @param registry The registry to read components from.
         * @param renderables The renderables to sort.
         */
        void sortRenderables(const ECS::Registry &registry, std::vector<ECS::Entity> &renderables);

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
         * Sets the width and height of the renderer.
         *
         * This will also set the glViewport.
         *
         * @param w The width of the renderer
         * @param h The height of the renderer
         */
        void setSize(int w, int h);

        /**
         * Returns the width and height of the renderer.
         *
         * @returns The width and height of the renderer.
         */
        glm::uvec2 getSize() const;

        /**
         * Returns the width and height of the attached GLFWwindow.
         *
         * @returns The width and height of the attached GLFWwindow.
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
         * Gets the AABB sufficient for culling entities outside the camera's view.
         *
         * @param registry The registry to get the camera's components from.
         * @param camera The camera to get the culling AABB for.
         *
         * @returns The AABB sufficient for culling entities outside the camera's view.
         */
        Core::AABB getCullingAABB(const ECS::Registry &registry, const ECS::Entity camera) const;

        /**
         * Sets whether the renderer should sync it's size with the window size.
         *
         * By default, this is true.
         *
         * When true, the renderer will set it's size to the window size on update.
         *
         * @param sync Whether to sync the renderer's size with the window size.
         */
        void syncSizeWithWindow(bool sync);

        /**
         * Gets whether the renderer is syncing it's size with the window size.
         *
         * @returns Whether the renderer is syncing it's size with the window size.
         */
        bool getSyncSizeWithWindow() const;

        /**
         * Sets whether the renderer should update the active camera size to match the renderer size on update.
         *
         * By default, this is false.
         *
         * @param sync Whether to sync the active camera size to the renderer size.
         */
        void syncActiveCameraSize(bool sync);

        /**
         * Returns whether the renderer is syncing the active camera size to the renderer size.
         *
         * @returns Whether the renderer is syncing the active camera size to the renderer size.
         */
        bool getSyncActiveCameraSize() const;

    private:
        int width;
        int height;

        /**
         * The number of updates to wait before pruning the AABB tree.
         *
         * Pruning involves removing entities that are no longer in the registry.
         */
        unsigned int treePruneFrequency = 60;

        /**
         * The number of updates since the last prune.
         */
        unsigned int updatesSinceLastTreePrune = 0;

        /**
         * Prunes the AABB trees.
         *
         * This involves removing entities no longer have a Renderable component, or are no longer in the registry.
         *
         * @param registry The registry to prune against.
         */
        void pruneTrees(const ECS::Registry &registry);

        /**
         * Static renderables and their previously computed aabbs.
         */
        std::unordered_map<ECS::Entity, Core::AABB> staticRenderables;

        /**
         * The AABB tree for static renderables.
         */
        Core::AABBTree<ECS::Entity> staticRenderablesTree;

        /**
         * Dynamic renderables and their previously computed aabbs.
         */
        std::unordered_map<ECS::Entity, Core::AABB> dynamicRenderables;

        /**
         * The AABB tree for dynamic renderables.
         *
         * The tree gives fat aabb's a margin of 100 pixels.
         */
        Core::AABBTree<ECS::Entity> dynamicRenderablesTree = Core::AABBTree<ECS::Entity>(100.0f);

        /**
         * Populates the renderables vector with all the entities that are inside the given aabb.
         *
         * @param registry The registry to read components from.
         * @param entities The entities to check.
         * @param viewAabb The aabb to check against.
         * @param isStatic When true only static entities will be checked, when false only non static entities are check.
         * @param renderables The vector to populate with the entities that are inside the given aabb.
         *
         * @returns The number of entities of the other renderable type, i.e. returns number of static entities when isStatic is false, and vice versa.
         */
        size_t getRenderables(const ECS::Registry &registry, const std::vector<ECS::Entity> &entities, const Core::AABB &viewAabb, bool isStatic, std::vector<ECS::Entity> &renderables);

        /**
         * Renders the given renderables.
         *
         * This does not cull entities outside the camera's view.
         *
         * @param registry The registry to read components from.
         * @param camera The camera to use for rendering.
         * @param renderables The renderables to render.
         * @param isStatic Whether the renderables are static or not.
         */
        void batchRenderables(const ECS::Registry &registry, const ECS::Entity activeCamera, const std::vector<ECS::Entity> &renderables, bool isStatic);

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
        std::unordered_map<TextureId, TextureManager::BoundTexture> bindTextures(const ECS::Registry &registry, const std::vector<ECS::Entity> &renderables);

        /**
         * Whether the renderer should sync it's size with the window size.
         *
         * By default, this is true.
         *
         * When true, the renderer will set it's size to the window size on update.
         */
        bool syncRendererSizeWithWindow = true;

        /**
         * Whether the renderer should sync the active camera size to the renderer size.
         *
         * By default, this is false.
         *
         * When this is true, the renderer will modify the active camera's viewport size to match the renderer's size.
         */
        bool syncActiveCameraSizeWithRenderer = false;

        /**
         * Gets the view projection matrix for the given camera.
         *
         * Assumes the camera has a Camera and Transform component.
         *
         * @param registry The registry to read components from.
         * @param camera The camera to get the view projection matrix for.
         *
         * @returns The view projection matrix for the given camera.
         */
        glm::mat4 getViewProjectionMatrix(const ECS::Registry &registry, const ECS::Entity camera) const;

        TextureManager textureManager;

        Shader meshShader;
        Shader instancedMeshShader;
        Shader batchedMeshShader;

        GLenum alphaBlendingSFactor = GL_SRC_ALPHA;
        GLenum alphaBlendingDFactor = GL_ONE_MINUS_SRC_ALPHA;

        Color clearColor = Color(0.0f);

        GLFWwindow *glfwWindow;

        GLenum bufferDrawType = GL_STREAM_DRAW;
    };
}