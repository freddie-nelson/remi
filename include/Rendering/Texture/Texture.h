#pragma once

#include "../Material/Color.h"

#include <string>

namespace Rendering
{
    using TextureId = size_t;

    /**
     * Represents a texture.
     *
     * A texture is a 2D image in raw RGBA bytes.
     *
     * If a non RGBA image or pixels buffer is loaded it will be converted to RGBA.
     *
     * The texture will destroy the pixels when it is destroyed.
     */
    class Texture
    {
    public:
        /**
         * Constructs a texture.
         *
         * The texture will have the same pixels, width, height and channels as the given texture.
         *
         * However the texture will have a different id.
         *
         * @param t The texture to copy.
         */
        Texture(const Texture &t);

        /**
         * Constructs a texture.
         *
         * @param path The path to the image to load.
         * @param flip Whether to flip the image vertically or not, by default this is true.
         */
        Texture(std::string path, bool flip = true);

        /**
         * Constructs a texture.
         *
         * Copies the pixels into the texture.
         *
         * @param pixels The pixels to create the texture from. Should be bytes of RGB or RGBA values.
         * @param width The width of the texture.
         * @param height The height of the texture.
         * @param channels The number of channels in the pixels, should be either 3 (RGB) or 4 (RGBA).
         */
        Texture(unsigned char *pixels, unsigned int width, unsigned int height, unsigned int channels);

        /**
         * Constructs a texture.
         *
         * The texture will be a widthxheight image of the given color.
         *
         * @param color The color to create the texture from.
         * @param width The width of the texture.
         * @param height The height of the texture.
         */
        Texture(Color color, unsigned int width, unsigned int height);

        /**
         * Destroys the texture.
         *
         * Frees the memory allocated for the pixels.
         */
        virtual ~Texture();

        /**
         * Loads an image from the given path into the texture.
         *
         * @param path The path to the image to load.
         * @param flip Whether to flip the image vertically or not, by default this is true.
         */
        void fromFile(std::string path, bool flip = true);

        /**
         * Creates the texture from the given pixels.
         *
         * Copies the pixels into the texture.
         *
         * @param pixels The pixels to create the texture from. Should be bytes of RGB or RGBA values.
         * @param width The width of the texture.
         * @param height The height of the texture.
         * @param channels The number of channels in the pixels, should be either 3 (RGB) or 4 (RGBA).
         */
        void fromPixels(unsigned char *pixels, unsigned int width, unsigned int height, unsigned int channels);

        /**
         * Creates the texture from the given color.
         *
         * The texture will be a widthxheight image of the given color.
         *
         * @param color The color to create the texture from.
         * @param width The width of the texture.
         * @param height The height of the texture.
         */
        void fromColor(Color color, unsigned int width, unsigned int height);

        /**
         * Gets the id of the texture.
         *
         * @returns The id of the texture.
         */
        size_t getId() const;

        /**
         * Gets the width of the texture.
         *
         * @returns The width of the texture.
         */
        unsigned int getWidth() const;

        /**
         * Gets the height of the texture.
         *
         * @returns The height of the texture.
         */
        unsigned int getHeight() const;

        /**
         * Gets the number of channels in the texture.
         *
         * Will always be 4 as the texture is always in RGBA format.
         *
         * @returns The number of channels in the texture.
         */
        unsigned int getChannels() const;

        /**
         * Gets the pixels of the texture.
         *
         * @returns The pixels of the texture.
         */
        unsigned char *getPixels() const;

        /**
         * Gets whether the texture has transparency or not.
         *
         * This is true if any of the pixels have an alpha value of less than 255.
         *
         * This is only calculated when the texture is created/changed.
         *
         * @returns Whether the texture has transparency or not.
         */
        bool isTransparent() const;

        /**
         * Copys the pixels, width, height and channels from the given texture into this texture.
         *
         * @param t The texture to copy.
         *
         * @returns This texture.
         */
        Texture &operator=(const Texture &t);

    protected:
        unsigned int width;
        unsigned int height;
        unsigned int channels;

        unsigned char *pixels = nullptr;

        bool hasTransparency = false;

        /**
         * Converts the given pixels to RGBA.
         *
         * The pixels must be in RGB format.
         *
         * @param pixels The pixels to convert.
         * @param width The width of the pixels.
         * @param height The height of the pixels.
         *
         * @returns The pixels in RGBA format.
         */
        unsigned char *toRGBA(unsigned char *pixels, unsigned int width, unsigned int height);

    private:
        static TextureId nextId;

        const TextureId id = nextId++;
    };
}