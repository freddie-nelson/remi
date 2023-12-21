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
        "uniform vec2 uTextureSize;\n"
        "uniform vec2 uTextureAtlasPos;\n"
        "uniform vec2 uTextureAtlasSize;\n"
        "\n"
        "in vec2 aPos;\n"
        "in vec2 aTexCoord;\n"
        "\n"
        "out vec2 vTexCoord;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   gl_Position = uViewProjectionMatrix * uMeshTransform * vec4(aPos, 0.0f, 1.0f);\n"
        "\n"
        "   // calculate texture coordinate in atlas\n"
        "   vec2 atlasTexCoord = (aTexCoord * uTextureSize) + uTextureAtlasPos;\n"
        "   vTexCoord = atlasTexCoord / uTextureAtlasSize;\n"
        "}\n";

    const std::string meshFragShader =
        "#version 330 core\n"
        "\n"
        "uniform sampler2D uTexture;\n"
        "uniform vec4 uColor;\n"
        "\n"
        "in vec2 vTexCoord;\n"
        "\n"
        "out vec4 FragColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   FragColor = texture(uTexture, vTexCoord) * uColor;\n"
        "}\n";
}