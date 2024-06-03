#include "../../include/Audio/SoundEffect.h"
#include "../../include/Utility/SDLHelpers.h"

#include <stdexcept>
#include <iostream>

Audio::SoundEffectId Audio::SoundEffect::nextId = 0;

Audio::SoundEffect::SoundEffect(std::string path)
{
    // initialize SDL in case it is not initialized
    remi::initSDL();

    fromFile(path);
}

Audio::SoundEffect::~SoundEffect()
{
    destroy();
}

void Audio::SoundEffect::fromFile(std::string path)
{
    if (chunk != nullptr)
    {
        destroy();
    }

    bool isWav = path.find(".wav") != std::string::npos;
    bool isOgg = path.find(".ogg") != std::string::npos;
    bool isMp3 = path.find(".mp3") != std::string::npos;
    bool isFlac = path.find(".flac") != std::string::npos;

    if (!isWav && !isOgg && !isMp3 && !isFlac)
    {
        throw std::invalid_argument("SoundEffect (fromFile): Unsupported audio format. Make sure your audio is a supported format and ends with the correct file extension.");
    }

    chunk = Mix_LoadWAV(path.c_str());
    if (chunk == nullptr)
    {
        throw std::runtime_error("SoundEffect (fromFile): Failed to load audio file from '" + path + "'. SDL_mixer Error: " + std::string(Mix_GetError()));
    }

    this->path = path;
}

const std::string &Audio::SoundEffect::getPath() const
{
    return path;
}

Mix_Chunk *Audio::SoundEffect::getChunk() const
{
    return chunk;
}

void Audio::SoundEffect::destroy()
{
    if (chunk != nullptr)
    {
        Mix_FreeChunk(chunk);
    }

    chunk = nullptr;
    path = "";
}