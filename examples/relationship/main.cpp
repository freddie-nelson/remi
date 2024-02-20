#include <remi/Engine.h>
#include <remi/Rendering/Camera/Camera.h>
#include <remi/Rendering/Camera/ActiveCamera.h>
#include <remi/Rendering/Mesh/Mesh.h>
#include <remi/Rendering/Material/Material.h>
#include <remi/Rendering/Renderable.h>
#include <remi/Core/Transform.h>

#include <Fps.h>
#include <BasicCamera.h>

#include "RotateSystem.h"

int main()
{
    // create engine
    remi::EngineConfig config;
    remi::Engine engine(config);

    // get world and registry
    auto &world = *engine.getWorld();
    auto &registry = world.getRegistry();
    auto &sceneGraph = world.getSceneGraph();

    // add camera and fps counter
    BasicCamera camera(&engine);
    Fps fps(&engine);

    // create parent in the middle of the screen
    auto parent = registry.create();
    registry.add(parent, Core::Transform());
    registry.add(parent, Rendering::Mesh2D(2.0f, 2.0f));
    registry.add(parent, Rendering::Material());
    registry.add(parent, Rendering::Renderable(true, true));

    // add child
    auto child = registry.create();
    registry.add(child, Rendering::Mesh2D(1.0f, 1.0f));
    registry.add(child, Rendering::Material(Rendering::Color(1.0f, 0.0f, 0.0f, 1.0f)));
    registry.add(child, Rendering::Renderable(true, false));

    auto &t = registry.add(child, Core::Transform());
    t.translate(glm::vec2(2.0f, 2.0f));

    // add child to parent
    sceneGraph.relate(parent, child);

    // add rotate system to rotate parent
    RotateSystem rotateSystem(parent, 1.0f);
    world.addSystem(&rotateSystem);

    // run engine
    engine.run();

    return 0;
}