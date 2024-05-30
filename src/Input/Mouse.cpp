#include "../../include/Input/Mouse.h"

#include <iostream>

Input::Mouse::Mouse(Core::Window &window)
    : window(window)
{
    window.attachObserver(Core::WindowPollEventName, this);

    // set initial values
    for (int i = 0; i < MouseButton::__LAST_MOUSE_BUTTON__; i++)
    {
        buttons[i] = false;
    }

    setMode(MouseMode::NORMAL);
}

Input::Mouse::~Mouse()
{
    window.detachObserver(Core::WindowPollEventName, this);
}

glm::vec2 Input::Mouse::getPosition(bool flipY) const
{
    if (flipY)
    {
        auto size = window.getSize();

        return glm::vec2(position.x, size.y - position.y);
    }

    return position;
}

const glm::vec2 &Input::Mouse::getScroll() const
{
    return scroll;
}

bool Input::Mouse::isPressed(MouseButton button) const
{
    return buttons[button];
}

void Input::Mouse::setMode(MouseMode mode)
{
    auto sdlWindow = window.getInternalWindow();

    switch (mode)
    {
    case MouseMode::NORMAL:
        SDL_SetRelativeMouseMode(SDL_FALSE);
        SDL_ShowCursor(SDL_ENABLE);
        break;
    case MouseMode::HIDDEN:
        SDL_SetRelativeMouseMode(SDL_FALSE);
        SDL_ShowCursor(SDL_DISABLE);
        break;
    case MouseMode::DISABLED:
        SDL_SetRelativeMouseMode(SDL_TRUE);
        break;
    }
}

Input::MouseMode Input::Mouse::getMode() const
{
    auto sdlMode = SDL_GetRelativeMouseMode();
    if (sdlMode == SDL_TRUE)
    {
        return MouseMode::DISABLED;
    }

    auto sdlCursor = SDL_ShowCursor(SDL_QUERY);
    if (sdlCursor == SDL_ENABLE)
    {
        return MouseMode::NORMAL;
    }

    return MouseMode::HIDDEN;
}

void Input::Mouse::updateObserver(std::string event, const std::vector<SDL_Event> &data)
{
    std::cout << "Mouse::updateObserver" << std::endl;

    for (auto &event : data)
    {
        switch (event.type)
        {
        case SDL_MOUSEMOTION:
            position = glm::vec2(event.motion.x, event.motion.y);
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button <= MouseButton::__LAST_MOUSE_BUTTON__)
            {
                buttons[event.button.button] = true;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if (event.button.button <= MouseButton::__LAST_MOUSE_BUTTON__)
            {
                buttons[event.button.button] = false;
            }
            break;
        case SDL_MOUSEWHEEL:
            scroll = glm::vec2(event.wheel.x, event.wheel.y);
            break;
        }
    }
}