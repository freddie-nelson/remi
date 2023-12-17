#pragma once

#include "../Material/Color.h"

#include <string>

namespace Rendering
{
    class Texture
    {
    public:
        /**
         * Constructs a texture.
         *
         * @param path The path to the image to load.
         */
        Texture(std::string path);

        /**
         * Constructs a texture.
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
        ~Texture();

        /**
         * Loads an image from the given path into the texture.
         *
         * @param path The path to the image to load.
         */
        void fromFile(std::string path);

        /**
         * Creates the texture from the given pixels.
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
         * @returns The number of channels in the texture.
         */
        unsigned int getChannels() const;

        /**
         * Gets the pixels of the texture.
         *
         * @returns The pixels of the texture.
         */
        unsigned char *getPixels() const;

    private:
        unsigned int width;
        unsigned int height;
        unsigned int channels;

        unsigned char *pixels;
    };
}