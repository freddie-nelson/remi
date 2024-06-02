#pragma once

#include "Audio.h"

#include <unordered_map>
#include <functional>

namespace Audio
{
    using PlayingId = size_t;

    using AudioFinishCallback = std::function<void(PlayingId)>;

    /**
     * Represents an audio manager.
     *
     * The audio manager is responsible for managing audio playback.
     */
    class AudioManager
    {
    public:
        /**
         * Creates a new audio manager.
         */
        AudioManager();

        /**
         * Destroys the audio manager.
         */
        ~AudioManager();

        /**
         * Plays the given audio.
         *
         * @param audio The audio to play.
         * @param volume The volume to play the audio at. This is in the range [0, 1].
         * @param loops The number of times to loop the audio. 0 means play once, -1 means loop forever.
         * @param finishCallback The callback to call when the audio finishes playing. This is not called when paused.
         *
         * @returns The id of the playing audio. Can be used to control the audios playback.
         */
        PlayingId play(const Audio &audio, float volume = 1.0f, int loops = 0, AudioFinishCallback finishCallback = nullptr) const;

        /**
         * Pauses the audio with the given id.
         *
         * @param id The id of the audio to pause.
         */
        void pause(PlayingId id) const;

        /**
         * Unpauses the audio with the given id.
         *
         * @param id The id of the audio to unpause.
         */
        void unpause(PlayingId id) const;

        /**
         * Stops the audio with the given id.
         *
         * @param id The id of the audio to stop.
         */
        void stop(PlayingId id) const;

        /**
         * Gets whether the audio with the given id is playing.
         *
         * @param id The id of the audio to check.
         *
         * @returns True if the audio is playing, false otherwise.
         */
        bool isPlaying(PlayingId id) const;

        /**
         * Sets the volume of the audio with the given id.
         *
         * @param id The id of the audio to set the volume of.
         * @param volume The volume to set the audio to. This is in the range [0, 1].
         */
        void setVolume(PlayingId id, float volume) const;

        /**
         * Gets the volume of the audio with the given id.
         *
         * @param id The id of the audio to get the volume of.
         */
        float getVolume(PlayingId id) const;

        /**
         * Stops all audio.
         */
        void stopAll() const;

    private:
        /**
         * A map of playing audio ids to their sdl mixer channel.
         */
        mutable std::unordered_map<PlayingId, int> playing;

        /**
         * A map of sdl mixer channels to the id of the audio they are playing.
         */
        mutable std::unordered_map<int, PlayingId> channels;

        mutable std::unordered_map<PlayingId, AudioFinishCallback> finishCallbacks;

        mutable PlayingId nextId = 0;

        static std::vector<AudioManager *> instances;

        static void audioFinishedCallback(int channel);
    };
}