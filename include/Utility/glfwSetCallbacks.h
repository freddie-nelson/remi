#pragma once

#include <functional>

#include <GLFW/glfw3.h>

namespace Utility
{
    void setWindowCloseCallback(GLFWwindow *window, std::function<void()> callback);

    void setWindowSizeCallback(GLFWwindow *window, std::function<void(int, int)> callback);

    void setKeyCallback(GLFWwindow *window, std::function<void(int, int, int, int)> callback);

    void setMouseButtonCallback(GLFWwindow *window, std::function<void(int, int, int)> callback);

    void setMousePosCallback(GLFWwindow *window, std::function<void(double, double)> callback);

    void setMouseScrollCallback(GLFWwindow *window, std::function<void(double, double)> callback);
}