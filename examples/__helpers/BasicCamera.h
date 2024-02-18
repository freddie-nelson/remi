#pragma once

#include <remi/Engine.h>
#include <remi/Rendering/Camera/Camera.h>
#include <remi/Rendering/Camera/ActiveCamera.h>

class BasicCamera : public World::System
{
public:
    BasicCamera(remi::Engine *engine) : engine(engine)
    {
        auto &world = *engine->getWorld();
        auto &registry = world.getRegistry();

        camera = registry.create();
        registry.add(camera, Rendering::Camera(engine->getConfig().windowWidth, engine->getConfig().windowHeight));
        registry.add(camera, Rendering::ActiveCamera());
        registry.add(camera, Core::Transform());

        world.addSystem(this);
    }

    void update(World::World &world, const Core::Timestep &timestep) override
    {
        auto &keyboard = *engine->getKeyboard();
        auto &registry = world.getRegistry();

        auto &t = registry.get<Core::Transform>(camera);
        float speed = 3.0f;

        glm::vec2 translation(0);

        if (keyboard.isPressed(Input::Key::ARROW_UP))
        {
            translation.y += speed;
        }
        if (keyboard.isPressed(Input::Key::ARROW_DOWN))
        {
            translation.y -= speed;
        }
        if (keyboard.isPressed(Input::Key::ARROW_LEFT))
        {
            translation.x -= speed;
        }
        if (keyboard.isPressed(Input::Key::ARROW_RIGHT))
        {
            translation.x += speed;
        }

        t.translate(translation * static_cast<float>(timestep.getSeconds()));
    }

private:
    ECS::Entity camera;
    remi::Engine *engine;
};