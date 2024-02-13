#include "../../include/Input/Mouse.h"

#include <iostream>

Input::Mouse::Mouse(GLFWwindow *window)
    : window(window)
{
    // set callbacks
    if (instances.size() == 0)
    {
        glfwSetCursorPosCallback(window, Input::Mouse::mousePositionCallback);
        glfwSetMouseButtonCallback(window, Input::Mouse::mouseButtonCallback);
        glfwSetScrollCallback(window, Input::Mouse::mouseScrollCallback);
    }

    Input::Mouse::instances.push_back(this);

    // set initial values
    for (int i = 0; i < MouseButton::__LAST_MOUSE_BUTTON__; i++)
    {
        buttons[i] = false;
    }
}

Input::Mouse::~Mouse()
{
    // remove this instance from the list of instances
    for (auto it = Input::Mouse::instances.begin(); it != Input::Mouse::instances.end(); it++)
    {
        if (*it == this)
        {
            Input::Mouse::instances.erase(it);
            break;
        }
    }
}

glm::vec2 Input::Mouse::getPosition(bool flipY)
{
    if (flipY)
    {
        int width, height;
        glfwGetWindowSize(window, &width, &height);

        return glm::vec2(position.x, height - position.y);
    }

    return position;
}

glm::vec2 Input::Mouse::getScroll()
{
    return scroll;
}

bool Input::Mouse::isPressed(MouseButton button)
{
    return buttons[button];
}

void Input::Mouse::setMode(MouseMode mode)
{
    switch (mode)
    {
    case MouseMode::NORMAL:
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        break;
    case MouseMode::HIDDEN:
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        break;
    case MouseMode::DISABLED:
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        break;
    }
}

void Input::Mouse::enableRawMotion()
{
    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
}

void Input::Mouse::disableRawMotion()
{
    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
}

std::vector<Input::Mouse *> Input::Mouse::instances;

void Input::Mouse::mousePositionCallback(GLFWwindow *window, double x, double y)
{
    // std::cout << "Mouse position: " << x << ", " << y << std::endl;

    for (auto instance : instances)
    {
        instance->position.x = static_cast<float>(x);
        instance->position.y = static_cast<float>(y);
    }
}

void Input::Mouse::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    // std::cout << "Mouse button: " << button << ", " << action << ", " << mods << std::endl;

    for (auto instance : instances)
    {
        if (button >= 0 && button < 5)
        {
            instance->buttons[button] = action != GLFW_RELEASE;
        }
    }
}

void Input::Mouse::mouseScrollCallback(GLFWwindow *window, double x, double y)
{
    for (auto instance : instances)
    {
        instance->scroll.x = static_cast<float>(x);
        instance->scroll.y = static_cast<float>(y);
    }
}