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

void Rendering::RenderTarget::bind(TextureManager &textureManager, bool bindFramebuffer) const
{
    textureManager.bindRenderTarget(readTexture);

    if (bindFramebuffer)
    {
        glViewport(0, 0, width, height);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    }
}

void Rendering::RenderTarget::unbind(TextureManager &textureManager, bool drawToReadTexture) const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    textureManager.unbindRenderTarget();

    if (drawToReadTexture)
        updateReadTexture();
}

void Rendering::RenderTarget::updateReadTexture() const
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, readFramebuffer);

    // bind read texture
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}

void Rendering::RenderTarget::clear(const Color &c, bool color, bool depth, bool stencil) const
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glClearWithColor(c, color, depth, stencil);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int Rendering::RenderTarget::getWidth() const
{
    return width;
}

void Rendering::RenderTarget::setWidth(unsigned int width)
{
    if (this->width == width)
    {
        return;
    }

    this->width = width;
    update();
}

unsigned int Rendering::RenderTarget::getHeight() const
{
    return height;
}

void Rendering::RenderTarget::setHeight(unsigned int height)
{
    if (this->height == height)
    {
        return;
    }

    this->height = height;
    update();
}

void Rendering::RenderTarget::resize(glm::uvec2 size)
{
    if (size.x == width && size.y == height)
    {
        return;
    }

    width = size.x;
    height = size.y;

    update();
}

GLuint Rendering::RenderTarget::getFramebuffer() const
{
    return framebuffer;
}

GLuint Rendering::RenderTarget::getDrawTexture() const
{
    return drawTexture;
}

GLuint Rendering::RenderTarget::getReadTexture() const
{
    return readTexture;
}

void Rendering::RenderTarget::create()
{
    // destroy any existing objects
    destroy();

    // create framebuffer
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // create draw texture
    drawTexture = createTexture();

    // attach texture to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, drawTexture, 0);

    // create depth/stencil buffer render buffer object
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

    // unbind render buffer
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // attach depth/stencil buffer to framebuffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

    // std::cout << "width: " << width << ", height: " << height << std::endl;

    // check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        throw std::runtime_error("RenderTarget (create): framebuffer is not complete. Status: " + glFramebufferStatusToString(glCheckFramebufferStatus(GL_FRAMEBUFFER)) + ".");
    }

    // unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // create read framebuffer
    glGenFramebuffers(1, &readFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, readFramebuffer);

    // create read texture
    readTexture = createTexture();

    // attach texture to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, readTexture, 0);

    // check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        throw std::runtime_error("RenderTarget (create): read framebuffer is not complete. Status: " + glFramebufferStatusToString(glCheckFramebufferStatus(GL_FRAMEBUFFER)) + ".");
    }
}

void Rendering::RenderTarget::destroy()
{
    if (framebuffer != 0)
    {
        glDeleteFramebuffers(1, &framebuffer);
        framebuffer = 0;
    }

    if (drawTexture != 0)
    {
        glDeleteTextures(1, &drawTexture);
        drawTexture = 0;
    }


    if (depthBuffer != 0)
    {
        glDeleteRenderbuffers(1, &depthBuffer);
        depthBuffer = 0;
    }

    if (readFramebuffer != 0)
    {
        glDeleteFramebuffers(1, &readFramebuffer);
        readFramebuffer = 0;
    }

    if (readTexture != 0)
    {
        glDeleteTextures(1, &readTexture);
        readTexture = 0;
    }
}

void Rendering::RenderTarget::update()
{
    // destroy any existing objects
    destroy();

    // create new objects
    create();
}

GLuint Rendering::RenderTarget::createTexture() {
    GLuint texture;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    // set filtering options
    // TODO: make filter mode configurable?
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}