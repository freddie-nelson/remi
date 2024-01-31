#pragma once

#include "Rendering/Texture/Texture.h"

#include <vector>

namespace Rendering
{

    /**
     * A texture that is animated.
     *
     * This is a wrapper around a vector of textures that are cycled through by the animation system.
     *
     * The animated texture will own the textures in the vector, this means they must be copied into the animated texture when set.
     */
    class AnimatedTexture
    {
    public:
        /**
         * The mode of the animation.
         *
         * LOOP: The animation will loop from the beginning to the end, continuously.
         * REVERSE: The animation will play from the end to the beginning, continuously.
         * PING_PONG: The animation will play from the beginning to the end, then from the end to the beginning, continuously.
         * ONCE: The animation will play from the beginning to the end, then stop.
         * ONCE_REVERSE: The animation will play from the end to the beginning, then stop.
         */
        enum AnimationMode
        {
            LOOP,
            REVERSE,
            PING_PONG,
            ONCE,
            ONCE_REVERSE,
        };

        /**
         * Create an animated texture.
         *
         * @param frames The paths to the frames of the animation.
         * @param duration The duration of the animation.
         * @param mode The mode of the animation.
         * @param flip Whether or not the animation textures should be flipped.
         */
        AnimatedTexture(std::vector<std::string> frames, float duration, AnimationMode mode = AnimationMode::LOOP, bool flip = true);

        /**
         * Create an animated texture.
         *
         * @param frames The frames of the animation.
         * @param duration The duration of the animation.
         * @param mode The mode of the animation.
         */
        AnimatedTexture(std::vector<Texture> frames, float duration, AnimationMode mode = AnimationMode::LOOP);

        /**
         * Destroy the animated texture.
         */
        ~AnimatedTexture();

        /**
         * Set the frames of the animation.
         *
         * @param frames The paths to the frames of the animation.
         * @param flip Whether or not the animation textures should be flipped.
         */
        void setFrames(std::vector<std::string> frames, bool flip = true);

        /**
         * Set the frames of the animation.
         *
         * @param frames The frames of the animation.
         */
        void setFrames(std::vector<Texture> frames);

        /**
         * Get the frames of the animation.
         *
         * @returns The frames of the animation.
         */
        const std::vector<Texture> &getFrames();

        /**
         * Get a frame of the animation.
         *
         * @returns The nth frame of the animation given by index.
         */
        const Texture &getFrame(unsigned int index);

        /**
         * Gets the current frame of the animation.
         *
         * @returns The current frame of the animation.
         */
        const Texture &getCurrentFrame();

        /**
         * Gets the index of the current frame of the animation.
         *
         * @returns The index of the current frame of the animation.
         */
        size_t getCurrentFrameIndex();

        /**
         * Get the duration of the animation.
         *
         * @returns The duration of the animation.
         */
        float getDuration();

        /**
         * Get the duration of a single frame of the animation.
         *
         * @returns The duration of a single frame of the animation.
         */
        float getFrameDuration();

        /**
         * Get the number of frames in the animation.
         *
         * @returns The number of frames in the animation.
         */
        size_t getFrameCount();

        /**
         * Set the mode of the animation.
         *
         * @param mode The mode of the animation.
         */
        void setMode(AnimationMode mode);

        /**
         * Get the mode of the animation.
         *
         * @returns The mode of the animation.
         */
        AnimationMode getMode();

        /**
         * Get the current time of the animation.
         *
         * @returns The current time of the animation.
         */
        float getTime();

        /**
         * Set the current time of the animation.
         *
         * @param time The current time of the animation.
         */
        void setTime(float time);

        /**
         * Steps the animation forward in time by dt.
         *
         * @param dt The amount of time to step the animation forward by.
         */
        void step(float dt);

    private:
        /**
         * The frames of the animation.
         */
        std::vector<Texture> frames;

        /**
         * The duration of the animation.
         */
        float duration;

        /**
         * The mode of the animation.
         */
        AnimationMode mode;

        /**
         * The current time of the animation.
         */
        float time = 0;

        /**
         * Whether or not the animation is reversing during ping ponging.
         */
        bool reversing;
    };
}