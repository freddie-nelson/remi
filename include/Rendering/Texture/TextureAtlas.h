#pragma once

#include "Texture.h"

#include <glm/glm.hpp>
#include <unordered_map>

namespace Rendering
{
    /**
     * A texture atlas is a texture that contains multiple textures.
     *
     * The texture atlas will be of a fixed size, and so the textures will be packed into the atlas.
     *
     * The size of the texture atlas will be min(MAX_ATLAS_SIZE, GL_MAX_TEXTURE_SIZE * ATLAS_SIZE_MULTIPLIER) (width and height).
     *
     * Large atlas sizes may crash the engine.
     *
     * ! TODO: investigate crashing when atlas size is large. Could also be happening when having multiple atlases?
     *
     * The atlas does not own the Texture objects it contains, and so will not destroy them when it is destroyed.
     *
     * The atlas will not modify the textures it contains.
     *
     * The texture packing algorithm is attempts to pack the textures into the atlas as tightly as possible.
     *
     * This is done by continuously subdividing the atlas into spaces as it is filled with textures.
     */
    class TextureAtlas
    {

    public:
        /**
         * The multiplier for the size of the atlas.
         *
         * By default this is 0.75f, so the atlas will be 75% of the maximum texture size.
         *
         * This value must be between 0 (exclusive) and 1 (inclusive).
         *
         * This will only affect the size of the atlas when it is created.
         */
        static float ATLAS_SIZE_MULTIPLIER;

        /**
         * The maximum size of the atlas.
         *
         * Can be made smaller to improve precision of uv coordinates for smaller textures.
         *
         * By default this is 8192.
         *
         * This will only affect the size of the atlas when it is created.
         */
        static unsigned int MAX_ATLAS_SIZE;

        /**
         * Gets the size of an atlas.
         */
        static unsigned int getAtlasSize();

        /**
         * Creates a new texture atlas.
         *
         * @param padding The padding between textures in the atlas, used to prevent artefacts.
         */
        TextureAtlas(unsigned int padding = 2);

        /**
         * Destroys the texture atlas.
         *
         * Frees the memory allocated for the pixels.
         */
        ~TextureAtlas();

        /**
         * Adds a texture to the atlas.
         *
         * If the texture is already in the atlas it will be overwritten, however no error will be thrown.
         *
         * @param texture The texture to add to the atlas.
         * @param repack Whether to repack the atlas after adding the texture.
         *
         * @returns The position of the top left corner of the texture in the atlas.
         *
         * @throws std::invalid_argument If the texture is null.
         * @throws std::invalid_argument If the texture is too big for the atlas.
         * @throws std::runtime_error If the texture cannot be packed into the atlas.
         */
        glm::vec2 add(const Texture *texture, bool repack = true);

        /**
         * Removes a texture from the atlas.
         *
         * Does nothing if the texture is not in the atlas.
         *
         * @param texId The texture to remove from the atlas.
         * @param repack Whether to repack the atlas after removing the texture.
         */
        void remove(TextureId texId, bool repack = true);

        /**
         * Gets the position of the top left corner of the texture in the atlas.
         *
         * @param texId The texture to get the position of.
         *
         * @returns The position of the top left corner of the texture in the atlas.
         *
         * @throws std::invalid_argument If the texture is not in the atlas.
         */
        glm::vec2 get(TextureId texId);

        /**
         * Checks if the atlas contains the given texture.
         *
         * @param texId The texture to check for.
         */
        bool has(TextureId texId);

        /**
         * Gets the padding between textures in the atlas.
         *
         * @returns The padding between textures in the atlas.
         */
        unsigned int getPadding() const;

        /**
         * Sets the padding between textures in the atlas.
         *
         * @param padding The padding between textures in the atlas.
         */
        void setPadding(unsigned int padding);

        /**
         * Gets the width of the atlas.
         *
         * @returns The width of the atlas.
         */
        unsigned int getWidth() const;

        /**
         * Gets the height of the atlas.
         *
         * @returns The height of the atlas.
         */
        unsigned int getHeight() const;

        /**
         * Gets the pixels of the atlas.
         *
         * @returns The pixels of the atlas.
         */
        const unsigned char *getPixels() const;

        /**
         * Packs the textures into the atlas.
         *
         * @throws std::runtime_error If the textures cannot be packed into the atlas.
         */
        void pack();

    private:
        /**
         * The width of the atlas.
         */
        unsigned int width;

        /**
         * The height of the atlas.
         */
        unsigned int height;

        /**
         * The atlas pixels.
         */
        unsigned char *pixels = nullptr;

        /**
         * The padding between textures in the atlas
         */
        unsigned int padding;

        /**
         * The coordinates of the textures in the atlas.
         */
        std::unordered_map<TextureId, glm::vec2> positions;

        /**
         * The textures in the atlas.
         */
        std::unordered_map<TextureId, const Texture *> textures;

        /**
         * A space in the atlas.
         */
        struct AtlasSpace
        {
            glm::vec2 position;
            unsigned int width;
            unsigned int height;
        };

        /**
         * Converts a position in the atlas to UV coordinates in the atlas.
         *
         * @param position The position in the atlas.
         *
         * @returns The UV coordinates in the atlas.
         *
         * @throws std::invalid_argument If the position is not in the atlas.
         */
        glm::vec2 posToUV(glm::vec2 position);

        /**
         * Writes the atlas to a png file.
         *
         * @param sample Must be greater than 1. Bigger values downsample the image.
         */
        void writeTestAtlas(int sample = 1);
    };
}