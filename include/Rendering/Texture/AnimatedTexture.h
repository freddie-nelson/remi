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
         * Create an animated texture.
         *
         * @param frames The paths to the frames of the animation.
         */
        AnimatedTexture(const std::vector<std::string> &frames, bool flip = true);

        /**
         * Create an animated texture.
         *
         * @param frames The frames of the animation.
         */
        AnimatedTexture(const std::vector<Texture *> &frames);

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
         * Get the number of frames in the animation.
         *
         * @returns The number of frames in the animation.
         */
        size_t getFrameCount() const;

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
         * Whether or not any of the frames of the animation are transparent.
         */
        bool transparent = false;

        /**
         * Updates whether or not any of the frames of the animation are transparent.
         */
        void updateTransparent();
    };
}