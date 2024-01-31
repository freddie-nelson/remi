#pragma once

#include "Texture.h"

#include <vector>

namespace Rendering
{

    /**
     * A texture that is animated.
     *
     * This is a wrapper around a vector of textures that are cycled through by the animation system.
     *
     * The animated texture will own the textures in the frames vector, they will be copied into the animated texture when set and destroyed with the animated texture.
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
         * @param duration The duration of the animation in milliseconds.
         * @param mode The mode of the animation.
         * @param flip Whether or not the animation textures should be flipped.
         */
        AnimatedTexture(const std::vector<std::string> &frames, float duration, AnimationMode mode = AnimationMode::LOOP, bool flip = true);

        /**
         * Create an animated texture.
         *
         * @param frames The frames of the animation.
         * @param duration The duration of the animation in milliseconds.
         * @param mode The mode of the animation.
         */
        AnimatedTexture(const std::vector<Texture *> &frames, float duration, AnimationMode mode = AnimationMode::LOOP);

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
        void setFrames(const std::vector<std::string> &frames, bool flip = true);

        /**
         * Set the frames of the animation.
         *
         * @param frames The frames of the animation.
         */
        void setFrames(const std::vector<Texture *> &frames);

        /**
         * Get the frames of the animation.
         *
         * @returns The frames of the animation.
         */
        const std::vector<Texture *> &getFrames() const;

        /**
         * Get a frame of the animation.
         *
         * @returns The nth frame of the animation given by index.
         */
        const Texture &getFrame(unsigned int index) const;

        /**
         * Gets the current frame of the animation.
         *
         * @returns The current frame of the animation.
         */
        const Texture &getCurrentFrame() const;

        /**
         * Gets the index of the current frame of the animation.
         *
         * @returns The index of the current frame of the animation.
         */
        size_t getCurrentFrameIndex() const;

        /**
         * Get the duration of the animation in milliseconds.
         *
         * @returns The duration of the animation in milliseconds.
         */
        float getDuration() const;

        /**
         * Set the duration of the animation in milliseconds.
         *
         * @param duration The duration of the animation in milliseconds.
         */
        void setDuration(float duration);

        /**
         * Get the duration of a single frame of the animation in milliseconds.
         *
         * @returns The duration of a single frame of the animation in milliseconds.
         */
        float getFrameDuration() const;

        /**
         * Get the number of frames in the animation.
         *
         * @returns The number of frames in the animation.
         */
        size_t getFrameCount() const;

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
        AnimationMode getMode() const;

        /**
         * Get the current time in milliseconds of the animation.
         *
         * @returns The current time in milliseconds of the animation.
         */
        float getTime() const;

        /**
         * Set the current time in millseconds of the animation.
         *
         * @param time The current time in milliseconds of the animation.
         */
        void setTime(float time);

        /**
         * Steps the animation forward in time by dt.
         *
         * @param dt The amount of time in milliseconds to step the animation forward by.
         */
        void step(float dt);

        /**
         * Pauses the animation.
         *
         * This will only have an effect if the animation is not paused.
         *
         * If the animation is paused, it will not step forward in time, even if step is called.
         */
        void pause();

        /**
         * Resumes the animation.
         *
         * This will only have an effect if the animation is paused.
         */
        void play();

        /**
         * Gets whether or not any frames in the animation are transparent.
         *
         * @returns Whether or not any frames in the animation are transparent.
         */
        bool isTransparent() const;

    private:
        /**
         * The frames of the animation.
         */
        std::vector<Texture *> frames;

        /**
         * Destroy the frames of the animation.
         */
        void destroyFrames();

        /**
         * The duration of the animation in milliseconds.
         */
        float duration;

        /**
         * The mode of the animation.
         */
        AnimationMode mode;

        /**
         * The current time in milliseconds of the animation.
         */
        float time = 0;

        /**
         * Whether or not the animation is paused.
         *
         * If the animation is paused, it will not step forward in time, even if step is called.
         */
        bool paused = false;

        /**
         * Whether or not the animation is reversing during ping ponging.
         */
        bool reversing = false;

        /**
         * Whether or not any of the frames of the animation are transparent.
         */
        bool transparent = false;

        /**
         * Updates whether or not any of the frames of the animation are transparent.
         */
        void updateTransparent();
    };
}