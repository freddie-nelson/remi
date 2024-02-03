#include "../../../include/Rendering/Texture/Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <iostream>
#include <stdexcept>

Rendering::TextureId Rendering::Texture::nextId = 0;

Rendering::Texture::Texture(const Rendering::Texture &t)
{
    fromPixels(t.pixels, t.width, t.height, t.channels);
}

Rendering::Texture::Texture(std::string path, bool flip)
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
        stbi_image_free(pixels);
    }
}

void Rendering::Texture::fromFile(std::string path, bool flip)
{
    if (path.empty())
    {
        throw std::invalid_argument("path must not be empty.");
    }

    if (pixels != nullptr)
    {
        stbi_image_free(pixels);
    }

    stbi_set_flip_vertically_on_load(flip);

    int w, h, numChannels;
    pixels = stbi_load(path.c_str(), &w, &h, &numChannels, 0);

    if (pixels == nullptr)
    {
        std::cout << "Failed to load image: " << path << std::endl;

        char *error = (char *)stbi_failure_reason();
        std::cout << "Error: " << error << std::endl;

        throw std::invalid_argument("Failed to load image.");
    }

    if (numChannels != 3 && numChannels != 4)
    {
        throw std::invalid_argument("Image must have 3 or 4 channels.");
    }

    if (numChannels == 3)
    {
        auto rgba = toRGBA(pixels, w, h);
        stbi_image_free(pixels);

        pixels = rgba;

        hasTransparency = false;
    }
    else
    {
        // check if the image has transparency
        hasTransparency = false;

        for (int i = 3; i < w * h; i += 4)
        {
            if (pixels[i] < 255)
            {
                hasTransparency = true;
                break;
            }
        }
    }

    std::cout << "Loaded image: " << path << std::endl;

    width = w;
    height = h;
    channels = 4;
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
        stbi_image_free(this->pixels);
    }

    if (channels == 3)
    {
        this->pixels = toRGBA(pixels, width, height);
        hasTransparency = false;
    }
    else
    {
        this->pixels = new unsigned char[width * height * channels];
        memcpy(this->pixels, pixels, width * height * channels);

        // check if the image has transparency
        hasTransparency = false;

        for (unsigned int i = 3; i < width * height; i += 4)
        {
            if (this->pixels[i] < 255)
            {
                hasTransparency = true;
                break;
            }
        }
    }

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
        stbi_image_free(this->pixels);
    }

    auto c = color.getColor();
    this->pixels = new unsigned char[width * height * 4];

    for (unsigned int i = 0; i < width * height; i++)
    {
        pixels[i * 4] = c.r * 255;
        pixels[i * 4 + 1] = c.g * 255;
        pixels[i * 4 + 2] = c.b * 255;
        pixels[i * 4 + 3] = c.a * 255;
    }

    // std::cout << "Color: " << c.r << ", " << c.g << ", " << c.b << ", " << c.a << std::endl;
    // std::cout << "width: " << width << ", height: " << height << std::endl;

    this->width = width;
    this->height = height;
    this->channels = 4;

    hasTransparency = color.a() < 1.0f;
}

size_t Rendering::Texture::getId() const
{
    return id;
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

bool Rendering::Texture::isTransparent() const
{
    return hasTransparency;
}

Rendering::Texture &Rendering::Texture::operator=(const Texture &t)
{
    fromPixels(t.pixels, t.width, t.height, t.channels);

    return *this;
}

unsigned char *Rendering::Texture::toRGBA(unsigned char *pixels, unsigned int width, unsigned int height)
{
    unsigned char *rgba = new unsigned char[width * height * 4];

    for (unsigned int i = 0; i < width * height; i++)
    {
        rgba[i * 4] = pixels[i * 3];
        rgba[i * 4 + 1] = pixels[i * 3 + 1];
        rgba[i * 4 + 2] = pixels[i * 3 + 2];
        rgba[i * 4 + 3] = 255;
    }

    return rgba;
}