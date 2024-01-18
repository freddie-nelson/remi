#include "../../../include/Rendering/Shader/VertexIndices.h"

Rendering::VertexIndices::VertexIndices(const std::vector<unsigned int> &indices) : indices(indices)
{
}

const std::vector<unsigned int> &Rendering::VertexIndices::get()
{
    return indices;
}

size_t Rendering::VertexIndices::size() const
{
    return indices.size();
}