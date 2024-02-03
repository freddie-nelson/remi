
#pragma once

#include "PostProcessingPass.h"
#include "../Material/Color.h"

namespace Rendering
{
    /**
     * Represents a brightness pass.
     *
     * This pass increases the brightness of each pixel in the render target texture.
     */
    class BrightnessPass : public PostProcessingPass
    {
    public:
        /**
         * Creates a BrightnessPass instance.
         *
         * @param brightness The amount to increase the brightness by. A number between 0 and 1 (inclusive).
         */
        BrightnessPass(float brightness);

        /**
         * Destroys the BrightnessPass instance.
         */
        ~BrightnessPass();

        /**
         * Gets the amount to increase the brightness by.
         *
         * @returns The amount to increase the brightness by.
         */
        float getBrightness();

        /**
         * Sets the amount to increase the brightness by.
         *
         * @param brightness The amount to increase the brightness by. A number between 0 and 1 (inclusive).
         */
        void setBrightness(float brightness);

        /**
         * Gets the name of the render pass.
         *
         * @returns The name of the render pass.
         */
        virtual constexpr std::string getName() override
        {
            return "BrightnessPass";
        };

    private:
        float brightness;
        Uniform<float> *brightnessUniform = nullptr;

        const std::string fragShader =
            "#version 300 es\n"
            "\n"
            "precision mediump float;\n"
            "\n"
            "uniform sampler2D uRenderTexture;\n"
            "uniform float uBrightness;\n"
            "\n"
            "in vec2 vTexCoord;\n"
            "\n"
            "out vec4 FragColor;\n"
            "\n"
            "vec3 rgb2hsv(vec3 c)\n"
            "{\n"
            "   vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);\n"
            "   vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));\n"
            "   vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));"
            "\n"
            "   float d = q.x - min(q.w, q.y);\n"
            "   float e = 1.0e-10;\n"
            "   return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);\n"
            "}\n"
            "\n"
            "vec3 hsv2rgb(vec3 c)\n"
            "{\n"
            "   vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);\n"
            "   vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);\n"
            "   return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);\n"
            "}\n"
            "\n"
            "void main()\n"
            "{\n"
            "    vec4 color = texture(uRenderTexture, vTexCoord);\n"
            "\n"
            "    vec3 hsv = rgb2hsv(color.rgb);\n"
            "    hsv.z = min(hsv.z + uBrightness, 1.0);\n"
            "\n"
            "    vec3 rgb = hsv2rgb(hsv);\n"
            "\n"
            "    FragColor = vec4(rgb, color.a);"
            "}\n";
    };
}