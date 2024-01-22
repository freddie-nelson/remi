#pragma once

#include "../Mesh/Mesh.h"

#include <ttf2mesh.h>
#include <string>
#include <vector>
#include <unordered_map>

namespace Rendering
{
    enum FontQuality
    {
        LOW = TTF_QUALITY_LOW,
        MEDIUM = TTF_QUALITY_NORMAL,
        HIGH = TTF_QUALITY_HIGH,
    };

    // enum FontWeight
    // {
    //     THIN = TTF_WEIGHT_THIN,
    //     EXTRALIGHT = TTF_WEIGHT_EXTRALIGHT,
    //     LIGHT = TTF_WEIGHT_LIGHT,
    //     NORMAL = TTF_WEIGHT_NORMAL,
    //     MEDIUM = TTF_WEIGHT_MEDIUM,
    //     SEMIBOLD = TTF_WEIGHT_DEMIBOLD,
    //     BOLD = TTF_WEIGHT_BOLD,
    //     EXTRABOLD = TTF_WEIGHT_EXTRABOLD,
    //     BLACK = TTF_WEIGHT_BLACK,
    // };

    /**
     * Represents a font that can be used to render text.
     *
     * Only supports TTF fonts.
     *
     * Internally uses ttf2mesh to generate a mesh from the font.
     */
    class Font
    {
    public:
        /**
         * Returns a list of paths to all system fonts.
         *
         * @return A list of paths to all system fonts.
         */
        static std::vector<std::string> getSystemFonts();

        /**
         * Creates a new font from the given path.
         *
         * @param path The path to the ttf font file.
         * @param quality The quality of the generate meshs.
         */
        Font(std::string path, FontQuality quality = FontQuality::MEDIUM);

        /**
         * Destroys the font.
         */
        ~Font();

        /**
         * Returns the path to the font file.
         *
         * @return The path to the font file.
         */
        const std::string &getPath() const;

        /**
         * Returns the quality of the generated meshs.
         *
         * @return The quality of the generated meshs.
         */
        FontQuality getQuality() const;

        /**
         * Represents a symbol of the font.
         *
         * A symbol is a utf16 character.
         *
         * Contains the mesh of the symbol and the glyph of the symbol.
         */
        struct Symbol
        {
            Mesh2D mesh;
            ttf_glyph_t *glyph;
        };

        /**
         * Returns the data for the given symbol.
         *
         * A symbol is a utf16 character.
         *
         * @param symbol The symbol to get the data for.
         *
         * @returns The data for the given symbol.
         */
        const Symbol &getSymbol(int symbol) const;

        /**
         * Returns whether the font has the given symbol.
         *
         * @param symbol The symbol to check.
         *
         * @return Whether the font has the given symbol.
         */
        bool hasSymbol(int symbol) const;

        /**
         * Returns the height of a line of text.
         *
         * This is the height of the tallest symbol in the font.
         *
         * @return The height of a line of text.
         */
        float getLineHeight() const;

        /**
         * Returns the width of a space.
         *
         * @return The width of a space.
         */
        float getSpaceWidth() const;

    private:
        std::string path;
        FontQuality quality;

        /**
         * The height of a line of text.
         *
         * This is the height of the tallest symbol in the font.
         */
        float lineHeight = 0;

        /**
         * The width of a space.
         */
        float spaceWidth = 0;

        /**
         * Maps a utf16 symbol to it's mesh.
         */
        std::unordered_map<int, Symbol> symbols;

        ttf_t *font = nullptr;

        /**
         * Loads the font.
         *
         * Creates a mesh for each glyph of the font.
         */
        void load();

        /**
         * Loads a glpyh from the font.
         *
         * Saves the mesh of the glyph in the symbols map.
         *
         * @param glyph The glyph to load.
         */
        void loadGlyph(ttf_glyph_t &glyph);
    };
}