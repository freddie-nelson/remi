#pragma once

#include <string>

namespace Rendering
{
    const std::string batchedMeshVertexShader =
        "#version 330 core\n"
        "\n"
        "uniform mat4 uViewProjectionMatrix;\n"
        "\n"
        "in vec4 aPos;\n"
        "in vec4 aColor;\n"
        "\n"
        "out vec4 vColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   vColor = aColor;\n"
        "   gl_Position = uViewProjectionMatrix * aPos;\n"
        "}\n";

    const std::string batchedMeshFragShader =
        "#version 330 core\n"
        "\n"
        "in vec4 vColor;\n"
        "\n"
        "out vec4 FragColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   FragColor = vColor;\n"
        "}\n";
}