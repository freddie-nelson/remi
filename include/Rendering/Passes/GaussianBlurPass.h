#pragma once

#include "PostProcessingPass.h"

namespace Rendering
{
    /**
     * Represents a gaussian blur pass.
     *
     * This pass blurs the render target texture.
     */
    class GaussianBlurPass : public PostProcessingPass
    {
    public:
        /**
         * Creates a GaussianBlurPass instance.
         *
         * @param radius The radius of the blur.
         * @param quality The quality of the blur.
         * @param directions The number of directions to blur in.
         */
        GaussianBlurPass(float radius = 8, float quality = 3, float directions = 16);

        /**
         * Destroys the GaussianBlurPass instance.
         */
        ~GaussianBlurPass();

        /**
         * Gets the radius of the blur.
         *
         * @returns The radius of the blur.
         */
        float getRadius();

        /**
         * Sets the radius of the blur.
         *
         * @param radius The radius of the blur.
         */
        void setRadius(float radius);

        /**
         * Gets the quality of the blur.
         *
         * @returns The quality of the blur.
         */
        float getQuality();

        /**
         * Sets the quality of the blur.
         *
         * @param quality The quality of the blur.
         */
        void setQuality(float quality);

        /**
         * Gets the number of directions to blur in.
         *
         * @returns The number of directions to blur in.
         */
        float getDirections();

        /**
         * Sets the number of directions to blur in.
         *
         * @param directions The number of directions to blur in.
         */
        void setDirections(float directions);

        /**
         * Gets the name of the render pass.
         *
         * @returns The name of the render pass.
         */
        virtual constexpr std::string getName() override
        {
            return "GaussianBlurPass";
        };

    private:
        float radius;
        float quality;
        float directions;

        Uniform<float> *radiusUniform = nullptr;
        Uniform<float> *qualityUniform = nullptr;
        Uniform<float> *directionsUniform = nullptr;

        const std::string fragShader =
            "#version 300 es\n"
            "\n"
            "precision mediump float;\n"
            "\n"
            "uniform sampler2D uRenderTexture;\n"
            "uniform vec2 uResolution;\n"
            "\n"
            "uniform float uDirections;\n"
            "uniform float uQuality;\n"
            "uniform float uRadius;\n"
            "\n"
            "in vec2 vTexCoord;\n"
            "\n"
            "out vec4 FragColor;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    const float pi2 = 6.28318530718;\n"
            "\n"
            "    vec2 radius = uRadius / uResolution;\n"
            "\n"
            "    vec4 color = texture(uRenderTexture, vTexCoord);\n"
            "\n"
            "    for (float d = 0.0f; d < pi2; d += pi2 / uDirections)\n"
            "    {\n"
            "        vec2 direction = vec2(cos(d), sin(d));\n"
            "\n"
            "        for (float i = 1.0f / uQuality; i <= 1.0f; i += 1.0f / uQuality)\n"
            "        {\n"
            "            color += texture(uRenderTexture, vTexCoord + direction * radius * i);\n"
            "        }\n"
            "    }\n"
            "\n"
            "    color /= uDirections * uQuality;\n"
            "\n"
            "    FragColor = color;\n"
            "}\n";
    };
}