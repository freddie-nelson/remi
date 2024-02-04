#pragma once

#include "Font.h"
#include "Text.h"

#include <unordered_map>

namespace Rendering
{
    /**
     * A class for creating text instances with memoization.
     *
     * This can be used to avoid creating the same text multiple times, as it is quite expensive.
     */
    class MemoizedText
    {
    public:
        MemoizedText() = delete;

        ~MemoizedText() = delete;

        /**
         * Creates the given text with the given font.
         *
         * If the text has already been created with the given font, the existing text is returned.
         *
         * @param text The text to render.
         * @param font The font to use.
         *
         * @return The text instance.
         */
        static const Text &text(std::string text, const Font *font);

    private:
        static std::unordered_map<const Font *, std::unordered_map<std::string, Text *>> memoizedTexts;
    };
}