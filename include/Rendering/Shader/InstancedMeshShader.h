#pragma once

#include <string>

namespace Rendering
{
    /**
     * The vertex shader for an instanced
     *
     * This shader needs access to the following uniforms:
     * - uViewProjectionMatrix: The view projection matrix to use.
     * - uPixelsPerMeter: The number of pixels per meter.
     * - uTextureAtlasSize: The size of the texture atlas.
     *
     * This shader needs access to the following inputs:
     * - aTransform: The transform of the mesh.
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
    const std::string instancedMeshVertexShader =
        "#version 300 es\n"
        "\n"
        "precision mediump float;\n"
        "\n"
        "uniform mat4 uViewProjectionMatrix;\n"
        "uniform uint uPixelsPerMeter;\n"
        "\n"
        "uniform vec2 uTextureAtlasSize;\n"
        "\n"
        "in mat4 aTransform;\n"
        "in vec2 aPos;\n"
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
        "   vec4 worldPos = aTransform * vec4(aPos, 0.0f, 1.0f);\n"
        "\n"
        "   // convert from meters to pixels\n"
        "   worldPos.xy *= float(uPixelsPerMeter);\n"
        "\n"
        "   gl_Position = uViewProjectionMatrix * worldPos;\n"
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