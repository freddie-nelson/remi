#pragma once

#include "./Color.h"
#include "../Texture/Texture.h"

namespace Rendering
{
    /**
     * Represents a material.
     *
     * A material is a combination of a color and a texture.
     *
     * The color is multiplied by the texture color to get the final color of the material.
     *
     * If the texture is not set it will be set to a 1x1 white texture.
     *
     * If the color is not set it will be set to (1.0, 1.0, 1.0, 1.0) or fully opaque white.
     *
     * The material does not own the texture, and so will not destroy the texture when it is destroyed or changed. This is so the texture can be shared between multiple materials.
     */
    class Material
    {
    public:
        /**
         * Constructs a material.
         *
         * The material will be set to (1.0, 1.0, 1.0, 1.0) or fully opaque white.
         */
        Material();

        /**
         * Constructs a material.
         *
         * @param color The color of the material.
         */
        Material(Color color);

        /**
         * Constructs a material.
         *
         * @param texture The texture of the material.
         */
        Material(Texture *texture);

        /**
         * Constructs a material.
         *
         * @param color The color of the material.
         * @param texture The texture of the material.
         */
        Material(Color color, Texture *texture);

        /**
         * Destroys the material.
         */
        ~Material();

        /**
         * Gets the color of the material.
         *
         * @returns The color of the material.
         */
        const Color &getColor() const;

        /**
         * Sets the color of the material.
         *
         * @param color The color of the material.
         */
        void setColor(Color color);

        /**
         * Gets the texture of the material.
         *
         * @returns The texture of the material.
         */
        const Texture *getTexture() const;

        /**
         * Sets the texture of the material.
         *
         * If the texture is nullptr it will be set to a 1x1 white texture.
         *
         * @param texture The texture of the material.
         */
        void setTexture(Texture *texture);

    private:
        /**
         * The default texture.
         *
         * This is a 1x1 white texture.
         */
        static Texture *defaultTexture;

        Color color;
        Texture *texture;
    };
}