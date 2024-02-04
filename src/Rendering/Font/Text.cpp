#include "../../../include/Rendering/Font/Text.h"
#include "../../../include/Rendering/Mesh/Triangulate.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <algorithm>

Rendering::Text::Text(std::string text, const Font &font) : text(text), font(font)
{
    createMesh();
}

const Rendering::Mesh2D &Rendering::Text::mesh(TextAlignment align) const
{
    if (!meshs.contains(align))
    {
        createMesh(align);
    }

    return meshs.at(align);
}

const Rendering::Mesh2D &Rendering::Text::createMesh(TextAlignment align) const
{
    std::vector<std::string> lines;
    boost::split(lines, text, boost::is_any_of("\n"));

    // precalculate line widths
    auto [lineWidths, maxLineWidth] = getLineWidths(lines);

    // create mesh
    std::vector<glm::vec2> vertices;
    std::vector<unsigned int> indices;

    for (size_t i = 0; i < lines.size(); i++)
    {
        auto &line = lines[i];

        float initialXAdvance = 0;
        switch (align)
        {
        case TextAlignment::LEFT:
            initialXAdvance = 0;
            break;
        case TextAlignment::CENTRE:
            initialXAdvance = (maxLineWidth / 2) - (lineWidths[i] / 2);
            break;
        case TextAlignment::RIGHT:
            initialXAdvance = maxLineWidth - lineWidths[i];
            break;
        }

        float xAdvance = initialXAdvance;
        float yAdvance = i * -font.getLineHeight();

        for (auto &c : line)
        {
            auto charAdvance = getAdvance(c);

            // no renderable glyph
            if (!font.hasSymbol(c))
            {
                xAdvance += charAdvance;
                continue;
            }

            auto &symbol = font.getSymbol(c);
            auto const glyph = symbol.glyph;

            auto &symbolVertices = symbol.mesh.getVertices();
            auto &symbolIndices = symbol.mesh.getIndices();

            auto &aabb = symbol.mesh.getAABB();
            auto height = aabb.getHeight();
            auto width = aabb.getWidth();

            // copy symbol indices
            for (auto &i : symbolIndices)
            {
                indices.push_back(i + vertices.size());
            }

            // copy symbol vertices
            for (auto &v : symbolVertices)
            {
                vertices.emplace_back(glm::vec2(v.x + xAdvance, v.y + yAdvance));
            }

            xAdvance += charAdvance;
        }
    }

    Mesh2D mesh(std::move(vertices), std::move(indices));

    // translate mesh centre to align text
    auto &aabb = mesh.getAABB();
    auto &centre = aabb.getCentre();

    // save
    meshs.insert_or_assign(align, std::move(mesh));

    return meshs.at(align);
}

Rendering::Text::LineInfo Rendering::Text::getLineWidths(const std::vector<std::string> &lines) const
{
    float maxLineWidth = 0;
    std::vector<float> lineWidths(lines.size());

    for (size_t i = 0; i < lineWidths.size(); i++)
    {
        auto &line = lines[i];
        lineWidths[i] = 0;

        for (size_t j = 0; j < line.size(); j++)
        {
            auto &c = line[j];

            if (j == line.size() - 1)
            {
                // last char so only add width of char
                lineWidths[i] += getWidth(c);
            }
            else
            {
                lineWidths[i] += getAdvance(c);
            }
        }

        maxLineWidth = std::max(maxLineWidth, lineWidths[i]);
    }

    return LineInfo{std::move(lineWidths), maxLineWidth};
}

float Rendering::Text::getAdvance(char c) const
{
    switch (c)
    {
    case ' ':
        return font.getSpaceWidth();
    case '\r':
        return 0;
    case '\t':
        return font.getSpaceWidth() * 4;
    default:
        if (!font.hasSymbol(c))
        {
            return 0;
        }
        else
        {
            return font.getSymbol(c).glyph->advance;
        }
    }
}

float Rendering::Text::getWidth(char c) const
{
    switch (c)
    {
    case ' ':
        return font.getSpaceWidth();
    case '\r':
        return 0;
    case '\t':
        return font.getSpaceWidth() * 4;
    default:
        if (!font.hasSymbol(c))
        {
            return 0;
        }
        else
        {
            return font.getSymbol(c).mesh.getAABB().getWidth();
        }
    }
}