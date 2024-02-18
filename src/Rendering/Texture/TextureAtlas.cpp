#include "../../../include/Rendering/Texture/TextureAtlas.h"
#include "../../../include/gl.h"

#include <stdexcept>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cstring>
#include <math.h>
#include <list>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

float Rendering::TextureAtlas::TextureAtlas::ATLAS_SIZE_MULTIPLIER = 0.50f;

unsigned int Rendering::TextureAtlas::TextureAtlas::MAX_ATLAS_SIZE = 4096;

unsigned int atlasSize = 0;
bool atlasSizeSet = false;

unsigned int Rendering::TextureAtlas::getAtlasSize()
{
    if (atlasSizeSet)
    {
        return atlasSize;
    }

    int maxTextureSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);

    atlasSize = std::min(TextureAtlas::MAX_ATLAS_SIZE, static_cast<unsigned int>(maxTextureSize * TextureAtlas::ATLAS_SIZE_MULTIPLIER));
    atlasSizeSet = true;

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

glm::vec2 Rendering::TextureAtlas::add(const Texture *texture, bool repack)
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

    // std::cout << "width: " << texture->getWidth() << ", height: " << texture->getHeight() << std::endl;
    // std::cout << "channels: " << texture->getChannels() << std::endl;

    // std::cout << "textures" << std::endl;
    // std::cout << "width: " << textures[texId]->getWidth() << ", height: " << textures[texId]->getHeight() << std::endl;
    // std::cout << "channels: " << textures[texId]->getChannels() << std::endl;

    // std::cout << "Adding texture to atlas: " << texId << std::endl;

    if (!repack)
    {
        return positions.contains(texId) ? positions.at(texId) : glm::vec2(-1, -1);
    }

    try
    {
        // std::cout << "width: " << textures[texId]->getWidth() << ", height: " << textures[texId]->getHeight() << std::endl;
        // std::cout << "channels: " << textures[texId]->getChannels() << std::endl;

        pack();
    }
    catch (std::runtime_error &e)
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

void Rendering::TextureAtlas::remove(TextureId texId, bool repack)
{
    if (!has(texId))
    {
        return;
    }

    textures.erase(texId);
    positions.erase(texId);

    if (repack)
        pack();
}

const Rendering::Texture *Rendering::TextureAtlas::getTexture(TextureId texId)
{
    if (!has(texId))
    {
        throw std::invalid_argument("TextureAtlas (getTexture): texture is not in the atlas.");
    }

    return textures[texId];
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

    // std::cout << "width: " << width << ", height: " << height << std::endl;
    // std::cout << "padding: " << padding << std::endl;
    // std::cout << "textures: " << textures.size() << std::endl;
    // std::cout << "pixels: " << pixels[0] << ", " << pixels[1] << ", " << pixels[2] << ", " << pixels[3] << std::endl;

    // std::cout << "clearing atlas" << std::endl;

    // clear the atlas
    // sets all pixels to fully transparent black
    memset(pixels, 0, width * height * 4);

    // std::cout << "cleared atlas" << std::endl;

    // sort the textures by height
    std::vector<TextureId> sortedTextures;
    for (auto &t : textures)
    {
        sortedTextures.push_back(t.first);
        positions[t.first] = glm::vec2{0, 0};
    }

    std::sort(sortedTextures.begin(), sortedTextures.end(), [&](TextureId a, TextureId b)
              { return textures[a]->getHeight() > textures[b]->getHeight(); });

    // std::cout << "sorted textures" << std::endl;

    // ! weird bug here on wasm desktop where textures get corrupted? junk values end up being for width, height and channels
    // ? fixed?

    // represents empty spaces in the atlas
    std::list<AtlasSpace> spaces;

    // add the first space
    // the first space is the entire atlas
    spaces.push_front({glm::vec2(0, 0), width, height});

    // pack the textures
    for (auto texId : sortedTextures)
    {
        auto &texture = textures[texId];

        auto texWidth = texture->getWidth();
        auto texHeight = texture->getHeight();
        auto texPixels = texture->getPixels();

        // this should never happen
        auto texChannels = texture->getChannels();
        if (texChannels != 4)
        {
            throw std::invalid_argument("TextureAtlas (pack): texture must have 4 channels, something has went severly wrong. Channels recieved: " + std::to_string(texChannels) + ".");
        }

        // find a space for the texture
        auto spaceIt = spaces.end();

        for (auto it = spaces.begin(); it != spaces.end(); it++)
        {
            auto &s = *it;

            if (s.width >= texWidth && s.height >= texHeight)
            {
                spaceIt = it;
                break;
            }
        }

        // if the texture doesn't fit in any of the spaces
        if (spaceIt == spaces.end())
        {
            throw std::runtime_error("TextureAtlas (pack): can't find space for texture '" + std::to_string(texId) + "'.");
        }

        // make copy so we can erase the space without invalidating the iterator
        auto space = *spaceIt;
        spaces.erase(spaceIt);

        // std::cout << "space: " << space.position.x << ", " << space.position.y << ", " << space.width << ", " << space.height << std::endl;

        // copy the texture to the atlas
        for (unsigned int y = 0; y < texHeight; y++)
        {
            size_t texRow = y * texWidth * 4;
            size_t atlasRow = (space.position.y + y) * width * 4;

            size_t atlasIndex = atlasRow + space.position.x * 4;

            auto texPixelsRow = &texPixels[texRow];
            auto pixelsStart = &pixels[atlasIndex];

            memcpy(pixelsStart, texPixelsRow, texWidth * 4);

            // for (unsigned int x = 0; x < texWidth; x++)
            // {
            //     unsigned int texIndex = (y * texWidth + x) * 4;

            //     unsigned int atlasX = curX + x;
            //     unsigned int atlasY = curY + y;
            //     unsigned int atlasIndex = (atlasY * width + atlasX) * 4;

            //     pixels[atlasIndex] = texPixels[texIndex];
            //     pixels[atlasIndex + 1] = texPixels[texIndex + 1];
            //     pixels[atlasIndex + 2] = texPixels[texIndex + 2];
            //     pixels[atlasIndex + 3] = texPixels[texIndex + 3];
            // }
        }

        // save the position of the texture
        positions[texId] = glm::vec2(space.position);

        // subdivide the space

        // remove the space

        // space below texture
        int spaceBelowHeight = static_cast<int>(space.height) - texHeight - padding;
        AtlasSpace spaceBelow = {glm::vec2(space.position.x, space.position.y + texHeight + padding), space.width, spaceBelowHeight < 0 ? 0 : spaceBelowHeight};

        if (spaceBelow.width > 0 && spaceBelow.height > 0)
        {
            spaces.push_front(std::move(spaceBelow));
        }

        // space to right of texture
        int spaceRightWidth = static_cast<int>(space.width) - texWidth - padding;
        AtlasSpace spaceRight = {glm::vec2(space.position.x + texWidth + padding, space.position.y), spaceRightWidth < 0 ? 0 : spaceRightWidth, texHeight};

        if (spaceRight.width > 0 && spaceRight.height > 0)
        {
            spaces.push_front(std::move(spaceRight));
        }
    }

    // test atlas output
    // writeTestAtlas(4);
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

void Rendering::TextureAtlas::writeTestAtlas(int sample)
{
    if (sample < 1)
    {
        throw std::invalid_argument("TextureAtlas (writeTestAtlas): sample must be greater than 1.");
    }

    if (width % sample != 0)
    {
        throw std::invalid_argument("TextureAtlas (writeTestAtlas): width must be divisible by sample.");
    }

    std::string filename = "atlas.png";

    int w = width / sample;
    int h = height / sample;

    unsigned char *pixels = new unsigned char[w * h * 4];

    size_t j = 0;

    for (size_t i = 0; i < width * height; i += sample)
    {

        pixels[j * 4] = this->pixels[i * 4];
        pixels[j * 4 + 1] = this->pixels[i * 4 + 1];
        pixels[j * 4 + 2] = this->pixels[i * 4 + 2];
        pixels[j * 4 + 3] = this->pixels[i * 4 + 3];

        j++;

        // skip to next row
        if (((i / 4) % width) == 0 && i != 0)
        {
            i += (sample - 1) * width * 4;
        }
    }

    std::cout << "Writing atlas to '" << filename << "'" << std::endl;
    stbi_write_png(filename.c_str(), w, h, 4, pixels, w * 4);

    delete pixels;
}