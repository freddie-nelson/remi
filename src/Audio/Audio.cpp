#include "../../include/Audio/Audio.h"
#include "../../include/Utility/SDLHelpers.h"

#include <stdexcept>
#include <iostream>

Audio::AudioId Audio::Audio::nextId = 0;

Audio::Audio::Audio(std::string path)
{
    // initialize SDL in case it is not initialized
    remi::initSDL();

    fromFile(path);
}

Audio::Audio::~Audio()
{
    destroy();
}

void Audio::Audio::fromFile(std::string path)
{
    if (chunk != nullptr)
    {
        destroy();
    }

    bool isWav = path.find(".wav") != std::string::npos;
    if (!isWav)
    {
        throw std::invalid_argument("Audio (fromFile): Unsupported audio format. Make sure your audio ends in '.wav' and is a wav file.");
    }

    chunk = Mix_LoadWAV(path.c_str());
    if (chunk == nullptr)
    {
        throw std::runtime_error("Audio (fromFile): Failed to load audio file from '" + path + "'. SDL_mixer Error: " + std::string(Mix_GetError()));
    }

    this->path = path;
}

const std::string &Audio::Audio::getPath() const
{
    return path;
}

Mix_Chunk *Audio::Audio::getChunk() const
{
    return chunk;
}

void Audio::Audio::destroy()
{
    if (chunk != nullptr)
    {
        Mix_FreeChunk(chunk);
    }

    chunk = nullptr;
    path = "";
}