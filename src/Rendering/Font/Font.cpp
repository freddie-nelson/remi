#include "../../../include/Rendering/Font/Font.h"

#include <stdexcept>
#include <algorithm>
#include <iostream>

std::vector<std::string> Rendering::Font::getSystemFonts()
{
#ifdef __EMSCRIPTEN__
    std::cout << "Font::getSystemFonts() is not implemented for Emscripten." << std::endl;

    return {};
#endif

    auto list = ttf_list_system_fonts(nullptr);
    if (list == nullptr || list[0] == nullptr)
    {
        return {};
    }

    std::vector<std::string> fonts;

    for (int i = 0; list[i] != nullptr; i++)
    {
        fonts.push_back(list[i]->filename);
    }

    return fonts;
}

Rendering::Font::Font(std::string path, FontQuality quality)
    : path(path), quality(quality)
{
    load();
}

Rendering::Font::~Font()
{
    ttf_free(font);
}

const std::string &Rendering::Font::getPath() const
{
    return path;
}

Rendering::FontQuality Rendering::Font::getQuality() const
{
    return quality;
}

const Rendering::Font::Symbol &Rendering::Font::getSymbol(int symbol) const
{
    if (!hasSymbol(symbol))
    {
        throw std::runtime_error("Symbol " + std::to_string(symbol) + " not found.");
    }

    return symbols.at(symbol);
}

bool Rendering::Font::hasSymbol(int symbol) const
{
    return symbols.contains(symbol);
}

float Rendering::Font::getLineHeight() const
{
    return lineHeight;
}

float Rendering::Font::getSpaceWidth() const
{
    return spaceWidth;
}

void Rendering::Font::load()
{
    std::cout << "Loading font: " << path << std::endl;

    // load font if not already loaded
    if (font == nullptr)
    {
        std::cout << "Loading font from file: " << path.c_str() << std::endl;

        ttf_load_from_file(path.c_str(), &font, false);
        std::cout << "Loaded font from file: " << path << std::endl;

        if (font == nullptr)
        {
            throw std::runtime_error("Failed to load font from file: " + path);
        }
    }

    std::cout << "Loaded font: " << path << std::endl;

    for (int i = 0; i < font->nglyphs; i++)
    {
        auto &glyph = font->glyphs[i];

        // skip glyphs without outline
        if (glyph.outline == nullptr)
        {
            continue;
        }

        loadGlyph(glyph);
    }

    // set space width
    int spaceId = ttf_find_glyph(font, ' ');
    spaceWidth = font->glyphs[spaceId].advance;

    std::cout << "Loaded glyphs: " << path << std::endl;
}

void Rendering::Font::loadGlyph(ttf_glyph_t &glyph)
{
    // load font if not already loaded
    if (font == nullptr)
    {
        throw std::runtime_error("Font not loaded.");
    }

    // create mesh
    ttf_mesh_t *mesh = nullptr;

    auto result = ttf_glyph2mesh(&glyph, &mesh, FontQuality::MEDIUM, TTF_FEATURES_DFLT);
    if (result != TTF_DONE)
    {
        // std::cout << "Failed to create mesh for glyph. Symbol: '" << std::to_string(static_cast<char>(glyph.symbol)) << "'. Error: " << result << "." << std::endl;
        // return;
        throw std::runtime_error("Failed to create mesh for glyph. Symbol: '" + std::string(new char{static_cast<char>(glyph.symbol)}) + "'. Error: " + std::to_string(result) + ".");
    }

    // vertices
    std::vector<glm::vec2> vertices(mesh->nvert);

    for (int i = 0; i < mesh->nvert; i++)
    {
        vertices[i] = glm::vec2(mesh->vert[i].x, mesh->vert[i].y);
    }

    // indices
    std::vector<unsigned int> indices(mesh->nfaces * 3);

    for (int i = 0; i < mesh->nfaces; i++)
    {
        indices[i * 3] = mesh->faces[i].v1;
        indices[i * 3 + 1] = mesh->faces[i].v2;
        indices[i * 3 + 2] = mesh->faces[i].v3;
    }

    // create symbol
    symbols[glyph.symbol] = Symbol{Mesh2D(std::move(vertices), std::move(indices), true), &glyph};

    // free mesh
    ttf_free_mesh(mesh);

    // update line height
    lineHeight = std::max(lineHeight, symbols[glyph.symbol].mesh.getAABB().getHeight());
}