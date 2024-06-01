#pragma once

#include <string>

#include <SDL2/SDL_mixer.h>

namespace Audio
{
    using AudioId = size_t;

    /**
     * Represents an audio file.
     *
     * Supported audio formats are:
     *  - WAV
     */
    class Audio
    {
    public:
        /**
         * Creates a new audio instance.
         *
         * @param path The path to the audio file.
         */
        Audio(std::string path);

        /**
         * Destroys the audio instance.
         */
        ~Audio();

        /**
         * Loads the audio from the given file.
         *
         * @param path The path to the audio file.
         */
        void fromFile(std::string path);

        /**
         * Gets the path of the audio.
         *
         * @returns The path of the audio.
         */
        const std::string &getPath() const;

    private:
        static AudioId nextId;

        const AudioId id = nextId++;

        Mix_Chunk *chunk = nullptr;
        std::string path;

        void destroy();
    };
}