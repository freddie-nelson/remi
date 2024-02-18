#include <remi/Engine.h>
#include <remi/Rendering/Camera/Camera.h>
#include <remi/Rendering/Camera/ActiveCamera.h>
#include <remi/Rendering/Mesh/Mesh.h>
#include <remi/Rendering/Material/Material.h>
#include <remi/Rendering/Renderable.h>
#include <remi/Core/Transform.h>

#include <Fps.h>
#include <BasicCamera.h>

int main()
{
    // create engine
    remi::EngineConfig config;
    remi::Engine engine(config);

    // get world and registry
    auto &world = *engine.getWorld();
    auto &registry = world.getRegistry();

    // add camera and fps counter
    BasicCamera camera(&engine);
    Fps fps(&engine);

    // create  square in the middle of the screen
    auto square = registry.create();
    registry.add(square, Core::Transform());
    registry.add(square, Rendering::Mesh2D(4.0f, 4.0f));

    // create animated texture
    size_t frameCount = 61;
    std::vector<std::string> frames;

    for (size_t i = 0; i < frameCount; i++)
    {
        frames.push_back("assets/1_" + std::to_string(i) + ".png");
    }

    Rendering::AnimatedTexture texture(frames, 1000, Rendering::AnimatedTexture::AnimationMode::LOOP);

    // create material
    auto &material = registry.add(square, Rendering::Material());
    material.setTexture(&texture);

    // we need to proivde a renderable component so the renderer knows
    // the entity should be rendered
    registry.add(square, Rendering::Renderable(true, true));

    // run engine
    engine.run();

    return 0;
}