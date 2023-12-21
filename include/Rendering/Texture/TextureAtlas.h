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
     * The atlas does not own the Texture objects it contains, and so will not destroy them when it is destroyed.
     *
     * The atlas will not modify the textures it contains.
     *
     * The texture packing algorithm is extremely simple, and so may not be the most efficient.
     *
     * TODO: Improve the texture packing algorithm.
     */
    class TextureAtlas
    {

    public:
        /**
         * The multiplier for the size of the atlas.
         *
         * By default this is 0.5f, so the atlas will be 50% of the maximum texture size.
         *
         * This value must be between 0 (exclusive) and 1 (inclusive).
         */
        static constexpr float ATLAS_SIZE_MULTIPLIER = 0.5f;

        /**
         * The maximum size of the atlas.
         *
         * Can be made smaller to improve precision of uv coordinates for smaller textures.
         */
        static constexpr unsigned int MAX_ATLAS_SIZE = 8192;

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
         *
         * @returns The position of the top left corner of the texture in the atlas.
         *
         * @throws std::invalid_argument If the texture is null.
         * @throws std::invalid_argument If the texture is too big for the atlas.
         * @throws std::runtime_error If the texture cannot be packed into the atlas.
         */
        glm::vec2 add(const Texture *texture);

        /**
         * Removes a texture from the atlas.
         *
         * Does nothing if the texture is not in the atlas.
         *
         * @param texId The texture to remove from the atlas.
         */
        void remove(TextureId texId);

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
         * Packs the textures into the atlas.
         *
         * @throws std::runtime_error If the textures cannot be packed into the atlas.
         */
        void pack();

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
    };
}