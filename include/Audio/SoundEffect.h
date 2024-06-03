#pragma once

#include <SDL2/SDL_mixer.h>

#include <string>

namespace Audio
{
    using SoundEffectId = size_t;

    /**
     * Represents a sound effect.
     *
     * A sound effect is a short audio clip that can be played in response to a game event.
     *
     * Sound effects are not movable or copyable.
     *
     * Supported audio formats are:
     *  - WAV
     *  - MP3
     *  - OGG
     *  - FLAC
     */
    class SoundEffect
    {
    public:
        /**
         * Creates a new sound effect.
         *
         * @param path The path to the sound effect's audio file.
         */
        SoundEffect(std::string path);

        /**
         * Destroys the sound effect instance.
         */
        ~SoundEffect();

        SoundEffect(const SoundEffect &) = delete;

        SoundEffect(SoundEffect &&) = delete;

        /**
         * Loads the sound effect from the given file.
         *
         * @param path The path to the sound effect's audio file.
         */
        void fromFile(std::string path);

        /**
         * Gets the path of the sound effect's audio file.
         *
         * @returns The path of the sound effect's audio file.
         */
        const std::string &getPath() const;

        /**
         * Gets the sdl mixer chunk of the sound effect.
         *
         * @note This is intended for internal use by the engine. Only use this if you know what you are doing.
         *
         * @returns The chunk of the sound effect.
         */
        Mix_Chunk *getChunk() const;

    private:
        static SoundEffectId nextId;

        const SoundEffectId id = nextId++;

        std::string path;
        Mix_Chunk *chunk = nullptr;

        void destroy();
    };
}