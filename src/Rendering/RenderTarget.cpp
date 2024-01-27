#include "../../include/Rendering/RenderTarget.h"

#include <stdexcept>
#include <string>

Rendering::RenderTarget::RenderTarget(unsigned int width, unsigned int height) : width(width), height(height)
{
    create();
}

Rendering::RenderTarget::~RenderTarget()
{
    destroy();
}

void Rendering::RenderTarget::bind(TextureManager &textureManager) const
{
    textureManager.bindRenderTarget(texture);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
}

void Rendering::RenderTarget::unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int Rendering::RenderTarget::getWidth() const
{
    return width;
}

void Rendering::RenderTarget::setWidth(unsigned int width)
{
    auto oldWidth = this->width;
    this->width = width;

    if (oldWidth != width)
    {
        update();
    }
}

unsigned int Rendering::RenderTarget::getHeight() const
{
    return height;
}

void Rendering::RenderTarget::setHeight(unsigned int height)
{
    auto oldHeight = this->height;
    this->height = height;

    if (oldHeight != height)
    {
        update();
    }
}

void Rendering::RenderTarget::resize(glm::uvec2 size)
{
    auto oldWidth = width;
    width = size.x;

    auto oldHeight = height;
    height = size.y;

    if (oldWidth != width || oldHeight != height)
    {
        update();
    }
}

GLuint Rendering::RenderTarget::getFramebuffer() const
{
    return framebuffer;
}

GLuint Rendering::RenderTarget::getTexture() const
{
    return texture;
}

void Rendering::RenderTarget::create()
{
    // destroy any existing objects
    destroy();

    // create framebuffer
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // create texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

    // set filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // create depth buffer render buffer object
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

    // attach texture to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // set draw buffers list
    GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, drawBuffers);

    std::cout << "width: " << width << ", height: " << height << std::endl;

    // check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        throw std::runtime_error("RenderTarget (create): framebuffer is not complete. Status: " + glFramebufferStatusToString(glCheckFramebufferStatus(GL_FRAMEBUFFER)) + ".");
    }

    // unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Rendering::RenderTarget::destroy()
{
    if (framebuffer != 0)
    {
        glDeleteFramebuffers(1, &framebuffer);
        framebuffer = 0;
    }

    if (texture != 0)
    {
        glDeleteTextures(1, &texture);
        texture = 0;
    }

    if (depthBuffer != 0)
    {
        glDeleteRenderbuffers(1, &depthBuffer);
        depthBuffer = 0;
    }
}

void Rendering::RenderTarget::update()
{
    // destroy any existing objects
    destroy();

    // create new objects
    create();
}
