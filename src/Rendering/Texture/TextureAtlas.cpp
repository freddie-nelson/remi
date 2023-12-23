#include "../../../include/Rendering/Texture/TextureAtlas.h"

#include "../../../include/externals/glad/gl.h"
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cstring>
#include <math.h>

unsigned int atlasSize = -1;
unsigned int Rendering::TextureAtlas::getAtlasSize()
{
    if (atlasSize != -1)
    {
        return atlasSize;
    }

    int maxTextureSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);

    atlasSize = std::min(TextureAtlas::MAX_ATLAS_SIZE, static_cast<unsigned int>(maxTextureSize * TextureAtlas::ATLAS_SIZE_MULTIPLIER));

    return atlasSize;
}

Rendering::TextureAtlas::TextureAtlas(unsigned int padding)
{
    this->padding = padding;

    // std::cout << "Max texture size: " << maxTextureSize << std::endl;
    // std::cout << "Atlas size: " << maxTextureSize * TextureAtlas::ATLAS_SIZE_MULTIPLIER << std::endl;

    width = TextureAtlas::getAtlasSize();
    height = width;

    // allocate memory for the pixels
    pixels = new unsigned char[width * height * 4];

    pack();
}

Rendering::TextureAtlas::~TextureAtlas()
{
    delete pixels;
}

glm::vec2 Rendering::TextureAtlas::add(const Texture *texture)
{
    if (texture == nullptr)
    {
        throw std::invalid_argument("TextureAtlas (add): texture must not be null.");
    }

    // if the texture is too big for the atlas
    if (texture->getWidth() > width || texture->getHeight() > height)
    {
        throw std::invalid_argument("TextureAtlas (add): texture is too big for the atlas.");
    }

    auto texId = texture->getId();
    textures[texId] = texture;

    // std::cout << "Adding texture to atlas: " << texId << std::endl;

    try
    {
        pack();
    }
    catch (std::runtime_error e)
    {
        // remove the texture from the atlas and repack
        textures.erase(texId);
        positions.erase(texId);
        pack();

        // rethrow the exception
        throw e;
    }

    return positions[texId];
}

void Rendering::TextureAtlas::remove(TextureId texId)
{
    if (!has(texId))
    {
        return;
    }

    textures.erase(texId);
    positions.erase(texId);

    pack();
}

glm::vec2 Rendering::TextureAtlas::get(TextureId texId)
{
    if (!has(texId))
    {
        throw std::invalid_argument("TextureAtlas (get): texture is not in the atlas.");
    }

    return positions[texId];
}

bool Rendering::TextureAtlas::has(TextureId texId)
{
    return textures.count(texId) != 0;
}

unsigned int Rendering::TextureAtlas::getPadding() const
{
    return padding;
}

void Rendering::TextureAtlas::setPadding(unsigned int padding)
{
    this->padding = padding;

    pack();
}

unsigned int Rendering::TextureAtlas::getWidth() const
{
    return width;
}

unsigned int Rendering::TextureAtlas::getHeight() const
{
    return height;
}

const unsigned char *Rendering::TextureAtlas::getPixels() const
{
    return pixels;
}

void Rendering::TextureAtlas::pack()
{
    // std::cout << "Packing atlas" << std::endl;

    // clear the atlas
    // sets all pixels to fully transparent black
    memset(pixels, 0, width * height * 4);

    // std::cout << "cleared atlas" << std::endl;

    // start in top left corner
    unsigned int curX = 0;
    unsigned int curY = 0;
    unsigned int curRowHeight = 0;

    // sort the textures by height
    std::vector<TextureId> sortedTextures;
    for (auto [texId, texture] : textures)
    {
        sortedTextures.push_back(texId);
    }

    std::sort(sortedTextures.begin(), sortedTextures.end(), [&](TextureId a, TextureId b)
              { return textures[a]->getHeight() > textures[b]->getHeight(); });

    // std::cout << "sorted textures" << std::endl;

    // pack the textures
    for (auto texId : sortedTextures)
    {
        auto texture = textures[texId];

        auto texWidth = texture->getWidth();
        auto texHeight = texture->getHeight();
        auto texPixels = texture->getPixels();

        auto texChannels = texture->getChannels();

        // this should never happen
        if (texChannels != 4)
        {
            throw std::invalid_argument("TextureAtlas (pack): texture must have 4 channels, something has went severly wrong.");
        }

        // texture can't fit in row, x
        if (curX + texWidth > width)
        {
            curX = 0;
            curY += curRowHeight + padding;
            curRowHeight = 0;
        }

        // texture can't fit in atlas
        if (curY + texHeight > height)
        {
            throw std::runtime_error("TextureAtlas (pack): atlas is full.");
        }

        // expand row height if needed
        if (texHeight > curRowHeight)
        {
            curRowHeight = texHeight;
        }

        // copy the texture to the atlas
        for (unsigned int y = 0; y < texHeight; y++)
        {
            for (unsigned int x = 0; x < texWidth; x++)
            {
                unsigned int texIndex = (y * texWidth + x) * 4;

                unsigned int atlasX = curX + x;
                unsigned int atlasY = curY + y;
                unsigned int atlasIndex = (atlasY * width + atlasX) * 4;

                pixels[atlasIndex] = texPixels[texIndex];
                pixels[atlasIndex + 1] = texPixels[texIndex + 1];
                pixels[atlasIndex + 2] = texPixels[texIndex + 2];
                pixels[atlasIndex + 3] = texPixels[texIndex + 3];
            }
        }

        // save the position of the texture
        positions[texId] = glm::vec2(curX, curY);

        // move the cursor
        curX += texWidth + padding;
        curY += texHeight + padding;
    }
}

glm::vec2 Rendering::TextureAtlas::posToUV(glm::vec2 pos)
{
    if (pos.x < 0 || pos.y < 0)
    {
        throw std::invalid_argument("TextureAtlas (posToUV): pos must be greater than 0.");
    }

    if (pos.x > width || pos.y > height)
    {
        throw std::invalid_argument("TextureAtlas (posToUV): pos must be less than the atlas size.");
    }

    return glm::vec2(pos.x / width, pos.y / height);
}