#pragma once

#include "./Texture/Texture.h"
#include "../gl.h"
#include "./Texture/TextureManager.h"

#include <glm/glm.hpp>

namespace Rendering
{
    /**
     * Represents a render target.
     *
     * This can be used to render to a texture.
     */
    class RenderTarget
    {
    public:
        /**
         * Creates a RenderTarget instance.
         *
         * @param width The width of the render target.
         * @param height The height of the render target.
         */
        RenderTarget(unsigned int width, unsigned int height);

        /**
         * Destroys the RenderTarget instance.
         */
        ~RenderTarget();

        /**
         * Binds the render target.
         *
         * This will make all rendering go to the texture.
         *
         * @param textureManager The texture manager to bind the texture to.
         * @param bindFramebuffer Whether to bind the framebuffer.
         */
        void bind(TextureManager &textureManager, bool bindFramebuffer = true) const;

        /**
         * Unbinds the render target.
         *
         * This will make all rendering go to the screen.
         *
         * @param textureManager The texture manager to unbind the texture from.
         */
        void unbind(TextureManager &textureManager) const;

        /**
         * Clears the render target.
         *
         * This will bind the render target, clear it and then unbind it.
         *
         * @param c The color to clear the render target with.
         * @param color Whether to clear the color buffer.
         * @param depth Whether to clear the depth buffer.
         * @param stencil Whether to clear the stencil buffer.
         */
        void clear(const Color &c, bool color, bool depth, bool stencil) const;

        /**
         * Gets the width of the render target.
         *
         * @returns The width of the render target.
         */
        unsigned int getWidth() const;

        /**
         * Sets the width of the render target.
         *
         * @param width The width of the render target.
         */
        void setWidth(unsigned int width);

        /**
         * Gets the height of the render target.
         *
         * @returns The height of the render target.
         */
        unsigned int getHeight() const;

        /**
         * Sets the height of the render target.
         *
         * @param height The height of the render target.
         */
        void setHeight(unsigned int height);

        /**
         * Resizes the render target.
         *
         * @param size The new size of the render target.
         */
        void resize(glm::uvec2 size);

        /**
         * Gets the framebuffer object.
         *
         * @returns The framebuffer object.
         */
        GLuint getFramebuffer() const;

        /**
         * Gets the texture object.
         *
         * @returns The texture object.
         */
        GLuint getTexture() const;

    private:
        unsigned int width;
        unsigned int height;

        GLuint framebuffer = 0;
        GLuint texture = 0;
        GLuint depthBuffer = 0;

        void create();
        void destroy();
        void update();
    };
}