#include "../../../include/Rendering/Font/MemoizedText.h"

std::unordered_map<const Rendering::Font *, std::unordered_map<std::string, Rendering::Text *>> Rendering::MemoizedText::memoizedTexts;

const Rendering::Text &Rendering::MemoizedText::text(std::string text, const Font *font)
{
    if (!memoizedTexts.contains(font))
    {
        // no memoized texts for this font
        memoizedTexts[font] = std::unordered_map<std::string, Text *>();
    }
    else if (memoizedTexts[font].contains(text))
    {
        // cached text
        return *memoizedTexts[font].at(text);
    }

    // need to create a new text
    auto const &pair = memoizedTexts[font].emplace(std::move(text), new Text(text, *font));

    return *pair.first->second;
}