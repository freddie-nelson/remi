
#pragma once

#include "RenderPass.h"
#include "../../Core/AABB/AABB.h"
#include "../../Core/AABB/AABBTree.h"
#include "../../Core/SpaceTransformer.h"

#include <vector>
#include <unordered_map>

namespace Rendering
{
    using CullingPassData = std::vector<ECS::Entity>;

    /**
     * Represents a culling pass.
     *
     * This pass culls the renderables that are not in the camera's view.
     *
     * This pass requires the output of the renderables pass.
     *
     * The output of this pass is a list of renderable entities that are in the camera's view.
     *
     * If multiple registries are used with the same instance of the culling pass, the AABB trees will become full of entities from all the registries. This could cause performance issues.
     */
    class CullingPass : public RenderPass
    {
    public:
        /**
         * Creates a CullingPass instance.
         */
        CullingPass() = default;

        /**
         * Destroys the CullingPass instance.
         */
        virtual ~CullingPass() = default;

        /**
         * Executes the culling pass.
         *
         * Requires the output of the renderables pass.
         *
         * @param input The input to the renderables pass.
         */
        RenderPassInput *execute(RenderPassInput *input) override;

        /**
         * Gets the name of the render pass.
         *
         * @returns The name of the render pass.
         */
        virtual constexpr std::string getName() override
        {
            return "CullingPass";
        };

    private:
        /**
         * Gets the AABB sufficient for culling entities outside the camera's view.
         *
         * @param world The world to use.
         * @param spaceTransformer The space transformer to use.
         * @param camera The camera to get the culling AABB for.
         *
         * @returns The AABB sufficient for culling entities outside the camera's view.
         */
        Core::AABB getCullingAABB(World::World &world, const Core::SpaceTransformer &spaceTransformer, const ECS::Entity camera) const;

        /**
         * The number of calls to getRenderables to wait before pruning the AABB tree.
         *
         * Pruning involves removing entities that are no longer in the registry.
         */
        unsigned int treePruneFrequency = 1000;

        /**
         * The number of calls to getRenderables since the last prune.
         */
        unsigned int callsSinceLastTreePrune = 0;

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
         *
         * The tree has no margin.
         */
        Core::AABBTree<ECS::Entity> staticRenderablesTree = Core::AABBTree<ECS::Entity>(0.0f);

        /**
         * Dynamic renderables and their previously computed aabbs.
         */
        std::unordered_map<ECS::Entity, Core::AABB> dynamicRenderables;

        /**
         * The AABB tree for dynamic renderables.
         *
         * The tree gives fat aabb's a margin of 2 metres.
         */
        Core::AABBTree<ECS::Entity> dynamicRenderablesTree = Core::AABBTree<ECS::Entity>(2.0f);

        /**
         * Populates the renderables vector with all the entities that are inside the given aabb.
         *
         * Also prunes the AABB trees, every `treePruneFrequency` calls.
         *
         * The entities provided must have a `renderable.isStatic` that matches the `isStatic` parameter.
         *
         * @param world The world to use.
         * @param entities The entities to check.
         * @param viewAabb The aabb to check against.
         * @param isStatic When true only static entities will be checked, when false only non static entities are check.
         * @param renderables The vector to populate with the entities that are inside the given aabb.
         *
         * @returns The number of entities of the other renderable type, i.e. returns number of static entities when isStatic is false, and vice versa.
         */
        void getRenderables(World::World &world, const std::vector<ECS::Entity> &entities, const Core::AABB &viewAabb, bool isStatic, std::vector<ECS::Entity> &renderables);

        /**
         * Draws the AABB tree.
         *
         * @param tree The tree to draw.
         * @param world The world to use.
         * @param renderer The renderer to use.
         * @param renderTarget The render target to use.
         * @param textureManager The texture manager to use.
         */
        void drawAABBTree(const Core::AABBTree<ECS::Entity> &tree, World::World &world, const Rendering::Renderer &renderer, const Rendering::RenderTarget &renderTarget, Rendering::TextureManager &textureManager) const;
    };
}