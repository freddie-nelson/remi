#pragma once

#include "Material.h"
#include "../Texture/AnimatedTexture.h"
#include "../../Core/Timestep.h"

namespace Rendering
{
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
     * Represents an animated material.
     *
     * An animated material is a material that contains an animated texture.
     *
     * The animated texture will be stepped every frame.
     *
     * The `getTexture` method will return the current frame of the animation.
     */
    class AnimatedMaterial : public Material
    {
    public:
        /**
         * Constructs an animated material.
         *
         * @param color The color of the material.
         * @param texture The texture of the material.
         * @param duration The duration of the animation in seconds.
         * @param mode The mode of the animation.
         */
        AnimatedMaterial(Color color, AnimatedTexture *texture, float duration = 1.0f, AnimationMode mode = AnimationMode::LOOP);

        /**
         * Constructs an animated material.
         *
         * @param texture The texture of the material.
         * @param duration The duration of the animation in seconds.
         * @param mode The mode of the animation.
         */
        AnimatedMaterial(AnimatedTexture *texture, float duration = 1.0f, AnimationMode mode = AnimationMode::LOOP);

        /**
         * Destroys the material.
         */
        virtual ~AnimatedMaterial();

        /**
         * Steps the animation forward in time by dt.
         *
         * @param dt The amount of time in seconds to step the animation forward by.
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
         * Restarts the animation.
         *
         * This will unpause the animation if it is paused.
         */
        void restart();

        // we don't want to allow the user to set the texture of the animated material
        void setTexture(Texture *texture) = delete;

        /**
         * Gets the animated texture of the material.
         *
         * @returns The animated texture of the material.
         */
        void setAnimatedTexture(AnimatedTexture *texture);

        /**
         * Gets the animated texture of the material.
         *
         * @returns The animated texture of the material.
         */
        const AnimatedTexture *getAnimatedTexture() const;

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
         * Gets the duration of the animation in milliseconds.
         *
         * @returns The duration of the animation in milliseconds.
         */
        float getDuration() const;

        /**
         * Sets the duration of the animation in milliseconds.
         *
         * @param duration The duration of the animation in milliseconds.
         */
        void setDuration(float duration);

        /**
         * Gets the duration of a single frame of the animation in milliseconds.
         *
         * @returns The duration of a single frame of the animation in milliseconds.
         */
        float getFrameDuration() const;

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
         * Gets whether or not the animation is transparent.
         *
         * @returns Whether or not the animation is transparent.
         */
        bool isTransparent() const;

        /**
         * Gets whether or not the animation is paused.
         *
         * @returns Whether or not the animation is paused.
         */
        bool isPaused() const;

    private:
        /**
         * The animated texture of the material.
         */
        AnimatedTexture *animatedTexture;

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
    };
}