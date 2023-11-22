#pragma once

#include <string>

namespace Rendering
{
    const std::string meshVertexShader =
        "#version 460 core\n"
        "\n"
        "uniform vec2 uResolution;\n"
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
        "   // convert from pixel coordinates to normalized device coordinates\n"
        "   // with the origin at the center of the screen\n"
        "   pos = (pos / uResolution) * 2.0f;\n"
        "\n"
        "   gl_Position = vec4(pos, 0.0f, 1.0f);\n"
        "}\n";

    const std::string meshFragShader =
        "#version 460 core\n"
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