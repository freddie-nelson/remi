#include <remi/Engine.h>
#include <remi/Rendering/Camera/Camera.h>
#include <remi/Rendering/Camera/ActiveCamera.h>
#include <remi/Rendering/Mesh/Mesh.h>
#include <remi/Rendering/Material/Material.h>
#include <remi/Rendering/Renderable.h>
#include <remi/Core/Transform.h>
#include <remi/Rendering/Font/Font.h>
#include <remi/Rendering/Font/Text.h>

#include <Fps.h>
#include <BasicCamera.h>

#include <random>

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

    // load font
    Rendering::Font font = Rendering::Font("assets/Anta-Regular.ttf");

    // create text
    Rendering::Text text("Hello, World!", font);

    // create text entity
    auto entity = registry.create();
    registry.add(entity, Core::Transform());
    registry.add(entity, Rendering::Material());
    registry.add(entity, text.mesh(Rendering::Text::TextAlignment::CENTRE));
    registry.add(entity, Rendering::Renderable(true, true));

    // run engine
    engine.run();

    return 0;
}