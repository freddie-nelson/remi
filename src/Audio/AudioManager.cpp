#include "../../include/Audio/AudioManager.h"
#include "../../include/Utility/SDLHelpers.h"

#include <SDL2/SDL_mixer.h>

#include <stdexcept>

Audio::AudioManager::AudioManager()
{
    // initialize SDL in case it is not initialized
    remi::initSDL();

    // add this instance to instances
    instances.push_back(this);

    // set audio finished callback
    Mix_ChannelFinished(audioFinishedCallback);
}

Audio::AudioManager::~AudioManager()
{
    stopAll();

    // remove this instance from instances
    auto it = std::find(instances.begin(), instances.end(), this);
    if (it != instances.end())
    {
        instances.erase(it);
    }
}

Audio::PlayingId Audio::AudioManager::play(const Audio &audio, float volume, int loops, AudioFinishCallback finishCallback) const
{
    auto id = nextId++;

    auto channel = Mix_PlayChannel(-1, audio.getChunk(), loops);
    if (channel == -1)
    {
        throw std::runtime_error("AudioManager (play): Failed to play audio '" + audio.getPath() + "': " + std::string(Mix_GetError()));
    }

    // remove old playing data
    // if channel is being reused then the audio is not playing anymore
    if (channels.contains(channel))
    {
        auto id = channels[channel];
        channels.erase(channel);
        playing.erase(id);
        finishCallbacks.erase(id);
    }

    playing[id] = channel;
    channels[channel] = id;

    if (finishCallback != nullptr)
    {
        finishCallbacks[id] = finishCallback;
    }

    setVolume(id, volume);

    return id;
}

void Audio::AudioManager::pause(PlayingId id) const
{
    if (!playing.contains(id))
    {
        throw std::invalid_argument("AudioManager (pause): Audio with playing id " + std::to_string(id) + " does not exist.");
    }

    auto channel = playing[id];
    Mix_Pause(channel);
}

void Audio::AudioManager::unpause(PlayingId id) const
{
    if (!playing.contains(id))
    {
        throw std::invalid_argument("AudioManager (unpause): Audio with playing id " + std::to_string(id) + " does not exist.");
    }

    auto channel = playing[id];
    Mix_Resume(channel);
}

void Audio::AudioManager::stop(PlayingId id) const
{
    if (!playing.contains(id))
    {
        throw std::invalid_argument("AudioManager (stop): Audio with playing id " + std::to_string(id) + " does not exist.");
    }

    auto channel = playing[id];
    Mix_HaltChannel(channel);

    playing.erase(id);
    channels.erase(channel);
}

bool Audio::AudioManager::isPlaying(PlayingId id) const
{
    if (!playing.contains(id))
    {
        return false;
    }

    auto channel = playing[id];
    return Mix_Playing(channel) == 1;
}

void Audio::AudioManager::setVolume(PlayingId id, float volume) const
{
    if (volume < 0.0f || volume > 1.0f)
    {
        throw std::invalid_argument("AudioManager (setVolume): Volume must be in the range [0, 1].");
    }

    if (!playing.contains(id))
    {
        throw std::invalid_argument("AudioManager (setVolume): Audio with playing id " + std::to_string(id) + " does not exist.");
    }

    auto channel = playing[id];
    Mix_Volume(channel, static_cast<int>(volume * MIX_MAX_VOLUME));
}

float Audio::AudioManager::getVolume(PlayingId id) const
{
    if (!playing.contains(id))
    {
        throw std::runtime_error("AudioManager (getVolume): Audio with playing id " + std::to_string(id) + " does not exist.");
    }

    auto channel = playing[id];
    return static_cast<float>(Mix_Volume(channel, -1)) / MIX_MAX_VOLUME;
}

void Audio::AudioManager::stopAll() const
{
    // using this method to stop all audio instead of Mix_HaltChannel(-1) so
    // only the audio played by this manager is stopped
    for (auto &[id, channel] : playing)
    {
        Mix_HaltChannel(channel);
    }

    playing.clear();
    channels.clear();
}

std::vector<Audio::AudioManager *> Audio::AudioManager::instances = {};

void Audio::AudioManager::audioFinishedCallback(int channel)
{
    // find instance that was using this channel
    AudioManager *instance = nullptr;

    for (auto &i : instances)
    {
        if (i->channels.contains(channel))
        {
            instance = i;
            break;
        }
    }

    if (instance == nullptr)
    {
        return;
    }

    auto id = instance->channels[channel];

    // cleanup audio inside instance
    instance->playing.erase(id);
    instance->channels.erase(channel);

    // call finish callback if it exists
    if (instance->finishCallbacks.contains(id))
    {
        instance->finishCallbacks[id](id);
    }

    instance->finishCallbacks.erase(id);
}