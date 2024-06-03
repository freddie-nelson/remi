#pragma once

#include <SDL2/SDL_mixer.h>

#include <string>

namespace Audio
{
    using MusicId = size_t;

    /**
     * Represents a music track.
     *
     * Music is not movable or copyable.
     *
     * Supported audio formats are:
     *  - WAV
     *  - MP3
     *  - OGG
     *  - FLAC
     */
    class Music
    {
    public:
        /**
         * Creates a new music track.
         *
         * @param path The path to the music track's audio file.
         */
        Music(std::string path);

        /**
         * Destroys the music track instance.
         */
        ~Music();

        Music(const Music &) = delete;

        Music(Music &&) = delete;

        /**
         * Loads the music track from the given file.
         *
         * @param path The path to the music track's audio file.
         */
        void fromFile(std::string path);

        /**
         * Gets the path of the music track's audio file.
         *
         * @returns The path of the music track's audio file.
         */
        const std::string &getPath() const;

        /**
         * Gets the SDL mixer music of the music track.
         *
         * @note This is intended for internal use by the engine. Only use this if you know what you are doing.
         *
         * @returns The music of the music track.
         */
        Mix_Music *getMusic() const;

    private:
        static MusicId nextId;

        const MusicId id = nextId++;

        std::string path;
        Mix_Music *music = nullptr;

        void destroy();
    };
}