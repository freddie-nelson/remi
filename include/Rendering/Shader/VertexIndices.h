#pragma once

#include <vector>

namespace Rendering
{
    class VertexIndices
    {
    public:
        /**
         * Creates vertex indices.
         *
         * The indices will be copied into the vertex indices, use std::move to move the indices into the vertex indices.
         *
         * @param indices The indices
         */
        VertexIndices(std::vector<unsigned int> indices);

        /**
         * Sets the indices.
         *
         * @param indices The indices
         */
        void set(std::vector<unsigned int> indices);

        /**
         * Gets the indices.
         *
         * @returns The indices.
         */
        const std::vector<unsigned int> &get();

    private:
        /**
         * The indices.
         */
        std::vector<unsigned int> indices;
    };
}