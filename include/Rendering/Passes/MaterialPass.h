
#pragma once

#include "RenderPass.h"
#include "../Material/ShaderMaterial.h"

#include <unordered_map>
#include <vector>

namespace Rendering
{
    struct MaterialPassData
    {
        std::unordered_map<ShaderMaterial::FragShaderKey, std::vector<ECS::Entity>> opaqueRenderables;
        std::unordered_map<ShaderMaterial::FragShaderKey, std::vector<ECS::Entity>> transparentRenderables;
        std::vector<ShaderMaterial::FragShaderKey> keys;
    };

    /**
     * Represents a material pass.
     *
     * This pass separates the renderables into groups based on their material or shader material.
     *
     * This pass requires either the output of the renderables pass or the output of the culling pass.
     *
     * The transparent renderables are sorted by their z index, from back to front.
     */
    class MaterialPass : public RenderPass
    {
    public:
        /**
         * Creates a MaterialPass instance.
         */
        MaterialPass() = default;

        /**
         * Destroys the MaterialPass instance.
         */
        virtual ~MaterialPass() = default;

        /**
         * Executes the material pass.
         *
         * Requires either the output of the renderables pass or the output of the culling pass.
         *
         * @param input The input to the renderables pass.
         */
        RenderPassInput *execute(RenderPassInput *input) override;

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