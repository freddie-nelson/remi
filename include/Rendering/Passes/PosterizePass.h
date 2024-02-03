
#pragma once

#include "PostProcessingPass.h"
#include "../Material/Color.h"

namespace Rendering
{
    /**
     * Represents a posterize pass.
     *
     * This pass posterizes the render target texture.
     */
    class PosterizePass : public PostProcessingPass
    {
    public:
        /**
         * Creates a PosterizePass instance.
         *
         * @param steps The number of steps in the posterization. A higher number will result in a smoother posterization.
         */
        PosterizePass(float steps);

        /**
         * Destroys the PosterizePass instance.
         */
        ~PosterizePass();

        /**
         * Gets the number of steps in the posterization.
         *
         * @returns The number of steps in the posterization.
         */
        float getSteps();

        /**
         * Sets the number of steps in the posterization.
         *
         * @param steps The number of steps in the posterization. A higher number will result in a smoother posterization.
         */
        void setSteps(float steps);

        /**
         * Gets the name of the render pass.
         *
         * @returns The name of the render pass.
         */
        virtual constexpr std::string getName() override
        {
            return "PosterizePass";
        };

    private:
        float steps;
        Uniform<float> *stepsUniform = nullptr;

        const std::string fragShader =
            "#version 300 es\n"
            "\n"
            "precision mediump float;\n"
            "\n"
            "uniform sampler2D uRenderTexture;\n"
            "uniform float uSteps;\n"
            "\n"
            "in vec2 vTexCoord;\n"
            "\n"
            "out vec4 FragColor;\n"
            "\n"
            "vec4 posterize(vec4 src, float steps)\n"
            "{\n"
            "   return vec4(floor(src.rgb * steps) / steps, src.a);\n"
            "}\n"
            "\n"
            "void main()\n"
            "{\n"
            "    FragColor = posterize(texture(uRenderTexture, vTexCoord), uSteps);\n"
            "}\n";
    };
}