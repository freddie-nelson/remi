#pragma once

#include <remi/Rendering/Renderer.h>
#include <remi/World/World.h>
#include <remi/ECS/System.h>
#include <remi/Engine.h>

class Application : public ECS::System
{
public:
    Application();
    ~Application();

    void run();

private:
    remi::Engine *engine;

    void init();
    void destroy();

    void update(const ECS::System::SystemUpdateData &data) override;

    void fixedUpdate(const ECS::System::SystemUpdateData &data) override;
};