#pragma once

#include "SoundEffect.h"

#include <unordered_map>
#include <functional>

namespace Audio
{

    /**
     * Represents a sound manager.
     *
     * The sound manager is responsible for managing playback of sound effects.
     */
    class SoundEffectManager
    {
    public:
        using PlayingId = size_t;

        using FinishCallback = std::function<void(PlayingId)>;

        /**
         * Creates a new sound effect manager.
         */
        SoundEffectManager();

        /**
         * Destroys the sound effect manager.
         */
        ~SoundEffectManager();

        /**
         * Plays the given sound effect.
         *
         * @param soundEffect The sound effect to play.
         * @param volume The volume to play the sound effect at. This is in the range [0, 1].
         * @param loops The number of times to loop the sound effect. 0 means play once, -1 means loop forever.
         * @param finishCallback The callback to call when the sound effect finishes playing. This is not called when paused.
         *
         * @returns The id of the playing sound effect. Can be used to control the sound effects playback.
         */
        PlayingId play(const SoundEffect &soundEffect, float volume = 1.0f, int loops = 0, FinishCallback finishCallback = nullptr) const;

        /**
         * Pauses the sound with the given id.
         *
         * @param id The id of the sound to pause.
         */
        void pause(PlayingId id) const;

        /**
         * Unpauses the sound effect with the given id.
         *
         * @param id The id of the sound effect to unpause.
         */
        void unpause(PlayingId id) const;

        /**
         * Stops the sound effect with the given id.
         *
         * @param id The id of the sound effect to stop.
         */
        void stop(PlayingId id) const;

        /**
         * Gets whether the sound effect with the given id is playing.
         *
         * @param id The id of the sound effect to check.
         *
         * @returns True if the sound effect is playing, false otherwise.
         */
        bool isPlaying(PlayingId id) const;

        /**
         * Sets the volume of the sound effect with the given id.
         *
         * @param id The id of the sound effect to set the volume of.
         * @param volume The volume to set the sound effect to. This is in the range [0, 1].
         */
        void setVolume(PlayingId id, float volume) const;

        /**
         * Gets the volume of the sound effect with the given id.
         *
         * @param id The id of the sound effect to get the volume of.
         */
        float getVolume(PlayingId id) const;

        /**
         * Stops all playing sound effects.
         */
        void stopAll() const;

    private:
        /**
         * A map of playing ids to their sdl mixer channel.
         */
        mutable std::unordered_map<PlayingId, int> playing;

        /**
         * A map of sdl mixer channels to the id of the sound effect they are playing.
         */
        mutable std::unordered_map<int, PlayingId> channels;

        mutable std::unordered_map<PlayingId, FinishCallback> finishCallbacks;

        mutable PlayingId nextId = 0;

        static std::vector<SoundEffectManager *> instances;

        static void handleSoundEffectFinished(int channel);
    };
}