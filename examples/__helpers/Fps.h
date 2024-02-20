#pragma once

#include <remi/World/World.h>
#include <remi/Rendering/Font/MemoizedText.h>
#include <remi/Rendering/Renderable.h>
#include <remi/Core/Transform.h>
#include <remi/Rendering/Material/Material.h>
#include <remi/Rendering/Font/Font.h>
#include <remi/Config.h>
#include <remi/Engine.h>

class Fps : public World::System
{
public:
    Fps(remi::Engine *engine) : engine(engine)
    {
        auto &world = *engine->getWorld();
        auto &registry = world.getRegistry();
        auto &sceneGraph = world.getSceneGraph();

        font = new Rendering::Font("assets/Roboto-Regular.ttf");

        auto &text = Rendering::MemoizedText::text("FPS: 0", font);

        entity = registry.create();
        registry.add(entity, Rendering::Material(Rendering::Color(1.0f, 1.0f, 1.0f, 1.0f)));
        registry.add(entity, text.mesh());
        registry.add(entity, Rendering::Renderable(true, false));

        auto &t = registry.add(entity, Core::Transform());
        t.setZIndex(Config::MAX_Z_INDEX);

        auto &spaceTransformer = *engine->getSpaceTransformer();
        auto &renderer = *engine->getRenderer();

        auto camera = renderer.getActiveCamera(registry);
        auto rendererHeight = renderer.getHeight();

        sceneGraph.relate(camera, entity);
        auto pos = spaceTransformer.transform(glm::vec2(65.0f, rendererHeight - 20.0f), camera, Core::SpaceTransformer::Space::SCREEN, Core::SpaceTransformer::Space::LOCAL);

        t.setTranslation(pos);
        t.setScale(glm::vec2(0.3f, 0.3f));

        world.addSystem(this);
    }

    void update(World::World &world, const Core::Timestep &timestep) override
    {
        auto &registry = world.getRegistry();

        int fps = 1.0f / timestep.getSeconds();
        auto &text = Rendering::MemoizedText::text("FPS: " + std::to_string(fps), font);

        auto &mesh = registry.get<Rendering::Mesh2D>(entity);
        mesh = text.mesh();
    }

private:
    remi::Engine *engine;
    Rendering::Font *font;

    ECS::Entity entity;
};