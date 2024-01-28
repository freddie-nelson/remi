#pragma once

#include "PostProcessingPass.h"
#include "../Material/Color.h"

namespace Rendering
{
    /**
     * Represents a color blend pass.
     *
     * This pass multiplies each pixel in the render target texture by a color.
     */
    class ColorBlendPass : public PostProcessingPass
    {
    public:
        /**
         * Creates a ColorBlendPass instance.
         *
         * @param color The color to blend.
         */
        ColorBlendPass(Color color);

        /**
         * Destroys the ColorBlendPass instance.
         */
        ~ColorBlendPass();

        /**
         * Gets the color of the blend.
         *
         * @returns The color of the blend.
         */
        const Color &getColor();

        /**
         * Sets the color of the blend.
         *
         * @param color The color of the blend.
         */
        void setColor(Color color);

        /**
         * Gets the name of the render pass.
         *
         * @returns The name of the render pass.
         */
        virtual constexpr std::string getName() override
        {
            return "ColorBlendPass";
        };

    private:
        Color color;
        glm::vec4 colorVec;
        Uniform<glm::vec4> *colorUniform = nullptr;

        const std::string fragShader =
            "#version 300 es\n"
            "\n"
            "precision mediump float;\n"
            "\n"
            "uniform sampler2D uRenderTexture;\n"
            "uniform vec4 uColor;\n"
            "\n"
            "in vec2 vTexCoord;\n"
            "\n"
            "out vec4 FragColor;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    FragColor = texture(uRenderTexture, vTexCoord) * uColor;\n"
            "}\n";
    };
}