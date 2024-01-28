#pragma once

#include "PostProcessingPass.h"

namespace Rendering
{
    /**
     * Represents an output pass.
     *
     * This pass outputs the render target texture to the screen.
     *
     * This pass does not require any data in the input.
     */
    class OutputPass : public PostProcessingPass
    {
    public:
        OutputPass();

        ~OutputPass() = default;

        /**
         * Gets the name of the render pass.
         *
         * @returns The name of the render pass.
         */
        virtual constexpr std::string getName() override
        {
            return "OutputPass";
        };

    private:
        const std::string fragShader =
            "#version 300 es\n"
            "\n"
            "precision mediump float;\n"
            "\n"
            "uniform sampler2D uRenderTexture;\n"
            "\n"
            "in vec2 vTexCoord;\n"
            "\n"
            "out vec4 FragColor;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    FragColor = texture(uRenderTexture, vTexCoord);\n"
            "}\n";
    };
}