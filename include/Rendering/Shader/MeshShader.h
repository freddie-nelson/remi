#pragma once

#include <string>

namespace Rendering
{
    /**
     * The vertex shader for a mesh.
     *
     * This shader needs access to the following uniforms:
     * - uViewProjectionMatrix: The view projection matrix to use.
     * - uPixelsPerMeter: The number of pixels per meter.
     * - uMeshTransform: The transform of the mesh.
     * - uTextureUnit: The texture unit to use. This is the texture atlas.
     * - uTextureSize: The size of the texture within the atlas.
     * - uTextureAtlasPos: The position of the texture within the texture atlas.
     * - uTextureAtlasSize: The size of the texture atlas.
     * - uColor: The color to use.
     *
     * This shader must have the following outputs:
     * - gl_Position: The position of the vertex.
     * - vTextureUnit: The texture unit to use. This is the texture atlas.
     * - vTexCoord: The texture coordinate to use. This is the texture coordinate within the atlas.
     * - vColor: The color to use.
     * - vUv: The UV coordinate. This is the UV coordinate of the vertex.
     */
    const std::string meshVertexShader =
        "#version 300 es\n"
        "\n"
        "precision mediump float;\n"
        "\n"
        "uniform mat4 uViewProjectionMatrix;\n"
        "uniform uint uPixelsPerMeter;\n"
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
        "out vec2 vUv;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   vec4 worldPos = uMeshTransform * vec4(aPos, 0.0f, 1.0f);\n"
        "\n"
        "   // convert from meters to pixels\n"
        "   worldPos.xy *= float(uPixelsPerMeter);\n"
        "\n"
        "   gl_Position = uViewProjectionMatrix * worldPos;\n"
        "\n"
        "   // calculate texture coordinate in atlas\n"
        "   vec2 atlasTexCoord = (aTexCoord * uTextureSize) + uTextureAtlasPos;\n"
        "   vTexCoord = atlasTexCoord / uTextureAtlasSize;\n"
        "\n"
        "  vTextureUnit = uTextureUnit;\n"
        "  vColor = uColor;\n"
        "  vUv = aTexCoord;\n"
        "}\n";

    /**
     * The fragment shader for a mesh.
     *
     * This shader needs access to the following uniforms:
     * - uTextures: The currently bound texture atlases.
     *
     * This shader needs access to the following inputs:
     * - vTextureUnit: The texture unit to use. This is the texture atlas.
     * - vTexCoord: The texture coordinate within the atlas.
     * - vColor: The color to use.
     * - vUv: The UV coordinate.
     *
     * This shader must have the following outputs:
     * - FragColor: The color of the fragment.
     */
    const std::string meshFragShader =
        "#version 300 es\n"
        "\n"
        "precision mediump float;\n"
        "\n"
        "uniform sampler2D uTextures[__MAX_TEXTURE_UNITS__];\n"
        "\n"
        "flat in uint vTextureUnit;\n"
        "in vec2 vTexCoord;\n"
        "in vec4 vColor;\n"
        "in vec2 vUv;\n"
        "\n"
        "out vec4 FragColor;\n"
        "\n"
        "__getTexture__"
        "\n"
        "void main()\n"
        "{\n"
        "   FragColor = getTexture(vTextureUnit, vTexCoord) * vColor;\n"
        "}\n";
}