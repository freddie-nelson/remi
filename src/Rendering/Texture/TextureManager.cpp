#include "../../../include/Rendering/Texture/TextureManager.h"
#include "../../../include/Rendering/Utility/OpenGLHelpers.h"

#include <cstring>
#include <stdexcept>
#include <iostream>
#include <unordered_set>

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

    // std::cout << "texture bound" << std::endl;

    textureLastUsed[texture->getId()] = lastUsedCount;

    return {
        .texture = texture,
        .textureSize = glm::vec2(texture->getWidth(), texture->getHeight()),
        .posInAtlas = atlases[atlasIndex]->get(texture->getId()),
        .atlasSize = glm::vec2(atlases[atlasIndex]->getWidth(), atlases[atlasIndex]->getHeight()),
        .textureUnit = atlasIndex,
    };
}

std::vector<Rendering::TextureManager::BoundTexture> Rendering::TextureManager::bind(const std::vector<const Texture *> &textures)
{
    auto boundTextures = std::vector<BoundTexture>();
    boundTextures.reserve(textures.size());

    std::unordered_set<unsigned int> atlasesToReload;

    for (auto texture : textures)
    {
        auto atlasIndex = getContainingAtlas(texture);
        if (atlasIndex == -1)
        {
            atlasIndex = addTextureToAtlas(texture, true, false);
            atlasesToReload.emplace(atlasIndex);
        }

        textureLastUsed[texture->getId()] = lastUsedCount;

        boundTextures.push_back({
            .texture = texture,
            .textureSize = glm::vec2(texture->getWidth(), texture->getHeight()),
            .posInAtlas = atlases[atlasIndex]->get(texture->getId()),
            .atlasSize = glm::vec2(atlases[atlasIndex]->getWidth(), atlases[atlasIndex]->getHeight()),
            .textureUnit = atlasIndex,
        });
    }

    // reload atlases
    for (auto atlas : atlasesToReload)
    {
        // std::cout << "reloading atlas " << atlas << std::endl;
        try
        {
            atlases[atlas]->pack();
            loadAtlas(atlas);
        }
        catch (std::runtime_error &e)
        {
            // recursively unbind and rebind all textures in the atlas
            // until they all fit
            // ! probably very slow
            unbind(textures);
            createAtlas();

            return bind(textures);
        }
    }

    return boundTextures;
}

void Rendering::TextureManager::unbind(const Texture *texture)
{
    if (texture == nullptr)
    {
        throw std::invalid_argument("TextureManager (unbind): texture must not be null.");
    }

    auto atlasIndex = getContainingAtlas(texture);
    if (atlasIndex == -1)
    {
        // texture is not bound
        return;
    }

    // remove from atlas
    auto &atlas = atlases[atlasIndex];
    atlas->remove(texture->getId());

    // remove from textureToAtlas
    textureToAtlas.erase(texture->getId());

    // reload atlas
    loadAtlas(atlasIndex);
}

void Rendering::TextureManager::unbind(const std::vector<const Texture *> &textures)
{
    if (textures.empty())
    {
        return;
    }

    std::unordered_set<size_t> atlasesToRepack;

    for (auto texture : textures)
    {
        if (texture == nullptr)
        {
            throw std::invalid_argument("TextureManager (unbind): texture must not be null.");
        }

        if (!textureToAtlas.contains(texture->getId()))
        {
            // texture is not bound
            continue;
        }

        // remove from atlas but don't repack yet
        auto atlas = textureToAtlas[texture->getId()];
        atlases[atlas]->remove(texture->getId(), false);

        atlasesToRepack.emplace(std::move(atlas));

        textureToAtlas.erase(texture->getId());
    }

    // repack and reload atlases
    for (auto atlas : atlasesToRepack)
    {
        atlases[atlas]->pack();
        loadAtlas(atlas);
    }
}

void Rendering::TextureManager::bindRenderTarget(GLuint texture)
{
    if (texture == 0)
    {
        throw std::invalid_argument("TextureManager (bindRenderTarget): render target texture must not be 0.");
    }

    glActiveTexture(GL_TEXTURE0 + renderTargetTextureUnit);
    glBindTexture(GL_TEXTURE_2D, texture);
}

void Rendering::TextureManager::unbindRenderTarget()
{
    glActiveTexture(GL_TEXTURE0 + renderTargetTextureUnit);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Rendering::TextureManager::unbindUnusedTextures()
{

    long long lastUsedCutoff = lastUsedCount - lastUsedTextureRemovalThreshold;

    std::vector<const Texture *> texturesToRemove;

    // remove textures that haven't been used in a while
    for (auto it = textureLastUsed.begin(); it != textureLastUsed.end(); it++)
    {
        auto &[texId, lastUsed] = *it;

        if (lastUsed < lastUsedCutoff)
        {
            texturesToRemove.push_back(atlases[textureToAtlas[texId]]->getTexture(texId));
        }
    }

    unbind(texturesToRemove);

    lastUsedCount++;
}

int Rendering::TextureManager::getTextureUnitsUsed() const
{
    return textureUnitsUsed;
}

int Rendering::TextureManager::getReservedTextureUnits() const
{
    return reservedTextureUnits;
}

int Rendering::TextureManager::getRenderTargetTextureUnit() const
{
    return renderTargetTextureUnit;
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

unsigned int Rendering::TextureManager::addTextureToAtlas(const Texture *texture, bool repack, bool reload)
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
        atlas->add(texture, repack);
    }
    catch (std::runtime_error &e)
    {
        // atlas is full so create a new one
        atlasIndex = createAtlas();
        auto atlas = atlases[atlasIndex];

        atlas->add(texture, repack);
    }

    // reload atlas
    if (reload)
        loadAtlas(atlasIndex);

    textureToAtlas[texture->getId()] = atlasIndex;

    return atlasIndex;
}

unsigned int Rendering::TextureManager::createAtlas(bool loadAtlasOnCreation)
{
    // std::cout << "Creating atlas" << std::endl;

    auto maxTextureUnits = glGetMaxTextureUnits();

    if (atlases.size() >= (maxTextureUnits - reservedTextureUnits))
    {
        throw std::runtime_error("TextureManager (createAtlas): no more texture units available.");
    }

    atlases.push_back(new TextureAtlas());
    texturesUniform[textureUnitsUsed++] = atlases.size() - 1;

    auto atlasIndex = atlases.size() - 1;

    if (loadAtlasOnCreation)
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

    // std::cout << "sending texture to GPU " << textureId << std::endl;
    // std::cout << "pixels: " << atlas->getPixels()[0] << std::endl;

    if (needToGenerate)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, atlas->getWidth(), atlas->getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, atlas->getPixels());
    }
    else
    {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, atlas->getWidth(), atlas->getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, atlas->getPixels());
    }

    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, atlas->getWidth(), atlas->getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, atlas->getPixels());
    // std::cout << "sent texture to GPU " << textureId << std::endl;

    // unbind texture
    // glBindTexture(GL_TEXTURE_2D, 0);
    // std::cout << "unbound texture " << textureId << std::endl;
}
