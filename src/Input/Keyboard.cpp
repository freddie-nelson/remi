#include "../../include/Input/Keyboard.h"

#include <iostream>

Input::KeyModifier::KeyModifier(int mods)
    : mods(mods)
{
}

bool Input::KeyModifier::shift()
{
    return mods & Modifier::MOD_SHIFT;
}

bool Input::KeyModifier::control()
{
    return mods & Modifier::MOD_CONTROL;
}

bool Input::KeyModifier::alt()
{
    return mods & Modifier::MOD_ALT;
}

bool Input::KeyModifier::super()
{
    return mods & Modifier::MOD_SUPER;
}

bool Input::KeyModifier::capsLock()
{
    return mods & Modifier::MOD_CAPS_LOCK;
}

bool Input::KeyModifier::numLock()
{
    return mods & Modifier::MOD_NUM_LOCK;
}

Input::Keyboard::Keyboard(Core::Window &window)
    : window(window)
{
    window.attachObserver(Core::WindowPollEventName, this);

    // set initial values
    for (int i = 0; i < Key::__LAST_KEY__; i++)
    {
        this->keys[i] = false;
        this->mods[i] = 0;
    }
}

Input::Keyboard::~Keyboard()
{
    window.detachObserver(Core::WindowPollEventName, this);
}

bool Input::Keyboard::isPressed(Key key) const
{
    return keys[key];
}

Input::KeyModifier Input::Keyboard::getModifiers(Key key) const
{
    return KeyModifier(mods[key]);
}

void Input::Keyboard::updateObserver(std::string event, const std::vector<SDL_Event> &data)
{
    for (auto &event : data)
    {
        switch (event.type)
        {
        case SDL_KEYDOWN:
        {
            auto key = static_cast<Key>(event.key.keysym.sym);

            keys[key] = true;
            mods[key] = event.key.keysym.mod;

            break;
        }
        case SDL_KEYUP:
        {
            auto key = static_cast<Key>(event.key.keysym.sym);

            keys[key] = false;
            mods[key] = 0;

            break;
        }
        }
    }
}