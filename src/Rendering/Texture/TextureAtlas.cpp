#include "../../../include/Rendering/Texture/TextureAtlas.h"

#include "../../../include/externals/glad/gl.h"
#include <stdexcept>
#include <vector>
#include <algorithm>

Rendering::TextureAtlas::TextureAtlas(unsigned int padding) : Texture(new unsigned char[1], 1, 1, 4)
{
    this->padding = padding;

    // set width and height
    int maxTextureSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);

    width = maxTextureSize;
    height = maxTextureSize;

    // allocate memory for the pixels
    pixels = new unsigned char[width * height * 4];

    pack();
}

Rendering::TextureAtlas::~TextureAtlas()
{
}

glm::vec2 Rendering::TextureAtlas::add(Texture *texture)
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

    try
    {
        pack();
    }
    catch (std::runtime_error e)
    {
        // remove the texture from the atlas and repack
        textures.erase(texId);
        pack();

        // rethrow the exception
        throw e;
    }

    return posToUV(positions[texId]);
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

    return posToUV(positions[texId]);
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

void Rendering::TextureAtlas::pack()
{
    // clear the atlas
    // sets all pixels to fully transparent black
    for (int i = 0; i < width * height * 4; i++)
    {
        pixels[i] = 0;
    }

    // start in top left corner
    unsigned int curX = 0;
    unsigned int curY = 0;
    unsigned int curRowHeight = 0;

    // sort the textures by height
    std::vector<TextureId> sortedTextures(textures.begin(), textures.end());
    std::sort(sortedTextures.begin(), sortedTextures.end(), [&](TextureId a, TextureId b)
              { return textures[a]->getHeight() > textures[b]->getHeight(); });

    // pack the textures
    for (auto &texId : sortedTextures)
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
        for (int y = 0; y < texHeight; y++)
        {
            for (int x = 0; x < texWidth; x++)
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