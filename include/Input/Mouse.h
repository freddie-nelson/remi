#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/vec2.hpp>
#include <vector>

namespace Input
{
    /**
     * Represents a mouse button.
     */
    enum MouseButton
    {
        LEFT,
        RIGHT,
        MIDDLE,
        BACK,
        FORWARD
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
    class Mouse
    {
    public:
        /**
         * Creates new mouse object.
         *
         * @param window The window to get the mouse state from.
         */
        Mouse(GLFWwindow *window);

        /**
         * Destroys the mouse object.
         */
        ~Mouse();

        /**
         * Get the mouse position relative to the top left corner of the window.
         *
         * On supported platforms, this also contains the subpixel offset of the mouse.
         *
         * @return The mouse position in pixels and (potentially) subpixels.
         */
        glm::vec2 getPosition();

        /**
         * Check if a mouse button is pressed.
         *
         * @param button The mouse button to check.
         *
         * @return True if the mouse button is pressed, false otherwise.
         */
        bool isPressed(MouseButton button);

        /**
         * Sets the mouse mode.
         *
         * @param mode The mouse mode to set.
         *
         * @see Input::MouseMode
         */
        void setMode(MouseMode mode);

        /**
         * Enables raw mouse motion if it is available.
         */
        void enableRawMotion();

        /**
         * Disables raw mouse motion if it is available.
         */
        void disableRawMotion();

    private:
        /**
         * The GLFW window to get the mouse state from.
         */
        GLFWwindow *window;

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
        bool buttons[5];

        static std::vector<Mouse *> instances;

        static void mousePositionCallback(GLFWwindow *window, double x, double y);

        static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);

        static void mouseScrollCallback(GLFWwindow *window, double x, double y);
    };
}