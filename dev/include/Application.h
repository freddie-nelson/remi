#pragma once

#include <blaze++/Rendering/Renderer.h>
#include <blaze++/World/World.h>
#include <blaze++/Engine.h>

class Application : public World::System
{
public:
    Application();
    ~Application();

    void run();

private:
    blz::Engine *engine;

    void init();
    void destroy();

    void update(World::World &world, const Core::Timestep &timestep);

    void fixedUpdate(World::World &world, const Core::Timestep &timestep);
};