#pragma once

#include <string>

namespace Rendering
{
    const std::string batchedMeshVertexShader =
        "#version 300 es\n"
        "\n"
        "precision mediump float;\n"
        "\n"
        "uniform mat4 uViewProjectionMatrix;\n"
        "\n"
        "uniform vec2 uTextureAtlasSize;\n"
        "\n"
        "in vec4 aPos;\n"
        "\n"
        "in vec2 aTextureAtlasPos;\n"
        "in uint aTextureUnit;\n"
        "in vec2 aTextureSize;\n"
        "in vec2 aTexCoord;\n"
        "in vec4 aColor;\n"
        "\n"
        "flat out uint vTextureUnit;\n"
        "out vec2 vTexCoord;\n"
        "out vec4 vColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   gl_Position = uViewProjectionMatrix * aPos;\n"
        "\n"
        "   // calculate texture coordinate in atlas\n"
        "   vec2 atlasTexCoord = (aTexCoord * aTextureSize) + aTextureAtlasPos;\n"
        "   vTexCoord = atlasTexCoord / uTextureAtlasSize;\n"
        "\n"
        "  vTextureUnit = aTextureUnit;\n"
        "  vColor = aColor;\n"
        "}\n";

    // uses the same fragment shader as MeshShader
}