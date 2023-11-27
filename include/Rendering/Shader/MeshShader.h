#pragma once

#include <string>

namespace Rendering
{
    const std::string meshVertexShader =
        "#version 330 core\n"
        "\n"
        "uniform mat4 uViewProjectionMatrix;\n"
        "uniform uint uMaxZIndex;\n"
        "\n"
        "uniform uint uMeshZIndex;\n"
        "uniform vec2 uMeshTranslation;\n"
        "uniform mat2 uMeshTransform;\n"
        "\n"
        "in vec2 aPos;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   vec2 pos = uMeshTransform * aPos;\n"
        "   pos += uMeshTranslation;\n"
        "\n"
        "   // zIndex is flipped and converted to float so 0 is below everything, then multplied by -1 since the camera is looking down the negative z axis.\n"
        "   float zIndex = float(uMaxZIndex - uMeshZIndex) * -1.0f;\n"
        "\n"
        "   gl_Position = vec4(pos, zIndex, 1.0f);\n"
        "   gl_Position = uViewProjectionMatrix * gl_Position;\n"
        "}\n";

    const std::string meshFragShader =
        "#version 330 core\n"
        "\n"
        "uniform vec4 uColor;\n"
        "\n"
        "out vec4 FragColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   FragColor = uColor;\n"
        "}\n";
}