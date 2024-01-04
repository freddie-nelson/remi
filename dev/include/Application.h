#pragma once

#include <blaze++/Rendering/Renderer.h>
#include <blaze++/Rendering/Window.h>
#include <blaze++/ECS/Registry.h>

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