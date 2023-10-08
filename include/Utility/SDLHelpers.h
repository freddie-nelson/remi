#pragma once

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

/**
 * Draws text centered at (x, y) using the given font and color.
 */
SDL_Rect drawText(SDL_Renderer *renderer, int x, int y, std::string text, TTF_Font *font, SDL_Color color);