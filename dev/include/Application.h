#pragma once

#include <remi/Rendering/Renderer.h>
#include <remi/World/World.h>
#include <remi/Engine.h>

class Application : public World::System
{
public:
    Application();
    ~Application();

    void run();

private:
    remi::Engine *engine;

    void init();
    void destroy();

    void update(World::World &world, const Core::Timestep &timestep);

    void fixedUpdate(World::World &world, const Core::Timestep &timestep);
};