#pragma once

#include <string>

namespace Rendering
{
    const std::string instancedMeshVertexShader =
        "#version 330 core\n"
        "\n"
        "uniform mat4 uViewProjectionMatrix;\n"
        "uniform uint uMaxZIndex;\n"
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
        "   // zIndex is flipped and converted to float so 0 is below everything, then multplied by -1 since the camera is looking down the negative z axis.\n"
        "   float zIndex = float(uMaxZIndex - aZIndex) * -1.0f;\n"
        "\n"
        "   gl_Position = vec4(pos, zIndex, 1.0f);\n"
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