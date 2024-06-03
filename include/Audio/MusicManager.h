#pragma once

#include "Music.h"

namespace Audio
{
    /**
     * Represents a music manager.
     *
     * The music manager is responsible for managing playback of music tracks.
     */
    class MusicManager
    {
    public:
        /**
         * Creates a new music manager.
         */
        MusicManager();

        /**
         * Destroys the music manager.
         *
         * This will stop the current music track if there is one playing.
         */
        ~MusicManager();

        MusicManager(const MusicManager &) = delete;

        MusicManager(MusicManager &&) = delete;

        /**
         * Plays the given music track.
         *
         * If there is already a music track playing, it will be stopped and the new music track will be played.
         *
         * @param music The music track to play.
         * @param volume The volume to play the music track at. This is in the range [0, 1].
         * @param loops The number of times to loop the music track. 0 means play once, -1 means loop forever.
         */
        void play(const Music &music, float volume = 1.0f, int loops = -1) const;

        /**
         * Pauses the currently playing music track.
         *
         * If there is no music track playing, this function does nothing.
         */
        void pause() const;

        /**
         * Unpauses the currently paused music track.
         *
         * If there is no music track paused, this function does nothing.
         */
        void unpause() const;

        /**
         * Stops the currently playing music track.
         *
         * If there is no music track playing, this function does nothing.
         */
        void stop() const;

        /**
         * Sets the volume of the currently playing music track.
         *
         * If there is no music track playing, this function does nothing.
         *
         * @param volume The volume to set the music track to. This is in the range [0, 1].
         */
        void setVolume(float volume) const;

        /**
         * Gets the volume of the currently playing music track.
         *
         * If there is no music track playing, this function returns 0.
         *
         * @returns The volume of the currently playing music track.
         */
        float getVolume() const;

        /**
         * Gets whether a music track is currently playing.
         *
         * @returns True if a music track is playing, false otherwise.
         */
        bool isPlaying() const;
    };
}