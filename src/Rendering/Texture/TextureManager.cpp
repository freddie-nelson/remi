#include "../../../include/Rendering/Texture/TextureManager.h"
#include "../../../include/Rendering/Utility/OpenGLHelpers.h"

#include <stdexcept>
#include <iostream>

Rendering::TextureManager::TextureManager()
{
    texturesUniform.resize(glGetMaxTextureUnits(), 0);
}

Rendering::TextureManager::~TextureManager()
{
    for (size_t i = 0; i < atlases.size(); i++)
    {
        auto textureId = atlasToTextureId[i];
        glDeleteTextures(1, &textureId);
    }
}

Rendering::TextureManager::BoundTexture Rendering::TextureManager::bind(const Texture *texture)
{
    if (texture == nullptr)
    {
        throw std::invalid_argument("TextureManager (bind): texture must not be null.");
    }

    // std::cout << "width: " << texture->getWidth() << ", height: " << texture->getHeight() << std::endl;
    // std::cout << "channels: " << texture->getChannels() << std::endl;

    auto atlasIndex = getContainingAtlas(texture);
    if (atlasIndex == -1)
    {
        // std::cout << "Adding texture to atlas" << std::endl;
        atlasIndex = addTextureToAtlas(texture);
    }

    return {
        .texture = texture,
        .textureSize = glm::vec2(texture->getWidth(), texture->getHeight()),
        .posInAtlas = atlases[atlasIndex].get(texture->getId()),
        .atlasSize = glm::vec2(atlases[atlasIndex].getWidth(), atlases[atlasIndex].getHeight()),
        .textureUnit = atlasIndex,
    };
}

int Rendering::TextureManager::getTextureUnitsUsed() const
{
    return textureUnitsUsed;
}

const std::vector<int> &Rendering::TextureManager::getTexturesUniform() const
{
    return texturesUniform;
}

int Rendering::TextureManager::getContainingAtlas(const Texture *texture)
{
    if (texture == nullptr)
    {
        throw std::invalid_argument("TextureManager (getContainingAtlas): texture must not be null.");
    }

    if (textureToAtlas.count(texture->getId()) == 0)
    {
        return -1;
    }

    return static_cast<int>(textureToAtlas[texture->getId()]);
}

unsigned int Rendering::TextureManager::addTextureToAtlas(const Texture *texture)
{
    if (texture == nullptr)
    {
        throw std::invalid_argument("TextureManager (addTextureToAtlas): texture must not be null.");
    }

    // texture is already in atlas
    if (getContainingAtlas(texture) != -1)
    {
        return getContainingAtlas(texture);
    }

    if (atlases.empty())
    {
        createAtlas();
    }

    // insert into last created atlas as other atlases may not have enough space
    auto atlasIndex = atlases.size() - 1;

    // std::cout << "width: " << texture->getWidth() << ", height: " << texture->getHeight() << std::endl;
    // std::cout << "channels: " << texture->getChannels() << std::endl;

    try
    {
        auto &atlas = atlases[atlasIndex];
        atlas.add(texture);
    }
    catch (std::runtime_error &e)
    {
        // atlas is full so create a new one
        atlasIndex = createAtlas();
        auto atlas = atlases[atlasIndex];

        atlas.add(texture);
    }

    // std::cout << "Added texture to atlas " << atlasIndex << std::endl;

    // reload atlas
    loadAtlas(atlasIndex);

    textureToAtlas[texture->getId()] = atlasIndex;

    return atlasIndex;
}

unsigned int Rendering::TextureManager::createAtlas()
{
    // std::cout << "Creating atlas" << std::endl;

    auto maxTextureUnits = glGetMaxTextureUnits();

    if (atlases.size() >= maxTextureUnits)
    {
        throw std::runtime_error("TextureManager (createAtlas): no more texture units available.");
    }

    atlases.push_back(TextureAtlas());
    texturesUniform[textureUnitsUsed++] = atlases.size() - 1;

    auto atlasIndex = atlases.size() - 1;
    loadAtlas(atlasIndex);

    return atlasIndex;
}

void Rendering::TextureManager::loadAtlas(unsigned int textureUnit)
{
    if (textureUnit >= atlases.size())
    {
        throw std::invalid_argument("TextureManager (loadAtlas): textureUnit must be less than atlases.size().");
    }

    // std::cout << "Loading atlas " << textureUnit << std::endl;
    auto &atlas = atlases[textureUnit];

    // bind texture unit
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    // std::cout << "active texture " << textureUnit << std::endl;

    // generate texture if it doesn't exist
    bool needToGenerate = !atlasToTextureId.contains(textureUnit);
    if (needToGenerate)
    {
        // std::cout << "Generating texture" << std::endl;

        unsigned int textureId;
        glGenTextures(1, &textureId);

        // std::cout << "Generated texture " << textureId << std::endl;

        atlasToTextureId[textureUnit] = textureId;

        glBindTexture(GL_TEXTURE_2D, textureId);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // std::cout << "set texture parameters " << textureId << std::endl;
    }

    auto textureId = atlasToTextureId[textureUnit];

    // send texture to GPU
    if (!needToGenerate)
        glBindTexture(GL_TEXTURE_2D, textureId);

    // std::cout << "bound texture " << textureId << std::endl;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // causing crash on my gtx 1060 3gb
    if (needToGenerate)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, atlas.getWidth(), atlas.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, atlas.getPixels());
    else
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, atlas.getWidth(), atlas.getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, atlas.getPixels());
    // std::cout << "sent texture to GPU " << textureId << std::endl;

    // unbind texture
    // glBindTexture(GL_TEXTURE_2D, 0);
    // std::cout << "unbound texture " << textureId << std::endl;
}
