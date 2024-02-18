#include <remi/Engine.h>
#include <remi/Rendering/Camera/Camera.h>
#include <remi/Rendering/Camera/ActiveCamera.h>
#include <remi/Rendering/Mesh/Mesh.h>
#include <remi/Rendering/Material/Material.h>
#include <remi/Rendering/Renderable.h>
#include <remi/Core/Transform.h>

int main()
{
    // create engine
    remi::EngineConfig config;
    remi::Engine engine(config);

    // get world and registry
    auto &world = *engine.getWorld();
    auto &registry = world.getRegistry();

    // create camera
    // we need an active camera to see anything
    auto camera = registry.create();
    registry.add(camera, Rendering::Camera(config.windowWidth, config.windowHeight));
    registry.add(camera, Rendering::ActiveCamera());
    registry.add(camera, Core::Transform());

    // create white square in the middle of the screen
    auto square = registry.create();
    registry.add(square, Core::Transform());
    registry.add(square, Rendering::Mesh2D(1.0f, 1.0f));
    registry.add(square, Rendering::Material());

    // we need to proivde a renderable component so the renderer knows
    // the entity should be rendered
    registry.add(square, Rendering::Renderable(true, true));

    // run engine
    engine.run();

    return 0;
}