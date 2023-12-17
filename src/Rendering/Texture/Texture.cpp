#include "../../../include/Rendering/Texture/Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../../../include/externals/stb/stb_image.h"

#include <stdexcept>

Rendering::Texture::Texture(std::string path)
{
    fromFile(path);
}

Rendering::Texture::Texture(unsigned char *pixels, unsigned int width, unsigned int height, unsigned int channels)
{
    fromPixels(pixels, width, height, channels);
}

Rendering::Texture::Texture(Color color, unsigned int width, unsigned int height)
{
    fromColor(color, width, height);
}

Rendering::Texture::~Texture()
{
    if (pixels != nullptr)
    {
        delete pixels;
    }
}

void Rendering::Texture::fromFile(std::string path)
{
    if (path.empty())
    {
        throw std::invalid_argument("path must not be empty.");
    }

    if (pixels != nullptr)
    {
        delete pixels;
    }

    int w, h, numChannels;
    pixels = stbi_load(path.c_str(), &w, &h, &numChannels, 0);

    if (pixels == nullptr)
    {
        throw std::invalid_argument("Failed to load image.");
    }

    if (numChannels != 3 && numChannels != 4)
    {
        throw std::invalid_argument("Image must have 3 or 4 channels.");
    }

    width = w;
    height = h;
    channels = numChannels;
}

void Rendering::Texture::fromPixels(unsigned char *pixels, unsigned int width, unsigned int height, unsigned int channels)
{
    if (pixels == nullptr)
    {
        throw std::invalid_argument("pixels must not be null.");
    }

    if (width == 0 || height == 0)
    {
        throw std::invalid_argument("width and height must be greater than 0.");
    }

    if (channels != 3 && channels != 4)
    {
        throw std::invalid_argument("channels must be 3 or 4.");
    }

    if (this->pixels != nullptr)
    {
        delete this->pixels;
    }

    this->pixels = pixels;
    this->width = width;
    this->height = height;
    this->channels = channels;
}

void Rendering::Texture::fromColor(Color color, unsigned int width, unsigned int height)
{
    if (width == 0 || height == 0)
    {
        throw std::invalid_argument("width and height must be greater than 0.");
    }

    if (this->pixels != nullptr)
    {
        delete this->pixels;
    }

    auto c = color.getColor();
    this->pixels = new unsigned char[width * height * 4];

    for (int i = 0; i < width * height; i++)
    {
        pixels[i * 4] = c.r * 255;
        pixels[i * 4 + 1] = c.g * 255;
        pixels[i * 4 + 2] = c.b * 255;
        pixels[i * 4 + 3] = c.a * 255;
    }

    this->width = width;
    this->height = height;
    this->channels = 4;
}

unsigned int Rendering::Texture::getWidth() const
{
    return width;
}

unsigned int Rendering::Texture::getHeight() const
{
    return height;
}

unsigned int Rendering::Texture::getChannels() const
{
    return channels;
}

unsigned char *Rendering::Texture::getPixels() const
{
    return pixels;
}
