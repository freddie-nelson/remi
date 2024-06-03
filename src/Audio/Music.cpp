#include "../../include/Audio/Music.h"
#include "../../include/Utility/SDLHelpers.h"

#include <stdexcept>

Audio::MusicId Audio::Music::nextId = 0;

Audio::Music::Music(std::string path)
{
    // initialize SDL in case it is not initialized
    remi::initSDL();

    fromFile(path);
}

Audio::Music::~Music()
{
    destroy();
}

void Audio::Music::fromFile(std::string path)
{
    if (music != nullptr)
    {
        destroy();
    }

    bool isWav = path.find(".wav") != std::string::npos;
    bool isOgg = path.find(".ogg") != std::string::npos;
    bool isMp3 = path.find(".mp3") != std::string::npos;
    bool isFlac = path.find(".flac") != std::string::npos;

    if (!isWav && !isOgg && !isMp3 && !isFlac)
    {
        throw std::invalid_argument("Music (fromFile): Unsupported audio format. Make sure your audio is a supported format and ends with the correct file extension.");
    }

    music = Mix_LoadMUS(path.c_str());
    if (music == nullptr)
    {
        throw std::runtime_error("Music (fromFile): Failed to load audio file from '" + path + "'. SDL_mixer Error: " + std::string(Mix_GetError()));
    }

    this->path = path;
}

const std::string &Audio::Music::getPath() const
{
    return path;
}

Mix_Music *Audio::Music::getMusic() const
{
    return music;
}

void Audio::Music::destroy()
{
    if (music != nullptr)
    {
        Mix_FreeMusic(music);
    }

    music = nullptr;
    path = "";
}