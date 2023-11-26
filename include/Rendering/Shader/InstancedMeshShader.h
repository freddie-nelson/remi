#pragma once

#include <string>

namespace Rendering
{
    const std::string instancedMeshVertexShader =
        "#version 330 core\n"
        "\n"
        "uniform vec2 uResolution;\n"
        "\n"
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
        "   // convert from pixel coordinates to normalized device coordinates\n"
        "   // with the origin at the center of the screen\n"
        "   pos = (pos / uResolution) * 2.0f;\n"
        "\n"
        "   gl_Position = vec4(pos, 0.0f, 1.0f);\n"
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