#pragma once

#include "./Rendering/Renderer.h"
#include "./Rendering/Window.h"

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

    // meshs
    std::vector<Rendering::Mesh2D> meshs;

    // instanced meshs
    std::vector<Rendering::Mesh2D> instancedMeshs;

    void update(float dt, Rendering::Renderer *renderer);
    void render(Rendering::Renderer *renderer);
};