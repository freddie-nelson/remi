#pragma once

#include <string>

namespace Rendering
{
    const std::string meshVertexShader =
        "#version 330 core\n"
        "\n"
        "uniform mat4 uViewProjectionMatrix;\n"
        "\n"
        "uniform mat4 uMeshTransform;\n"
        "\n"
        "uniform uint uTextureUnit;\n"
        "uniform vec2 uTextureSize;\n"
        "uniform vec2 uTextureAtlasPos;\n"
        "uniform vec2 uTextureAtlasSize;\n"
        "uniform vec4 uColor;\n"
        "\n"
        "in vec2 aPos;\n"
        "in vec2 aTexCoord;\n"
        "\n"
        "flat out uint vTextureUnit;\n"
        "out vec2 vTexCoord;\n"
        "out vec4 vColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   gl_Position = uViewProjectionMatrix * uMeshTransform * vec4(aPos, 0.0f, 1.0f);\n"
        "\n"
        "   // calculate texture coordinate in atlas\n"
        "   vec2 atlasTexCoord = (aTexCoord * uTextureSize) + uTextureAtlasPos;\n"
        "   vTexCoord = atlasTexCoord / uTextureAtlasSize;\n"
        "\n"
        "  vTextureUnit = uTextureUnit;\n"
        "  vColor = uColor;\n"
        "}\n";

    const std::string meshFragShader =
        "#version 330 core\n"
        "\n"
        "uniform sampler2D uTextures[__MAX_TEXTURE_UNITS__];\n"
        "\n"
        "flat in uint vTextureUnit;\n"
        "in vec2 vTexCoord;\n"
        "in vec4 vColor;\n"
        "\n"
        "out vec4 FragColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   FragColor = texture(uTextures[vTextureUnit], vTexCoord) * vColor;\n"
        "}\n";
}