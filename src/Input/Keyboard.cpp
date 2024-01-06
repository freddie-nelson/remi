#include "../../include/Input/Keyboard.h"

#include <iostream>

Input::KeyModifier::KeyModifier(int mods)
    : mods(mods)
{
}

bool Input::KeyModifier::shift()
{
    return mods & GLFW_MOD_SHIFT;
}

bool Input::KeyModifier::control()
{
    return mods & GLFW_MOD_CONTROL;
}

bool Input::KeyModifier::alt()
{
    return mods & GLFW_MOD_ALT;
}

bool Input::KeyModifier::super()
{
    return mods & GLFW_MOD_SUPER;
}

bool Input::KeyModifier::capsLock()
{
    return mods & GLFW_MOD_CAPS_LOCK;
}

bool Input::KeyModifier::numLock()
{
    return mods & GLFW_MOD_NUM_LOCK;
}

Input::Keyboard::Keyboard(GLFWwindow *window)
    : window(window)
{
    // set callbacks
    if (instances.size() == 0)
    {
        glfwSetKeyCallback(window, Input::Keyboard::keyCallback);
    }

    Input::Keyboard::instances.push_back(this);
}

Input::Keyboard::~Keyboard()
{
    // remove this instance from the list of instances
    for (auto it = Input::Keyboard::instances.begin(); it != Input::Keyboard::instances.end(); it++)
    {
        if (*it == this)
        {
            Input::Keyboard::instances.erase(it);
            break;
        }
    }
}

bool Input::Keyboard::isPressed(Key key)
{
    return keys[key];
}

Input::KeyModifier Input::Keyboard::getModifiers(Key key)
{
    return KeyModifier(mods[key]);
}

std::vector<Input::Keyboard *> Input::Keyboard::instances;

void Input::Keyboard::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    // std::cout << "key: " << key << std::endl;

    for (auto instance : Input::Keyboard::instances)
    {
        instance->keys[key] = action != GLFW_RELEASE;
        instance->mods[key] = mods;
    }
}