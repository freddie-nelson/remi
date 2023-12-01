#pragma once

#include <string>

namespace Rendering
{
    const std::string meshVertexShader =
        "#version 330 core\n"
        "\n"
        "uniform mat4 uViewProjectionMatrix;\n"
        "uniform mat4 uMeshTransform;\n"
        "\n"
        "in vec2 aPos;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   gl_Position = uViewProjectionMatrix * uMeshTransform * vec4(aPos, 0.0f, 1.0f);\n"
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