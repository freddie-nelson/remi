#pragma once

namespace remi
{
    /**
     * Checks if SDL is initialized.
     *
     * @returns True if SDL is initialized, false otherwise.
     */
    bool isSDLInitialized();

    /**
     * Checks if SDL_Mixer is initialized.
     *
     * @returns True if SDL_Mixer is initialized, false otherwise.
     */
    bool isSDLMixInitialized();

    /**
     * Initializes SDL and SDL_Mixer.
     *
     * @note Calls `initSDLMix` internally.
     */
    void initSDL();

    /**
     * Quits SDL and SDL_Mixer.
     *
     * @note Calls `quitSDLMix` internally.
     */
    void quitSDL();

    /**
     * Initializes SDL_mixer.
     */
    void initSDLMix();

    /**
     * Quits SDL_mixer.
     */
    void quitSDLMix();
}