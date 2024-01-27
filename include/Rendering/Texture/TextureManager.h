#pragma once

#include "Texture.h"
#include "TextureAtlas.h"
#include "../Utility/OpenGLHelpers.h"
#include "../../gl.h"

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

        /**
         * Binds the given render target so that it can be used for rendering.
         *
         * This will bind the render target texture to the last texture unit.
         *
         * @param texture The texture object to bind to the render target texture unit.
         */
        void bindRenderTarget(GLuint texture);

        /**
         * Gets the number of texture units currently used.
         *
         * @returns The number of texture units currently used.
         */
        int getTextureUnitsUsed() const;

        /**
         * Gets the number of texture units that are reserved for the engine.
         *
         * These will be placed in the last texture units, after all atlas slots.
         *
         * They are in the last slots of the texturesUniform vector.
         *
         * @returns The number of texture units that are reserved for the engine.
         */
        int getReservedTextureUnits() const;

        /**
         * Gets the texture unit that the render target is bound to.
         *
         * This is reserved for the engine and should not be modified.
         *
         * @returns The texture unit that the render target is bound to.
         */
        int getRenderTargetTextureUnit() const;

        /**
         * Gets the array of active texture units currently bound.
         *
         * The values which are 0 and after textureUnitsUsed, but before reservedTextureUnits indices from the end are not bound .
         *
         * @returns The array of active texture units currently bound.
         */
        const std::vector<int> &getTexturesUniform() const;

    private:
        int textureUnitsUsed = 0;

        /**
         * The number of texture units that are reserved for the engine.
         *
         * These will be placed in the last texture units, after all atlas slots.
         */
        const int reservedTextureUnits = 3;

        /**
         * The texture unit that the render target is bound to.
         *
         * This is the last texture unit.
         *
         * This is reserved for the engine and should not be modified.
         */
        const int renderTargetTextureUnit = glGetMaxTextureUnits() - 1;

        /**
         * The texture units that are currently bound.
         *
         * The size of the vector is always equal to the number of texture units supported by the GPU.
         *
         * The values which are 0 and after textureUnitsUsed, but before reservedTextureUnits indices from the end are not bound .
         */
        std::vector<int> texturesUniform;

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
    };
}