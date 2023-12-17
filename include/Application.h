#pragma once

#include "./Rendering/Renderer.h"
#include "./Rendering/Window.h"
#include "./ECS/Registry.h"

#include <string>

class Application
{
public:
    Application(std::string windowTitle, int windowWidth, int windowHeight);
    ~Application();

    int run();

private:
    std::string windowTitle;
    int initialWindowWidth;
    int initialWindowHeight;

    Rendering::Window *window;

    int init();
    void destroy();

    ECS::Registry registry;

    void update(float dt, Rendering::Renderer *renderer);
    void render(Rendering::Renderer *renderer);
};