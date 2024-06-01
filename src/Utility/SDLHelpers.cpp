#include "../../include/Utility/SDLHelpers.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <stdexcept>

void remi::initSDL()
{
    if (SDL_WasInit(SDL_INIT_EVERYTHING) != 0)
    {
        // sdl is already initialized
        return;
    }

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        throw std::runtime_error("Failed to initialize SDL2. SDL2 Error: " + std::string(SDL_GetError()));
    }

    // initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        throw std::runtime_error("Failed to initialize SDL_mixer. SDL_mixer Error: " + std::string(Mix_GetError()));
    }
}