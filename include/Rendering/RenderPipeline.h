#pragma once

#include "./Passes/RenderPass.h"

#include <map>

namespace Rendering
{
    /**
     * Represents a render pipeline.
     *
     * A render pipeline is a series of passes that render the scene.
     *
     * Each pass transforms the entities in the scene in some way, eventually rendering them to the screen.
     *
     * The render pipeline passes and orders can be found by calling toString() and outputting the result.
     */
    class RenderPipeline
    {
    public:
        /**
         * Creates a RenderPipeline instance.
         */
        RenderPipeline();

        /**
         * Destroys the RenderPipeline instance.
         */
        ~RenderPipeline();

        /**
         * Executes the render pipeline.
         *
         * This will execute each pass in the render pipeline in order.
         *
         * @param input The input to the first pass in the render pipeline.
         */
        void execute(RenderPassInput *input);

        /**
         * Adds a pass to the render pipeline.
         *
         * @param pass The pass to add.
         * @param order The order in which to execute the pass.
         */
        void add(RenderPass *pass, unsigned int order);

        /**
         * Removes a pass from the render pipeline.
         *
         * @param pass The pass to remove.
         */
        void remove(RenderPass *pass);

        /**
         * Gets a pass from the render pipeline.
         *
         * @param order The order of the pass to get.
         */
        RenderPass *get(unsigned int order) const;

        /**
         * Checks if the render pipeline has a pass with the given order.
         *
         * @param order The order of the pass to check for.
         *
         * @returns Whether the render pipeline has a pass with the given order.
         */
        bool has(unsigned int order) const;

        /**
         * Gets the passes and orders of passes in the pipeline as a human readable string.
         *
         * @returns The passes and orders of passes in the pipeline as a human readable string.
         */
        std::string toString() const;

    private:
        /**
         * The passes in the render pipeline.
         *
         * The key is the order in which the pass is executed.
         */
        std::map<unsigned int, RenderPass *> passes;
    };
}