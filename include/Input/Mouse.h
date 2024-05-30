#pragma once

#include "../Core/Window.h"
#include "../Core/Observer.h"

#include <glm/vec2.hpp>
#include <vector>

namespace Input
{
    /**
     * Represents a mouse button.
     */
    enum MouseButton
    {
        LEFT = SDL_BUTTON_LEFT,
        MIDDLE = SDL_BUTTON_MIDDLE,
        RIGHT = SDL_BUTTON_RIGHT,
        BACK = SDL_BUTTON_X1,
        FORWARD = SDL_BUTTON_X2,

        __LAST_MOUSE_BUTTON__ = 255,
    };

    /**
     * Represents the mouse mode.
     *
     * The modes are:
     * - NORMAL: The mouse is visible and behaves normally.
     * - HIDDEN: The mouse is invisible, but behaves normally.
     * - DISABLED: The mouse is invisible, and the cursor is locked to the window, this provides essentially virtual and unlimited mouse movement.
     */
    enum MouseMode
    {
        NORMAL,
        HIDDEN,
        DISABLED
    };

    /**
     * Represents the mouse.
     */
    class Mouse : public Core::Observer<const std::vector<SDL_Event> &>
    {
    public:
        /**
         * Creates new mouse object.
         *
         * @param window The window to get the mouse state from.
         */
        Mouse(Core::Window &window);

        /**
         * Destroys the mouse object.
         */
        ~Mouse();

        /**
         * Get the mouse position relative to the top left corner of the window.
         *
         * On supported platforms, this also contains the subpixel offset of the mouse.
         *
         * @param flipY If true, the Y coordinate will be flipped so that the origin is at the bottom left corner of the window. This is useful as it will be in screen space.
         *
         * @return The mouse position in pixels and (potentially) subpixels.
         */
        glm::vec2 getPosition(bool flipY = false) const;

        /**
         * Get the mouse scroll offset.
         *
         * @return The mouse scroll offset in pixels.
         */
        const glm::vec2 &getScroll() const;

        /**
         * Check if a mouse button is pressed.
         *
         * @param button The mouse button to check.
         *
         * @return True if the mouse button is pressed, false otherwise.
         */
        bool isPressed(MouseButton button) const;

        /**
         * Sets the mouse mode.
         *
         * @param mode The mouse mode to set.
         *
         * @see Input::MouseMode
         */
        void setMode(MouseMode mode);

        /**
         * Get the current mouse mode.
         *
         * @return The current mouse mode.
         */
        MouseMode getMode() const;

        /**
         * Update the mouse with new events.
         *
         * This will be attached to the "poll" event of the window.
         *
         * @param event The event that occurred.
         * @param data The list of events from the window.
         */
        void updateObserver(std::string event, const std::vector<SDL_Event> &data) override;

    private:
        /**
         * The window to get the mouse state from.
         */
        Core::Window &window;

        /**
         * The mouse position relative to the window.
         *
         * This is in pixels.
         *
         * On supported platforms, this also contains the subpixel offset of the mouse.
         */
        glm::vec2 position = glm::vec2(0.0f);

        /**
         * The mouse scroll offset.
         *
         * This is in pixels.
         */
        glm::vec2 scroll = glm::vec2(0.0f);

        /**
         * The mouse button states.
         *
         * The index of the array is the button, and the value is the state.
         *
         * This maps directly to the MouseButton enum.
         */
        bool buttons[MouseButton::__LAST_MOUSE_BUTTON__];
    };
}