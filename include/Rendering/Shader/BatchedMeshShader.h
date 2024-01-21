#pragma once

#include <string>

namespace Rendering
{
    /**
     * The vertex shader for a batched mesh.
     *
     * This shader needs access to the following uniforms:
     * - uViewProjectionMatrix: The view projection matrix to use.
     * - uTextureAtlasSize: The size of the texture atlas.
     *
     * This shader needs access to the following inputs:
     * - aPos: The position of the vertex.
     * - aTextureAtlasPos: The position of the texture within the texture atlas.
     * - aTextureUnit: The texture unit to use. This is the texture atlas.
     * - aTextureSize: The size of the texture within the atlas.
     * - aTexCoord: The UV coordinate of the vertex.
     * - aColor: The color of the vertex.
     *
     * This shader must have the following outputs:
     * - gl_Position: The position of the vertex.
     * - vTextureUnit: The texture unit to use. This is the texture atlas.
     * - vTexCoord: The texture coordinate to use. This is the texture coordinate within the atlas.
     * - vColor: The color to use.
     * - vUv: The UV coordinate. This is the UV coordinate of the vertex.
     */
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
        "out vec2 vUv;\n"
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
        "  vUv = aTexCoord;\n"
        "}\n";

    // uses the same fragment shader as MeshShader
}