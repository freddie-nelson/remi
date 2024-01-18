#pragma once

#include <vector>

namespace Rendering
{
    /**
     * Represents vertex indices.
     */
    class VertexIndices
    {
    public:
        /**
         * Creates vertex indices.
         *
         * To edit the value of the indices, edit the underlying vector.
         *
         * @param indices The indices
         */
        VertexIndices(std::vector<unsigned int> &indices);

        /**
         * Gets the indices.
         *
         * @returns The indices.
         */
        std::vector<unsigned int> &get();

        /**
         * Gets the size of the indices.
         *
         * @returns The size of the indices.
         */
        size_t size() const;

    private:
        /**
         * The indices.
         */
        std::vector<unsigned int> &indices;
    };
}