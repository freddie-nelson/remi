#pragma once

#include <string>

namespace Rendering
{
    const std::string instancedMeshVertexShader =
        "#version 330 core\n"
        "\n"
        "uniform mat4 uViewProjectionMatrix;\n"
        "\n"
        "in mat4 aTransform;\n"
        "in vec4 aColor;\n"
        "\n"
        "in vec2 aPos;\n"
        "\n"
        "out vec4 vColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   gl_Position = uViewProjectionMatrix * aTransform * vec4(aPos, 0.0f, 1.0f);\n"
        "   vColor = aColor;\n"
        "}\n";

    const std::string instancedMeshFragShader =
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