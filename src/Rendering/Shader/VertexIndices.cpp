#include "../../../include/Rendering/Shader/VertexIndices.h"

Rendering::VertexIndices::VertexIndices(std::vector<unsigned int> indices) : indices(indices)
{
}

void Rendering::VertexIndices::set(std::vector<unsigned int> indices)
{
    this->indices = indices;
}

const std::vector<unsigned int> &Rendering::VertexIndices::get()
{
    return indices;
}