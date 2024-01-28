
#pragma once

#include "RenderPass.h"
#include "../Material/ShaderMaterial.h"

#include <unordered_map>
#include <vector>

namespace Rendering
{
    /**
     * Represents a batch.
     *
     * A batch is a collection of renderables that can be rendered with the same shader.
     *
     * If the renderables are transparent, they are sorted by their z index, from back to front.
     */
    struct Batch
    {
        bool transparent;
        ShaderMaterial::FragShaderKey key;
        std::vector<ECS::Entity> renderables;
    };

    using BatchPassData = std::vector<Batch>;

    /**
     * Represents a batch pass.
     *
     * This pass separates the renderables into batches based on their material or shader material.
     *
     * The transparent renderables are sorted by their z index, from back to front.
     *
     * This pass requires either the output of the renderables pass or the output of the culling pass.
     */
    class BatchPass : public RenderPass
    {
    public:
        /**
         * Creates a BatchPass instance.
         */
        BatchPass() = default;

        /**
         * Destroys the BatchPass instance.
         */
        virtual ~BatchPass() = default;

        /**
         * Executes the batch pass.
         *
         * Requires either the output of the renderables pass or the output of the culling pass.
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
            return "BatchPass";
        };

    private:
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
    };
}