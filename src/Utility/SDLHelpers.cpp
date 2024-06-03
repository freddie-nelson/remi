#include "../../include/Utility/SDLHelpers.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <stdexcept>
#include <cstdlib>
#include <iostream>

bool sdlInitialized = false;
bool remi::isSDLInitialized()
{
    return sdlInitialized;
}

bool sdlMixInitialized = false;
bool remi::isSDLMixInitialized()
{
    return sdlMixInitialized;
}

void remi::initSDL()
{
    if (sdlInitialized)
    {
        return;
    }

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        throw std::runtime_error("Failed to initialize SDL2. SDL2 Error: " + std::string(SDL_GetError()));
    }

    // initialize SDL_mixer
    initSDLMix();

    sdlInitialized = true;
    std::cout << "Remi: SDL initialized." << std::endl;
}

void remi::quitSDL()
{
    quitSDLMix();
    SDL_Quit();

    sdlInitialized = false;
}

void remi::initSDLMix()
{
    if (sdlMixInitialized)
    {
        return;
    }

    int flags = MIX_INIT_OGG | MIX_INIT_MP3 | MIX_INIT_MOD | MIX_INIT_FLAC | MIX_INIT_OPUS | MIX_INIT_MID | MIX_INIT_WAVPACK;
    int initted = Mix_Init(flags);

    if (initted == 0)
    {
        throw std::runtime_error("Failed to initialize SDL_mixer. SDL_mixer Error: " + std::string(Mix_GetError()));
    }
    else if ((initted & flags) != flags)
    {
        if (!(initted & MIX_INIT_OGG))
            std::cout << "Remi (SDL_mixer): Missing support for OGG audio format." << std::endl;
        if (!(initted & MIX_INIT_MP3))
            std::cout << "Remi (SDL_mixer): Missing support for MP3 audio format." << std::endl;
        if (!(initted & MIX_INIT_MOD))
            std::cout << "Remi (SDL_mixer): Missing support for MOD audio format." << std::endl;
        if (!(initted & MIX_INIT_FLAC))
            std::cout << "Remi (SDL_mixer): Missing support for FLAC audio format." << std::endl;
        if (!(initted & MIX_INIT_OPUS))
            std::cout << "Remi (SDL_mixer): Missing support for OPUS audio format." << std::endl;
        if (!(initted & MIX_INIT_MID))
            std::cout << "Remi (SDL_mixer): Missing support for MIDI audio format." << std::endl;
        if (!(initted & MIX_INIT_WAVPACK))
            std::cout << "Remi (SDL_mixer): Missing support for WAVPACK audio format." << std::endl;
    }

    if (Mix_OpenAudio(48000, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        throw std::runtime_error("Failed to initialize SDL_mixer. SDL_mixer Error: " + std::string(Mix_GetError()));
    }

    sdlMixInitialized = true;
}

void remi::quitSDLMix()
{
    Mix_CloseAudio();
    Mix_Quit();

    sdlMixInitialized = false;
}