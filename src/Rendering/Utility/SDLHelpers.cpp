#include "../../../include/Rendering/Utility/SDLHelpers.h"

SDL_Rect drawText(SDL_Renderer *renderer, int x, int y, std::string text, TTF_Font *font, SDL_Color color)
{
    SDL_Surface *surface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect rect;
    rect.x = x - surface->w / 2;
    rect.y = y - surface->h / 2;
    rect.w = surface->w;
    rect.h = surface->h;

    SDL_RenderCopy(renderer, texture, NULL, &rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    return rect;
}