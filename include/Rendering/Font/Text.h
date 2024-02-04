#pragma once

#include "Font.h"

#include <unordered_map>
#include <string>

namespace Rendering
{

    /**
     * Represents a piece of text.
     *
     * This class can generate Mesh2D components for containing text.
     */
    class Text
    {
    public:
        enum TextAlignment
        {
            LEFT,
            CENTRE,
            RIGHT,
        };

        /**
         * Creates a text instance.
         *
         * @param text The text to render.
         * @param font The font to use.
         */
        Text(std::string text, const Font &font);

        /**
         * Gets the mesh component to render.
         *
         * @param align The horizontal alignment of the text.
         *
         * @return The mesh component to render.
         */
        const Mesh2D &mesh(TextAlignment align = TextAlignment::CENTRE) const;

    private:
        std::string text;
        const Font &font;

        /**
         * The text meshs aligned to the given alignment.
         */
        mutable std::unordered_map<TextAlignment, Mesh2D> meshs;

        /**
         * Creates a mesh.
         *
         * Saves the mesh in the meshs map.
         *
         * For CENTRE, CENTRE alignment, the mesh is saved in the mesh variable too.
         *
         * @param align The horizontal alignment of the text.
         */
        const Mesh2D &createMesh(TextAlignment align = TextAlignment::CENTRE) const;

        struct LineInfo
        {
            std::vector<float> lineWidths;
            float maxLineWidth;
        };

        LineInfo getLineWidths(const std::vector<std::string> &lines) const;

        float getAdvance(char c) const;

        float getWidth(char c) const;
    };

}