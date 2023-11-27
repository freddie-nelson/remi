#pragma once

#include <string>

namespace Rendering
{
    const std::string instancedMeshVertexShader =
        "#version 330 core\n"
        "\n"
        "uniform vec2 uResolution;\n"
        "uniform mat4 uViewProjectionMatrix;\n"
        "\n"
        "in uint aZIndex;\n"
        "in vec2 aTranslation;\n"
        "in mat2 aTransform;\n"
        "in vec4 aColor;\n"
        "\n"
        "in vec2 aPos;\n"
        "\n"
        "out vec4 vColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   vec2 pos = aTransform * aPos;\n"
        "   pos += aTranslation;\n"
        "\n"
        "   gl_Position = vec4(pos, float(aZIndex), 1.0f);\n"
        "   gl_Position = uViewProjectionMatrix * gl_Position;\n"
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