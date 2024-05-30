#pragma once

#include "../Core/Window.h"
#include "../Core/Observer.h"

#include <vector>

namespace Input
{
    /**
     * Represents a physical key on the keyboard.
     *
     * @note This directly maps to the SDL2 scan codes.
     */
    enum Key
    {
        KEY_UNKNOWN = SDL_SCANCODE_UNKNOWN,

        /**
         *  \name Usage page 0x07
         *
         *  These values are from usage page 0x07 (USB keyboard page).
         */
        /* @{ */

        KEY_A = SDL_SCANCODE_A,
        KEY_B = SDL_SCANCODE_B,
        KEY_C = SDL_SCANCODE_C,
        KEY_D = SDL_SCANCODE_D,
        KEY_E = SDL_SCANCODE_E,
        KEY_F = SDL_SCANCODE_F,
        KEY_G = SDL_SCANCODE_G,
        KEY_H = SDL_SCANCODE_H,
        KEY_I = SDL_SCANCODE_I,
        KEY_J = SDL_SCANCODE_J,
        KEY_K = SDL_SCANCODE_K,
        KEY_L = SDL_SCANCODE_L,
        KEY_M = SDL_SCANCODE_M,
        KEY_N = SDL_SCANCODE_N,
        KEY_O = SDL_SCANCODE_O,
        KEY_P = SDL_SCANCODE_P,
        KEY_Q = SDL_SCANCODE_Q,
        KEY_R = SDL_SCANCODE_R,
        KEY_S = SDL_SCANCODE_S,
        KEY_T = SDL_SCANCODE_T,
        KEY_U = SDL_SCANCODE_U,
        KEY_V = SDL_SCANCODE_V,
        KEY_W = SDL_SCANCODE_W,
        KEY_X = SDL_SCANCODE_X,
        KEY_Y = SDL_SCANCODE_Y,
        KEY_Z = SDL_SCANCODE_Z,

        KEY_1 = SDL_SCANCODE_1,
        KEY_2 = SDL_SCANCODE_2,
        KEY_3 = SDL_SCANCODE_3,
        KEY_4 = SDL_SCANCODE_4,
        KEY_5 = SDL_SCANCODE_5,
        KEY_6 = SDL_SCANCODE_6,
        KEY_7 = SDL_SCANCODE_7,
        KEY_8 = SDL_SCANCODE_8,
        KEY_9 = SDL_SCANCODE_9,
        KEY_0 = SDL_SCANCODE_0,

        KEY_RETURN = SDL_SCANCODE_RETURN,
        KEY_ESCAPE = SDL_SCANCODE_ESCAPE,
        KEY_BACKSPACE = SDL_SCANCODE_BACKSPACE,
        KEY_TAB = SDL_SCANCODE_TAB,
        KEY_SPACE = SDL_SCANCODE_SPACE,

        KEY_MINUS = SDL_SCANCODE_MINUS,
        KEY_EQUALS = SDL_SCANCODE_EQUALS,
        KEY_LEFTBRACKET = SDL_SCANCODE_LEFTBRACKET,
        KEY_RIGHTBRACKET = SDL_SCANCODE_RIGHTBRACKET,
        KEY_BACKSLASH = SDL_SCANCODE_BACKSLASH, /**< Located at the lower left of the return
                                                 *   key on ISO keyboards and at the right end
                                                 *   of the QWERTY row on ANSI keyboards.
                                                 *   Produces REVERSE SOLIDUS (backslash) and
                                                 *   VERTICAL LINE in a US layout, REVERSE
                                                 *   SOLIDUS and VERTICAL LINE in a UK Mac
                                                 *   layout, NUMBER SIGN and TILDE in a UK
                                                 *   Windows layout, DOLLAR SIGN and POUND SIGN
                                                 *   in a Swiss German layout, NUMBER SIGN and
                                                 *   APOSTROPHE in a German layout, GRAVE
                                                 *   ACCENT and POUND SIGN in a French Mac
                                                 *   layout, and ASTERISK and MICRO SIGN in a
                                                 *   French Windows layout.
                                                 */
        KEY_NONUSHASH = SDL_SCANCODE_NONUSHASH, /**< ISO USB keyboards actually use this code
                                                 *   instead of 49 for the same key, but all
                                                 *   OSes I've seen treat the two codes
                                                 *   identically. So, as an implementor, unless
                                                 *   your keyboard generates both of those
                                                 *   codes and your OS treats them differently,
                                                 *   you should generate SDL_SCANCODE_BACKSLASH
                                                 *   instead of this code. As a user, you
                                                 *   should not rely on this code because SDL
                                                 *   will never generate it with most (all?)
                                                 *   keyboards.
                                                 */
        KEY_SEMICOLON = SDL_SCANCODE_SEMICOLON,
        KEY_APOSTROPHE = SDL_SCANCODE_APOSTROPHE,
        KEY_GRAVE = SDL_SCANCODE_GRAVE, /**< Located in the top left corner (on both ANSI
                                         *   and ISO keyboards). Produces GRAVE ACCENT and
                                         *   TILDE in a US Windows layout and in US and UK
                                         *   Mac layouts on ANSI keyboards, GRAVE ACCENT
                                         *   and NOT SIGN in a UK Windows layout, SECTION
                                         *   SIGN and PLUS-MINUS SIGN in US and UK Mac
                                         *   layouts on ISO keyboards, SECTION SIGN and
                                         *   DEGREE SIGN in a Swiss German layout (Mac:
                                         *   only on ISO keyboards), CIRCUMFLEX ACCENT and
                                         *   DEGREE SIGN in a German layout (Mac: only on
                                         *   ISO keyboards), SUPERSCRIPT TWO and TILDE in a
                                         *   French Windows layout, COMMERCIAL AT and
                                         *   NUMBER SIGN in a French Mac layout on ISO
                                         *   keyboards, and LESS-THAN SIGN and GREATER-THAN
                                         *   SIGN in a Swiss German, German, or French Mac
                                         *   layout on ANSI keyboards.
                                         */
        KEY_COMMA = SDL_SCANCODE_COMMA,
        KEY_PERIOD = SDL_SCANCODE_PERIOD,
        KEY_SLASH = SDL_SCANCODE_SLASH,

        KEY_CAPSLOCK = SDL_SCANCODE_CAPSLOCK,

        KEY_F1 = SDL_SCANCODE_F1,
        KEY_F2 = SDL_SCANCODE_F2,
        KEY_F3 = SDL_SCANCODE_F3,
        KEY_F4 = SDL_SCANCODE_F4,
        KEY_F5 = SDL_SCANCODE_F5,
        KEY_F6 = SDL_SCANCODE_F6,
        KEY_F7 = SDL_SCANCODE_F7,
        KEY_F8 = SDL_SCANCODE_F8,
        KEY_F9 = SDL_SCANCODE_F9,
        KEY_F10 = SDL_SCANCODE_F10,
        KEY_F11 = SDL_SCANCODE_F11,
        KEY_F12 = SDL_SCANCODE_F12,

        KEY_PRINTSCREEN = SDL_SCANCODE_PRINTSCREEN,
        KEY_SCROLLLOCK = SDL_SCANCODE_SCROLLLOCK,
        KEY_PAUSE = SDL_SCANCODE_PAUSE,
        KEY_INSERT = SDL_SCANCODE_INSERT, /* help on some Mac keyboards(but
                                       does send code 73,
                                   not 117) */
        KEY_HOME = SDL_SCANCODE_HOME,
        KEY_PAGEUP = SDL_SCANCODE_PAGEUP,
        KEY_DELETE = SDL_SCANCODE_DELETE,
        KEY_END = SDL_SCANCODE_END,
        KEY_PAGEDOWN = SDL_SCANCODE_PAGEDOWN,
        KEY_RIGHT = SDL_SCANCODE_RIGHT,
        KEY_LEFT = SDL_SCANCODE_LEFT,
        KEY_DOWN = SDL_SCANCODE_DOWN,
        KEY_UP = SDL_SCANCODE_UP,

        KEY_NUMLOCKCLEAR = SDL_SCANCODE_NUMLOCKCLEAR, /* clear on Mac keyboards
                                                       */
        KEY_KP_DIVIDE = SDL_SCANCODE_KP_DIVIDE,
        KEY_KP_MULTIPLY = SDL_SCANCODE_KP_MULTIPLY,
        KEY_KP_MINUS = SDL_SCANCODE_KP_MINUS,
        KEY_KP_PLUS = SDL_SCANCODE_KP_PLUS,
        KEY_KP_ENTER = SDL_SCANCODE_KP_ENTER,
        KEY_KP_1 = SDL_SCANCODE_KP_1,
        KEY_KP_2 = SDL_SCANCODE_KP_2,
        KEY_KP_3 = SDL_SCANCODE_KP_3,
        KEY_KP_4 = SDL_SCANCODE_KP_4,
        KEY_KP_5 = SDL_SCANCODE_KP_5,
        KEY_KP_6 = SDL_SCANCODE_KP_6,
        KEY_KP_7 = SDL_SCANCODE_KP_7,
        KEY_KP_8 = SDL_SCANCODE_KP_8,
        KEY_KP_9 = SDL_SCANCODE_KP_9,
        KEY_KP_0 = SDL_SCANCODE_KP_0,
        KEY_KP_PERIOD = SDL_SCANCODE_KP_PERIOD,

        KEY_NONUSBACKSLASH = SDL_SCANCODE_NONUSBACKSLASH, /**< This is the additional key that ISO
                                                           *   keyboards have over ANSI ones,
                                                           *   located between left shift and Y.
                                                           *   Produces GRAVE ACCENT and TILDE in a
                                                           *   US or UK Mac layout, REVERSE SOLIDUS
                                                           *   (backslash) and VERTICAL LINE in a
                                                           *   US or UK Windows layout, and
                                                           *   LESS-THAN SIGN and GREATER-THAN SIGN
                                                           *   in a Swiss German, German, or French
                                                           *   layout. */
        KEY_APPLICATION = SDL_SCANCODE_APPLICATION,       /* compose */
        KEY_POWER = SDL_SCANCODE_POWER,                   /*not a physical key - but some Mac keyboards
                                                           *do have a power key. */
        KEY_KP_EQUALS = SDL_SCANCODE_KP_EQUALS,
        KEY_F13 = SDL_SCANCODE_F13,
        KEY_F14 = SDL_SCANCODE_F14,
        KEY_F15 = SDL_SCANCODE_F15,
        KEY_F16 = SDL_SCANCODE_F16,
        KEY_F17 = SDL_SCANCODE_F17,
        KEY_F18 = SDL_SCANCODE_F18,
        KEY_F19 = SDL_SCANCODE_F19,
        KEY_F20 = SDL_SCANCODE_F20,
        KEY_F21 = SDL_SCANCODE_F21,
        KEY_F22 = SDL_SCANCODE_F22,
        KEY_F23 = SDL_SCANCODE_F23,
        KEY_F24 = SDL_SCANCODE_F24,
        KEY_EXECUTE = SDL_SCANCODE_EXECUTE,
        KEY_HELP = SDL_SCANCODE_HELP, /**< AL Integrated Help Center */
        KEY_MENU = SDL_SCANCODE_MENU, /**< Menu (show menu) */
        KEY_SELECT = SDL_SCANCODE_SELECT,
        KEY_STOP = SDL_SCANCODE_STOP,   /**< AC Stop */
        KEY_AGAIN = SDL_SCANCODE_AGAIN, /**< AC Redo/Repeat */
        KEY_UNDO = SDL_SCANCODE_UNDO,   /**< AC Undo */
        KEY_CUT = SDL_SCANCODE_CUT,     /**< AC Cut */
        KEY_COPY = SDL_SCANCODE_COPY,   /**< AC Copy */
        KEY_PASTE = SDL_SCANCODE_PASTE, /**< AC Paste */
        KEY_FIND = SDL_SCANCODE_FIND,   /**< AC Find */
        KEY_MUTE = SDL_SCANCODE_MUTE,
        KEY_VOLUMEUP = SDL_SCANCODE_VOLUMEUP,
        KEY_VOLUMEDOWN = SDL_SCANCODE_VOLUMEDOWN,
        /* not sure whether there's a reason to enable these */
        /*     KEY_LOCKINGCAPSLOCK = SDL_SCANCODE_LOCKINGCAPSLOCK,  */
        /*     KEY_LOCKINGNUMLOCK = SDL_SCANCODE_LOCKINGNUMLOCK, */
        /*     KEY_LOCKINGSCROLLLOCK = SDL_SCANCODE_LOCKINGSCROLLLOCK, */
        KEY_KP_COMMA = SDL_SCANCODE_KP_COMMA,
        KEY_KP_EQUALSAS400 = SDL_SCANCODE_KP_EQUALSAS400,

        KEY_INTERNATIONAL1 = SDL_SCANCODE_INTERNATIONAL1, /* see footnotes in USB doc */
        KEY_INTERNATIONAL2 = SDL_SCANCODE_INTERNATIONAL2,
        KEY_INTERNATIONAL3 = SDL_SCANCODE_INTERNATIONAL3, /**< Yen */
        KEY_INTERNATIONAL4 = SDL_SCANCODE_INTERNATIONAL4,
        KEY_INTERNATIONAL5 = SDL_SCANCODE_INTERNATIONAL5,
        KEY_INTERNATIONAL6 = SDL_SCANCODE_INTERNATIONAL6,
        KEY_INTERNATIONAL7 = SDL_SCANCODE_INTERNATIONAL7,
        KEY_INTERNATIONAL8 = SDL_SCANCODE_INTERNATIONAL8,
        KEY_INTERNATIONAL9 = SDL_SCANCODE_INTERNATIONAL9,
        KEY_LANG1 = SDL_SCANCODE_LANG1, /**< Hangul/English toggle */
        KEY_LANG2 = SDL_SCANCODE_LANG2, /**< Hanja conversion */
        KEY_LANG3 = SDL_SCANCODE_LANG3, /**< Katakana */
        KEY_LANG4 = SDL_SCANCODE_LANG4, /**< Hiragana */
        KEY_LANG5 = SDL_SCANCODE_LANG5, /**< Zenkaku/Hankaku */
        KEY_LANG6 = SDL_SCANCODE_LANG6, /**< reserved */
        KEY_LANG7 = SDL_SCANCODE_LANG7, /**< reserved */
        KEY_LANG8 = SDL_SCANCODE_LANG8, /**< reserved */
        KEY_LANG9 = SDL_SCANCODE_LANG9, /**< reserved */

        KEY_ALTERASE = SDL_SCANCODE_ALTERASE, /**< Erase-Eaze */
        KEY_SYSREQ = SDL_SCANCODE_SYSREQ,
        KEY_CANCEL = SDL_SCANCODE_CANCEL, /**< AC Cancel */
        KEY_CLEAR = SDL_SCANCODE_CLEAR,
        KEY_PRIOR = SDL_SCANCODE_PRIOR,
        KEY_RETURN2 = SDL_SCANCODE_RETURN2,
        KEY_SEPARATOR = SDL_SCANCODE_SEPARATOR,
        KEY_OUT = SDL_SCANCODE_OUT,
        KEY_OPER = SDL_SCANCODE_OPER,
        KEY_CLEARAGAIN = SDL_SCANCODE_CLEARAGAIN,
        KEY_CRSEL = SDL_SCANCODE_CRSEL,
        KEY_EXSEL = SDL_SCANCODE_EXSEL,

        KEY_KP_00 = SDL_SCANCODE_KP_00,
        KEY_KP_000 = SDL_SCANCODE_KP_000,
        KEY_THOUSANDSSEPARATOR = SDL_SCANCODE_THOUSANDSSEPARATOR,
        KEY_DECIMALSEPARATOR = SDL_SCANCODE_DECIMALSEPARATOR,
        KEY_CURRENCYUNIT = SDL_SCANCODE_CURRENCYUNIT,
        KEY_CURRENCYSUBUNIT = SDL_SCANCODE_CURRENCYSUBUNIT,
        KEY_KP_LEFTPAREN = SDL_SCANCODE_KP_LEFTPAREN,
        KEY_KP_RIGHTPAREN = SDL_SCANCODE_KP_RIGHTPAREN,
        KEY_KP_LEFTBRACE = SDL_SCANCODE_KP_LEFTBRACE,
        KEY_KP_RIGHTBRACE = SDL_SCANCODE_KP_RIGHTBRACE,
        KEY_KP_TAB = SDL_SCANCODE_KP_TAB,
        KEY_KP_BACKSPACE = SDL_SCANCODE_KP_BACKSPACE,
        KEY_KP_A = SDL_SCANCODE_KP_A,
        KEY_KP_B = SDL_SCANCODE_KP_B,
        KEY_KP_C = SDL_SCANCODE_KP_C,
        KEY_KP_D = SDL_SCANCODE_KP_D,
        KEY_KP_E = SDL_SCANCODE_KP_E,
        KEY_KP_F = SDL_SCANCODE_KP_F,
        KEY_KP_XOR = SDL_SCANCODE_KP_XOR,
        KEY_KP_POWER = SDL_SCANCODE_KP_POWER,
        KEY_KP_PERCENT = SDL_SCANCODE_KP_PERCENT,
        KEY_KP_LESS = SDL_SCANCODE_KP_LESS,
        KEY_KP_GREATER = SDL_SCANCODE_KP_GREATER,
        KEY_KP_AMPERSAND = SDL_SCANCODE_KP_AMPERSAND,
        KEY_KP_DBLAMPERSAND = SDL_SCANCODE_KP_DBLAMPERSAND,
        KEY_KP_VERTICALBAR = SDL_SCANCODE_KP_VERTICALBAR,
        KEY_KP_DBLVERTICALBAR = SDL_SCANCODE_KP_DBLVERTICALBAR,
        KEY_KP_COLON = SDL_SCANCODE_KP_COLON,
        KEY_KP_HASH = SDL_SCANCODE_KP_HASH,
        KEY_KP_SPACE = SDL_SCANCODE_KP_SPACE,
        KEY_KP_AT = SDL_SCANCODE_KP_AT,
        KEY_KP_EXCLAM = SDL_SCANCODE_KP_EXCLAM,
        KEY_KP_MEMSTORE = SDL_SCANCODE_KP_MEMSTORE,
        KEY_KP_MEMRECALL = SDL_SCANCODE_KP_MEMRECALL,
        KEY_KP_MEMCLEAR = SDL_SCANCODE_KP_MEMCLEAR,
        KEY_KP_MEMADD = SDL_SCANCODE_KP_MEMADD,
        KEY_KP_MEMSUBTRACT = SDL_SCANCODE_KP_MEMSUBTRACT,
        KEY_KP_MEMMULTIPLY = SDL_SCANCODE_KP_MEMMULTIPLY,
        KEY_KP_MEMDIVIDE = SDL_SCANCODE_KP_MEMDIVIDE,
        KEY_KP_PLUSMINUS = SDL_SCANCODE_KP_PLUSMINUS,
        KEY_KP_CLEAR = SDL_SCANCODE_KP_CLEAR,
        KEY_KP_CLEARENTRY = SDL_SCANCODE_KP_CLEARENTRY,
        KEY_KP_BINARY = SDL_SCANCODE_KP_BINARY,
        KEY_KP_OCTAL = SDL_SCANCODE_KP_OCTAL,
        KEY_KP_DECIMAL = SDL_SCANCODE_KP_DECIMAL,
        KEY_KP_HEXADECIMAL = SDL_SCANCODE_KP_HEXADECIMAL,

        KEY_LCTRL = SDL_SCANCODE_LCTRL,
        KEY_LSHIFT = SDL_SCANCODE_LSHIFT,
        KEY_LALT = SDL_SCANCODE_LALT, /* option */
        KEY_LGUI = SDL_SCANCODE_LGUI, /* meta */
        KEY_RCTRL = SDL_SCANCODE_RCTRL,
        KEY_RSHIFT = SDL_SCANCODE_RSHIFT,
        KEY_RALT = SDL_SCANCODE_RALT, /* option */
        KEY_RGUI = SDL_SCANCODE_RGUI, /* meta */

        KEY_MODE = SDL_SCANCODE_MODE, /**< I'm not sure if this is really not covered
                                       *   by any of the above, but since there's a
                                       *   special KMOD_MODE for it I'm adding it here
                                       */

        /* @} */ /* Usage page 0x07 */

        /**
         *  \name Usage page 0x0C
         *
         *  These values are mapped from usage page 0x0C (USB consumer page).
         *  See https://usb.org/sites/default/files/hut1_2.pdf
         *
         *  There are way more keys in the spec than we can represent in the
         *  current scancode range, so pick the ones that commonly come up in
         *  real world usage.
         */
        /* @{ */

        KEY_AUDIONEXT = SDL_SCANCODE_AUDIONEXT,
        KEY_AUDIOPREV = SDL_SCANCODE_AUDIOPREV,
        KEY_AUDIOSTOP = SDL_SCANCODE_AUDIOSTOP,
        KEY_AUDIOPLAY = SDL_SCANCODE_AUDIOPLAY,
        KEY_AUDIOMUTE = SDL_SCANCODE_AUDIOMUTE,
        KEY_MEDIASELECT = SDL_SCANCODE_MEDIASELECT,
        KEY_WWW = SDL_SCANCODE_WWW, /**< AL Internet Browser */
        KEY_MAIL = SDL_SCANCODE_MAIL,
        KEY_CALCULATOR = SDL_SCANCODE_CALCULATOR, /**< AL Calculator */
        KEY_COMPUTER = SDL_SCANCODE_COMPUTER,
        KEY_AC_SEARCH = SDL_SCANCODE_AC_SEARCH,       /**< AC Search */
        KEY_AC_HOME = SDL_SCANCODE_AC_HOME,           /**< AC Home */
        KEY_AC_BACK = SDL_SCANCODE_AC_BACK,           /**< AC Back */
        KEY_AC_FORWARD = SDL_SCANCODE_AC_FORWARD,     /**< AC Forward */
        KEY_AC_STOP = SDL_SCANCODE_AC_STOP,           /**< AC Stop */
        KEY_AC_REFRESH = SDL_SCANCODE_AC_REFRESH,     /**< AC Refresh */
        KEY_AC_BOOKMARKS = SDL_SCANCODE_AC_BOOKMARKS, /**< AC Bookmarks */

        /* @} */ /* Usage page 0x0C */

        /**
         *  \name Walther keys
         *
         *  These are values that Christian Walther added (for mac keyboard?).
         */
        /* @{ */

        KEY_BRIGHTNESSDOWN = SDL_SCANCODE_BRIGHTNESSDOWN,
        KEY_BRIGHTNESSUP = SDL_SCANCODE_BRIGHTNESSUP,
        KEY_DISPLAYSWITCH = SDL_SCANCODE_DISPLAYSWITCH, /**< display mirroring/dual display
                                               switch, video mode switch */
        KEY_KBDILLUMTOGGLE = SDL_SCANCODE_KBDILLUMTOGGLE,
        KEY_KBDILLUMDOWN = SDL_SCANCODE_KBDILLUMDOWN,
        KEY_KBDILLUMUP = SDL_SCANCODE_KBDILLUMUP,
        KEY_EJECT = SDL_SCANCODE_EJECT,
        KEY_SLEEP = SDL_SCANCODE_SLEEP, /**< SC System Sleep */

        KEY_APP1 = SDL_SCANCODE_APP1,
        KEY_APP2 = SDL_SCANCODE_APP2,

        /* @} */ /* Walther keys */

        /**
         *  \name Usage page 0x0C (additional media keys)
         *
         *  These values are mapped from usage page 0x0C (USB consumer page).
         */
        /* @{ */

        KEY_AUDIOREWIND = SDL_SCANCODE_AUDIOREWIND,
        KEY_AUDIOFASTFORWARD = SDL_SCANCODE_AUDIOFASTFORWARD,

        /* @} */ /* Usage page 0x0C (additional media keys) */

        /**
         *  \name Mobile keys
         *
         *  These are values that are often used on mobile phones.
         */
        /* @{ */

        KEY_SOFTLEFT = SDL_SCANCODE_SOFTLEFT,   /**< Usually situated below the display on phones and
                                            used as a multi-function feature key for selecting
                                            a software defined function shown on the bottom left
                                            of the display. */
        KEY_SOFTRIGHT = SDL_SCANCODE_SOFTRIGHT, /**< Usually situated below the display on phones and
                                           used as a multi-function feature key for selecting
                                           a software defined function shown on the bottom right
                                           of the display. */
        KEY_CALL = SDL_SCANCODE_CALL,           /**< Used for accepting phone calls. */
        KEY_ENDCALL = SDL_SCANCODE_ENDCALL,     /**< Used for rejecting phone calls. */

        /* @} */ /* Mobile keys */

        /* Add any other keys here. */

        __LAST_KEY__ = SDL_NUM_SCANCODES, /* just marks the number of scancodes
                                     for array bounds */
    };

    /**
     * Represents a modifier key.
     */
    enum Modifier
    {
        MOD_SHIFT = KMOD_SHIFT,
        MOD_CONTROL = KMOD_CTRL,
        MOD_ALT = KMOD_ALT,
        MOD_SUPER = KMOD_GUI,
        MOD_CAPS_LOCK = KMOD_CAPS,
        MOD_NUM_LOCK = KMOD_NUM
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
         * The modifiers for the key press directly from the window.
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
    class Keyboard : public Core::Observer<const std::vector<SDL_Event> &>
    {
    public:
        /**
         * Creates a new keyboard object.
         *
         * @param window The window to get the keyboard state from.
         */
        Keyboard(Core::Window &window);

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
        bool isPressed(Key key) const;

        /**
         * Gets the modifiers for the given key.
         *
         * If the key is not pressed, the modifiers will be 0.
         *
         * @param key The key to get the modifiers for.
         *
         * @returns The modifiers for the given key.
         */
        KeyModifier getModifiers(Key key) const;

        /**
         * Update the keyboard with new events.
         *
         * This will be attached to the "poll" event of the window.
         *
         * @param event The event that occurred.
         * @param data The list of events from the window.
         */
        void updateObserver(std::string event, const std::vector<SDL_Event> &data) override;

    private:
        /**
         * The window to get the keyboard state from.
         */
        Core::Window &window;

        /**
         * The state of each key.
         *
         * The key code is used as the index.
         *
         * True if the key is pressed, false otherwise.
         */
        bool keys[Key::__LAST_KEY__ + 1];

        /**
         * The modifiers for each key press.
         *
         * The key code is used as the index.
         *
         * The value is a bitfield of modifier flags from the window.
         */
        int mods[Key::__LAST_KEY__ + 1];
    };
}