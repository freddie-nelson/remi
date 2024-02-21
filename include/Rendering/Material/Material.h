#pragma once

#include "./Color.h"
#include "../Texture/Texture.h"
#include "../Texture/AnimatedTexture.h"

namespace Rendering
{
    using MaterialId = size_t;

    /**
     * Represents a material.
     *
     * A material is a combination of a color and a texture.
     *
     * The color is multiplied by the texture color to get the final color of the material.
     *
     * If the texture is not set it will be set to a 64x64 white texture.
     *
     * The material can also be animated by setting the texture to an animated texture. The material cannot have a texture and an animated texture at the same time.
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
         * The material will be set to the same color and texture as the given material.
         *
         * However the material will have a different id.
         *
         * @param m The material to copy.
         */
        Material(const Material &m);

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
         * @param texture The animated texture of the material.
         */
        Material(AnimatedTexture *texture);

        /**
         * Constructs a material.
         *
         * @param color The color of the material.
         * @param texture The texture of the material.
         */
        Material(Color color, Texture *texture);

        /**
         * Constructs a material.
         *
         * @param color The color of the material.
         * @param texture The animated texture of the material.
         */
        Material(Color color, AnimatedTexture *texture);

        /**
         * Destroys the material.
         */
        virtual ~Material();

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
         * If the material is animated this will return the current frame of the animation.
         *
         * @returns The texture of the material.
         */
        const Texture *getTexture() const;

        /**
         * Gets the animated texture of the material.
         *
         * If the material is not animated this will return nullptr.
         *
         * @returns The animated texture of the material.
         */
        AnimatedTexture *getAnimatedTexture() const;

        /**
         * Gets whether the material is animated or not.
         *
         * This is true if the material has an animated texture.
         *
         * @returns Whether the material is animated or not.
         */
        bool isAnimated() const;

        /**
         * Sets the texture of the material.
         *
         * If the texture is nullptr it will be set to a 1x1 white texture.
         *
         * @param texture The texture of the material.
         */
        void setTexture(Texture *texture);

        /**
         * Sets the animated texture of the material.
         *
         * If the texture is nullptr it will be set to a 1x1 white non animated texture.
         *
         * @param texture The animated texture of the material.
         */
        void setTexture(AnimatedTexture *texture);

        /**
         * Gets whether the material is transparent or not.
         *
         * The material is transparent if the alpha value of the color is less than 1.0 or the texture has transparency.
         *
         * @returns Whether the material is transparent or not.
         */
        virtual bool isTransparent() const;

        /**
         * Copys the texture and color from the given material into this material.
         *
         * @param m The material to copy.
         *
         * @returns This material.
         */
        Material &operator=(const Material &m);

    protected:
        Color color;
        Texture *texture = nullptr;
        AnimatedTexture *animatedTexture = nullptr;

    private:
        static MaterialId nextId;

        const MaterialId id = nextId++;

        /**
         * The default texture.
         *
         * This is an 32x32 white texture.
         */
        static Texture *defaultTexture;
    };
}