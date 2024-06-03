#include "../../include/Audio/MusicManager.h"
#include "../../include/Utility/SDLHelpers.h"

#include <stdexcept>
#include <iostream>

Audio::MusicManager::MusicManager()
{
    if (instances.size() > 0)
    {
        std::cout << "MusicManager (constructor): Multiple instances of MusicManager detected. This may lead to unexpected behavior." << std::endl;
    }

    // initialize SDL in case it is not initialized
    remi::initSDL();

    // add this instance to instances
    instances.push_back(this);

    Mix_HookMusicFinished(handleMusicFinished);
}

Audio::MusicManager::~MusicManager()
{
    stop();

    // remove this instance from instances
    auto it = std::find(instances.begin(), instances.end(), this);
    if (it != instances.end())
    {
        instances.erase(it);
    }
}

void Audio::MusicManager::play(const Music &music, float volume, int loops, FinishCallback finishCallback) const
{
    stop();

    if (Mix_PlayMusic(music.getMusic(), loops) == -1)
    {
        throw std::runtime_error("MusicManager (play): Failed to play music. SDL_mixer Error: " + std::string(Mix_GetError()));
    }

    setVolume(volume);

    if (finishCallback != nullptr)
    {
        this->finishCallback = finishCallback;
    }
}

void Audio::MusicManager::pause() const
{
    if (!isPlaying())
    {
        return;
    }

    Mix_PauseMusic();
}

void Audio::MusicManager::unpause() const
{
    if (!isPlaying())
    {
        return;
    }

    Mix_ResumeMusic();
}

void Audio::MusicManager::stop() const
{
    if (!isPlaying())
    {
        return;
    }

    Mix_HaltMusic();

    finishCallback = nullptr;
}

void Audio::MusicManager::setVolume(float volume) const
{
    if (volume < 0.0f || volume > 1.0f)
    {
        throw std::invalid_argument("MusicManager (setVolume): Volume must be in the range [0, 1].");
    }

    Mix_VolumeMusic(static_cast<int>(volume * MIX_MAX_VOLUME));
}

float Audio::MusicManager::getVolume() const
{
    return static_cast<float>(Mix_VolumeMusic(-1)) / MIX_MAX_VOLUME;
}

bool Audio::MusicManager::isPlaying() const
{
    return Mix_PlayingMusic() == 1;
}

void Audio::MusicManager::handleMusicFinished()
{
    for (auto instance : instances)
    {
        if (instance->finishCallback != nullptr)
        {
            instance->finishCallback();
            instance->finishCallback = nullptr;
        }
    }
}