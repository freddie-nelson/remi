#pragma once

#include <glm/vec4.hpp>
#include <glm/vec3.hpp>

namespace Rendering
{

    /**
     * A color.
     *
     * Acts as a wrapper around glm::vec4 with additional helpful color related methods.
     *
     * Also insures that the color is valid, i.e. all components are between 0 and 1 inclusive.
     */
    class Color
    {
    public:
        /**
         * Checks if the given color is valid.
         *
         * A color is valid if all of it's components are between 0 and 1 inclusive.
         *
         * @param rgba The color to check.
         *
         * @returns True if the color is valid.
         * @throws std::invalid_argument If the color is not valid.
         */
        static bool isColorValid(glm::vec4 rgba);

        /**
         * Checks if the given color is valid.
         *
         * A color is valid if all of it's components are between 0 and 1 inclusive.
         *
         * @param rgba The color to check.
         *
         * @returns True if the color is valid, false otherwise.
         */
        static bool isColorValidSafe(glm::vec4 rgba);

        /**
         * Constructs a color.
         *
         * @param r The red component of the color.
         * @param g The green component of the color.
         * @param b The blue component of the color.
         * @param a The alpha component of the color.
         *
         * @throws std::invalid_argument If the color is not valid.
         */
        Color(float r, float g, float b, float a);

        /**
         * Constructs a color.
         *
         * @param rgba The color.
         *
         * @throws std::invalid_argument If the color is not valid.
         */
        Color(glm::vec4 rgba);

        /**
         * Constructs a color.
         *
         * The alpha component will be set to 1.
         *
         * @param rgb The color.
         *
         * @throws std::invalid_argument If the color is not valid.
         */
        Color(glm::vec3 rgb);

        /**
         * Constructs a color.
         *
         * @param rgb The value to set each component to.
         * @param a The alpha component of the color.
         *
         * @throws std::invalid_argument If the color is not valid.
         */
        Color(float rgb, float a = 1.0f);

        /**
         * Lightens the color by the given amount.
         *
         * This multiplies each rgb component by `amount + 1`.
         *
         * @param amount The amount to lighten the color by.
         */
        void lighten(float amount);

        /**
         * Darkens the color by the given amount.
         *
         * This multiplies each rgb component by `1 - amount`.
         *
         * @param amount The amount to darken the color by.
         */
        void darken(float amount);

        /**
         * Blends between this color and the given color to create a new color based on the alpha values of each.
         *
         * In the blending process this color is the background and the given color is the foreground.
         *
         * Does not modify this color or `fg`.
         *
         * @param fg The color to blend with.
         *
         * @returns The new blended color.
         */
        Color blend(const Color &fg);

        /**
         * Clamps the color.
         *
         * @param min The minimum value to clamp to.
         * @param max The maximum value to clamp to.
         */
        void clamp(float min, float max);

        /**
         * Clamps the rgb components of the color.
         *
         * @param min The minimum value to clamp to.
         * @param max The maximum value to clamp to.
         */
        void clampRGB(float min, float max);

        /**
         * Clamps the red component of the color.
         *
         * @param min The minimum value to clamp to.
         * @param max The maximum value to clamp to.
         */
        void clampR(float min, float max);

        /**
         * Clamps the green component of the color.
         *
         * @param min The minimum value to clamp to.
         * @param max The maximum value to clamp to.
         */
        void clampG(float min, float max);

        /**
         * Clamps the blue component of the color.
         *
         * @param min The minimum value to clamp to.
         * @param max The maximum value to clamp to.
         */
        void clampB(float min, float max);

        /**
         * Clamps the alpha component of the color.
         *
         * @param min The minimum value to clamp to.
         * @param max The maximum value to clamp to.
         */
        void clampA(float min, float max);

        /**
         * Sets the color.
         *
         * @param r The red component of the color.
         * @param g The green component of the color.
         * @param b The blue component of the color.
         * @param a The alpha component of the color.
         *
         * @throws std::invalid_argument If the color is not valid.
         */
        void setColor(float r, float g, float b, float a);

        /**
         * Sets the color.
         *
         * @param rgba The color.
         *
         * @throws std::invalid_argument If the color is not valid.
         */
        void setColor(glm::vec4 rgba);

        /**
         * Sets the color.
         *
         * The alpha component will be set to 1.
         *
         * @param rgb The color.
         *
         * @throws std::invalid_argument If the color is not valid.
         */
        void setColor(glm::vec3 rgb);

        /**
         * Sets the color.
         *
         * @param rgb The value to set each component to.
         * @param a The alpha component of the color.
         *
         * @throws std::invalid_argument If the color is not valid.
         */
        void setColor(float rgb, float a = 1.0f);

        /**
         * Returns the color as a glm::vec4.
         *
         * @returns The color as a glm::vec4.
         */
        glm::vec4 getColor() const;

        /**
         * Sets the color from a HSLA color value.
         *
         * Component values are between 0 and 1 inclusive.
         *
         * @param h The hue of the color.
         * @param s The saturation of the color.
         * @param l The lightness of the color.
         * @param a The alpha value of the color.
         */
        void fromHSLA(float h, float s, float l, float a = 1.0f);

        /**
         * Returns the red component of the color.
         *
         * @returns The red component of the color.
         */
        inline float r() const
        {
            return rgba.r;
        }

        /**
         * Returns the green component of the color.
         *
         * @returns The green component of the color.
         */
        inline float g() const
        {
            return rgba.g;
        }

        /**
         * Returns the blue component of the color.
         *
         * @returns The blue component of the color.
         */
        inline float b() const
        {
            return rgba.b;
        }

        /**
         * Returns the alpha component of the color.
         *
         * @returns The alpha component of the color.
         */
        inline float a() const
        {
            return rgba.a;
        }

        /**
         * Sets the red component of the color.
         *
         * @param r The value to set the red component to.
         *
         * @throws std::invalid_argument if r is an invalid value
         */
        void setR(float r);

        /**
         * Sets the green component of the color.
         *
         * @param g The value to set the green component to.
         *
         * @throws std::invalid_argument if g is an invalid value
         */
        void setG(float g);

        /**
         * Sets the blue component of the color.
         *
         * @param b The value to set the blue component to.
         *
         * @throws std::invalid_argument if b is an invalid value
         */
        void setB(float b);

        /**
         * Sets the alpha component of the color.
         *
         * @param a The value to set the alpha component to.
         *
         * @throws std::invalid_argument if a is an invalid value
         */
        void setA(float a);

    private:
        glm::vec4 rgba;

        /**
         * Validates the min and max paramters for Color::clamp methods.
         *
         * @throws std::invalid_argument when min and are invalid
         */
        void validateClampMinMax(float min, float max);

        /**
         * Convert a hue to r, b or g.
         *
         * @returns float between 0 and 1 inclusive.
         */
        float hueToRgb(float p, float q, float t);
    };
}