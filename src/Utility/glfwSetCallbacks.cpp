#include "../../include/Utility/glfwSetCallbacks.h"

std::function<void()> windowCloseCallback;

void Utility::setWindowCloseCallback(GLFWwindow *window, std::function<void()> callback)
{
    windowCloseCallback = std::move(callback);

    auto lambda = [](GLFWwindow *window)
    {
        windowCloseCallback();
    };

    glfwSetWindowCloseCallback(window, lambda);
}

std::function<void(int, int)> windowSizeCallback;

void Utility::setWindowSizeCallback(GLFWwindow *window, std::function<void(int, int)> callback)
{
    windowSizeCallback = std::move(callback);

    auto lambda = [](GLFWwindow *window, int width, int height)
    {
        windowSizeCallback(width, height);
    };

    glfwSetWindowSizeCallback(window, lambda);
}

std::function<void(int, int, int, int)> keyCallback;

void Utility::setKeyCallback(GLFWwindow *window, std::function<void(int, int, int, int)> callback)
{
    keyCallback = std::move(callback);

    auto lambda = [](GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        keyCallback(key, scancode, action, mods);
    };

    glfwSetKeyCallback(window, lambda);
}

std::function<void(int, int, int)> mouseButtonCallback;

void Utility::setMouseButtonCallback(GLFWwindow *window, std::function<void(int, int, int)> callback)
{
    mouseButtonCallback = std::move(callback);

    auto lambda = [](GLFWwindow *window, int button, int action, int mods)
    {
        mouseButtonCallback(button, action, mods);
    };

    glfwSetMouseButtonCallback(window, lambda);
}

std::function<void(double, double)> mousePosCallback;

void Utility::setMousePosCallback(GLFWwindow *window, std::function<void(double, double)> callback)
{
    mousePosCallback = std::move(callback);

    auto lambda = [](GLFWwindow *window, double x, double y)
    {
        mousePosCallback(x, y);
    };

    glfwSetCursorPosCallback(window, lambda);
}

std::function<void(double, double)> mouseScrollCallback;

void Utility::setMouseScrollCallback(GLFWwindow *window, std::function<void(double, double)> callback)
{
    mouseScrollCallback = std::move(callback);

    auto lambda = [](GLFWwindow *window, double x, double y)
    {
        mouseScrollCallback(x, y);
    };

    glfwSetScrollCallback(window, lambda);
}