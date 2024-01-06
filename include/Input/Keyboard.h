#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <vector>

namespace Input
{
    /**
     * Represents a key on the keyboard.
     */
    enum Key
    {
        // Printable keys
        BLZ_SPACE = GLFW_KEY_SPACE,
        BLZ_APOSTROPHE = GLFW_KEY_APOSTROPHE,
        BLZ_COMMA = GLFW_KEY_COMMA,
        BLZ_MINUS = GLFW_KEY_MINUS,
        BLZ_PERIOD = GLFW_KEY_PERIOD,
        BLZ_SLASH = GLFW_KEY_SLASH,
        BLZ_0 = GLFW_KEY_0,
        BLZ_1 = GLFW_KEY_1,
        BLZ_2 = GLFW_KEY_2,
        BLZ_3 = GLFW_KEY_3,
        BLZ_4 = GLFW_KEY_4,
        BLZ_5 = GLFW_KEY_5,
        BLZ_6 = GLFW_KEY_6,
        BLZ_7 = GLFW_KEY_7,
        BLZ_8 = GLFW_KEY_8,
        BLZ_9 = GLFW_KEY_9,
        BLZ_SEMICOLON = GLFW_KEY_SEMICOLON,
        BLZ_EQUAL = GLFW_KEY_EQUAL,
        BLZ_A = GLFW_KEY_A,
        BLZ_B = GLFW_KEY_B,
        BLZ_C = GLFW_KEY_C,
        BLZ_D = GLFW_KEY_D,
        BLZ_E = GLFW_KEY_E,
        BLZ_F = GLFW_KEY_F,
        BLZ_G = GLFW_KEY_G,
        BLZ_H = GLFW_KEY_H,
        BLZ_I = GLFW_KEY_I,
        BLZ_J = GLFW_KEY_J,
        BLZ_K = GLFW_KEY_K,
        BLZ_L = GLFW_KEY_L,
        BLZ_M = GLFW_KEY_M,
        BLZ_N = GLFW_KEY_N,
        BLZ_O = GLFW_KEY_O,
        BLZ_P = GLFW_KEY_P,
        BLZ_Q = GLFW_KEY_Q,
        BLZ_R = GLFW_KEY_R,
        BLZ_S = GLFW_KEY_S,
        BLZ_T = GLFW_KEY_T,
        BLZ_U = GLFW_KEY_U,
        BLZ_V = GLFW_KEY_V,
        BLZ_W = GLFW_KEY_W,
        BLZ_X = GLFW_KEY_X,
        BLZ_Y = GLFW_KEY_Y,
        BLZ_Z = GLFW_KEY_Z,
        BLZ_LEFT_BRACKET = GLFW_KEY_LEFT_BRACKET,
        BLZ_BACKSLASH = GLFW_KEY_BACKSLASH,
        BLZ_RIGHT_BRACKET = GLFW_KEY_RIGHT_BRACKET,
        BLZ_GRAVE_ACCENT = GLFW_KEY_GRAVE_ACCENT,
        BLZ_WORLD_1 = GLFW_KEY_WORLD_1,
        BLZ_WORLD_2 = GLFW_KEY_WORLD_2,

        // Function keys
        BLZ_ESCAPE = GLFW_KEY_ESCAPE,
        BLZ_ENTER = GLFW_KEY_ENTER,
        BLZ_TAB = GLFW_KEY_TAB,
        BLZ_BACKSPACE = GLFW_KEY_BACKSPACE,
        BLZ_INSERT = GLFW_KEY_INSERT,
        BLZ_DELETE = GLFW_KEY_DELETE,
        BLZ_RIGHT = GLFW_KEY_RIGHT,
        BLZ_LEFT = GLFW_KEY_LEFT,
        BLZ_DOWN = GLFW_KEY_DOWN,
        BLZ_UP = GLFW_KEY_UP,
        BLZ_PAGE_UP = GLFW_KEY_PAGE_UP,
        BLZ_PAGE_DOWN = GLFW_KEY_PAGE_DOWN,
        BLZ_HOME = GLFW_KEY_HOME,
        BLZ_END = GLFW_KEY_END,
        BLZ_CAPS_LOCK = GLFW_KEY_CAPS_LOCK,
        BLZ_SCROLL_LOCK = GLFW_KEY_SCROLL_LOCK,
        BLZ_NUM_LOCK = GLFW_KEY_NUM_LOCK,
        BLZ_PRINT_SCREEN = GLFW_KEY_PRINT_SCREEN,
        BLZ_PAUSE = GLFW_KEY_PAUSE,
        BLZ_F1 = GLFW_KEY_F1,
        BLZ_F2 = GLFW_KEY_F2,
        BLZ_F3 = GLFW_KEY_F3,
        BLZ_F4 = GLFW_KEY_F4,
        BLZ_F5 = GLFW_KEY_F5,
        BLZ_F6 = GLFW_KEY_F6,
        BLZ_F7 = GLFW_KEY_F7,
        BLZ_F8 = GLFW_KEY_F8,
        BLZ_F9 = GLFW_KEY_F9,
        BLZ_F10 = GLFW_KEY_F10,
        BLZ_F11 = GLFW_KEY_F11,
        BLZ_F12 = GLFW_KEY_F12,
        BLZ_F13 = GLFW_KEY_F13,
        BLZ_F14 = GLFW_KEY_F14,
        BLZ_F15 = GLFW_KEY_F15,
        BLZ_F16 = GLFW_KEY_F16,
        BLZ_F17 = GLFW_KEY_F17,
        BLZ_F18 = GLFW_KEY_F18,
        BLZ_F19 = GLFW_KEY_F19,
        BLZ_F20 = GLFW_KEY_F20,
        BLZ_F21 = GLFW_KEY_F21,
        BLZ_F22 = GLFW_KEY_F22,
        BLZ_F23 = GLFW_KEY_F23,
        BLZ_F24 = GLFW_KEY_F24,
        BLZ_F25 = GLFW_KEY_F25,
        BLZ_KP_0 = GLFW_KEY_KP_0,
        BLZ_KP_1 = GLFW_KEY_KP_1,
        BLZ_KP_2 = GLFW_KEY_KP_2,
        BLZ_KP_3 = GLFW_KEY_KP_3,
        BLZ_KP_4 = GLFW_KEY_KP_4,
        BLZ_KP_5 = GLFW_KEY_KP_5,
        BLZ_KP_6 = GLFW_KEY_KP_6,
        BLZ_KP_7 = GLFW_KEY_KP_7,
        BLZ_KP_8 = GLFW_KEY_KP_8,
        BLZ_KP_9 = GLFW_KEY_KP_9,
        BLZ_KP_DECIMAL = GLFW_KEY_KP_DECIMAL,
        BLZ_KP_DIVIDE = GLFW_KEY_KP_DIVIDE,
        BLZ_KP_MULTIPLY = GLFW_KEY_KP_MULTIPLY,
        BLZ_KP_SUBTRACT = GLFW_KEY_KP_SUBTRACT,
        BLZ_KP_ADD = GLFW_KEY_KP_ADD,
        BLZ_KP_ENTER = GLFW_KEY_KP_ENTER,
        BLZ_KP_EQUAL = GLFW_KEY_KP_EQUAL,
        BLZ_LEFT_SHIFT = GLFW_KEY_LEFT_SHIFT,
        BLZ_LEFT_CONTROL = GLFW_KEY_LEFT_CONTROL,
        BLZ_LEFT_ALT = GLFW_KEY_LEFT_ALT,
        BLZ_LEFT_SUPER = GLFW_KEY_LEFT_SUPER,
        BLZ_RIGHT_SHIFT = GLFW_KEY_RIGHT_SHIFT,
        BLZ_RIGHT_CONTROL = GLFW_KEY_RIGHT_CONTROL,
        BLZ_RIGHT_ALT = GLFW_KEY_RIGHT_ALT,
        BLZ_RIGHT_SUPER = GLFW_KEY_RIGHT_SUPER,
        BLZ_MENU = GLFW_KEY_MENU,

        __BLZ_LAST__ = Key::BLZ_MENU
    };

    /**
     * Represents a modifier key.
     */
    enum Modifier
    {
        BLZ_MOD_SHIFT = GLFW_MOD_SHIFT,
        BLZ_MOD_CONTROL = GLFW_MOD_CONTROL,
        BLZ_MOD_ALT = GLFW_MOD_ALT,
        BLZ_MOD_SUPER = GLFW_MOD_SUPER,
        BLZ_MOD_CAPS_LOCK = GLFW_MOD_CAPS_LOCK,
        BLZ_MOD_NUM_LOCK = GLFW_MOD_NUM_LOCK
    };

    /**
     * Represents the modifiers for a key press.
     */
    struct KeyModifier
    {
        /**
         * Creates a new key modifier object.
         */
        KeyModifier(int mods);

        /**
         * The modifiers for the key press directly from GLFW.
         */
        int mods;

        /**
         * Checks if the shift key was pressed.
         *
         * @returns True if the shift key was pressed, false otherwise.
         */
        bool shift();

        /**
         * Checks if the control key was pressed.
         *
         * @returns True if the control key was pressed, false otherwise.
         */
        bool control();

        /**
         * Checks if the alt key was pressed.
         *
         * @returns True if the alt key was pressed, false otherwise.
         */
        bool alt();

        /**
         * Checks if the super key was pressed.
         *
         * This is the windows key on windows, the command key on macos and the super key on linux.
         *
         * @returns True if the super key was pressed, false otherwise.
         */
        bool super();

        /**
         * Checks if the caps lock key was pressed.
         *
         * @returns True if the caps lock key was pressed, false otherwise.
         */
        bool capsLock();

        /**
         * Checks if the num lock key was pressed.
         *
         * @returns True if the num lock key was pressed, false otherwise.
         */
        bool numLock();
    };

    /**
     * Represents the keyboard.
     */
    class Keyboard
    {
    public:
        /**
         * Creates a new keyboard object.
         *
         * @param window The window to get the keyboard state from.
         */
        Keyboard(GLFWwindow *window);

        /**
         * Destroys the keyboard object.
         */
        ~Keyboard();

        /**
         * Checks if the given key is pressed.
         *
         * @param key The key to check.
         *
         * @returns True if the key is pressed, false otherwise.
         */
        bool isPressed(Key key);

        /**
         * Gets the modifiers for the given key.
         *
         * If the key is not pressed, the modifiers will be 0.
         *
         * @param key The key to get the modifiers for.
         *
         * @returns The modifiers for the given key.
         */
        KeyModifier getModifiers(Key key);

    private:
        /**
         * The window to get the keyboard state from.
         */
        GLFWwindow *window;

        /**
         * The state of each key.
         *
         * The key code is used as the index.
         *
         * True if the key is pressed, false otherwise.
         */
        bool keys[Key::__BLZ_LAST__ + 1];

        /**
         * The modifiers for each key press.
         *
         * The key code is used as the index.
         *
         * The value is a bitfield of modifier flags from glfw.
         */
        int mods[Key::__BLZ_LAST__ + 1];

        /**
         * The list of keyboard instances created.
         *
         * Used in callbacks to update each instances state.
         */
        static std::vector<Keyboard *> instances;

        static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    };
}