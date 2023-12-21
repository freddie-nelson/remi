#pragma once

#include "Texture.h"
#include "TextureAtlas.h"

#include "../../externals/glad/gl.h"
#include <unordered_map>
#include <vector>

namespace Rendering
{
    /**
     * Manages textures.
     *
     * This class handles the loading/unloading and binding/unbinding of textures from the GPU.
     *
     * All textures will be placed in texture atlases when bound.
     *
     * When destroyed it will unbind and destroy all the textures on the GPU.
     */
    class TextureManager
    {
    public:
        /**
         * Creates a new texture manager.
         */
        TextureManager();

        /**
         * Destroys the texture manager.
         *
         * Unbinds and destroys all the textures on the GPU.
         */
        ~TextureManager();

        /**
         * Represents a texture that has been bound to the GPU.
         *
         * This contains the texture and its position in the atlas, as well as the texture unit the atlas is bound to on the gpu.
         */
        struct BoundTexture
        {
            const Texture *texture;
            glm::vec2 textureSize;
            glm::vec2 posInAtlas;
            glm::vec2 atlasSize;
            int textureUnit;
        };

        /**
         * Binds the given texture so that it can be used for rendering.
         *
         * @param texture The texture to bind.
         *
         * @returns The texture unit and uvs of the texture in the bound atlas.
         */
        BoundTexture bind(const Texture *texture);

    private:
        /**
         * The texture atlases that have been created.
         *
         * The index corresponds to the texture unit that the atlas is bound to.
         *
         * The texture atlas will be bound to the texture unit when it is created.
         */
        std::vector<TextureAtlas> atlases;

        /**
         * Map of atlas index to opengl texture id.
         */
        std::unordered_map<unsigned int, GLuint> atlasToTextureId;

        /**
         * The containing atlas for each texture.
         */
        std::unordered_map<TextureId, unsigned int> textureToAtlas;

        /**
         * Trys to get the containing atlas for the given texture.
         *
         * If the texture is not contained in an atlas then nullptr will be returned.
         *
         * @param texture The texture to check.
         *
         * @returns The index of the texture unit that the atlas is bound to, this is also the index of the atlas in the atlases vector, or -1 if the texture is not in an atlas.
         *
         * @throws std::invalid_argument If the texture is null.
         */
        int getContainingAtlas(const Texture *texture);

        /**
         * Adds the given texture to an available atlas.
         *
         * If there are no available atlases then a new one will be created.
         *
         * @param texture The texture to add to the atlas.
         *
         * @returns The index of the texture unit that the atlas is bound to, this is also the index of the atlas in the atlases vector.
         *
         * @throws std::invalid_argument If the texture is null.
         */
        unsigned int addTextureToAtlas(const Texture *texture);

        /**
         * Creates a new texture atlas.
         *
         * @returns The index of the texture unit that the atlas is bound to, this is also the index of the atlas in the atlases vector.
         *
         * @throws std::runtime_error If there are no more texture units available.
         */
        unsigned int createAtlas();

        /**
         * Loads the atlas onto the GPU.
         *
         * @param textureUnit The texture unit / index of the atlas in the atlases vector.
         *
         * @throws std::invalid_argument If the texture unit is invalid.
         */
        void loadAtlas(unsigned int textureUnit);

        /**
         * Gets the maximum number of texture units supported by the GPU.
         *
         * @returns The maximum number of texture units supported by the GPU.
         */
        unsigned int getMaxTextureUnits();
    };
}