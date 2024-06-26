#include <remi/Engine.h>
#include <remi/Rendering/Camera/Camera.h>
#include <remi/Rendering/Camera/ActiveCamera.h>
#include <remi/Rendering/Mesh/Mesh.h>
#include <remi/Rendering/Material/Material.h>
#include <remi/Rendering/Renderable.h>
#include <remi/Core/Transform.h>
#include <remi/Rendering/Font/Font.h>
#include <remi/Rendering/Font/Text.h>
#include <remi/Physics/RigidBody2d.h>
#include <remi/Physics/Collider2d.h>

#include <Fps.h>
#include <BasicCamera.h>

#include <random>

int main()
{
    // create engine
    remi::EngineConfig config;
    // config.drawDebugPhysics = true;

    remi::Engine engine(config);

    // get world and registry
    auto &world = *engine.getWorld();
    auto &registry = world.getRegistry();
    auto &sceneGraph = world.getSceneGraph();

    // add camera and fps counter
    BasicCamera camera(&engine);
    Fps fps(&engine);

    // create floor
    auto floor = registry.create();
    auto &floorMesh = registry.add(floor, Rendering::Mesh2D(100.0f, 1.0f));
    registry.add(floor, Rendering::Material());
    registry.add(floor, Rendering::Renderable(true, true));

    // make floor static rigidbody
    auto &floorBody = registry.add(floor, Physics::RigidBody2D());
    floorBody.setType(Physics::RigidBodyType::STATIC);

    // use floor mesh to create collider
    auto floorShape = new Physics::PolygonColliderShape2D(floorMesh);
    auto &floorCollider = registry.add(floor, Physics::Collider2D(floorShape));

    // make floor non slippery
    floorCollider.setFriction(0.5f);

    // shape is clone in so we can delete it
    delete floorShape;

    // move floor down
    auto &floorTransform = registry.add(floor, Core::Transform());
    floorTransform.translate(glm::vec2(0.0f, -3.0f));

    // load font
    Rendering::Font font = Rendering::Font("assets/Anta-Regular.ttf");

    // create text
    std::string s = "Hello, World!";

    for (size_t i = 0; i < s.size(); i++)
    {
        auto &c = s[i];

        // skip spaces
        if (c == ' ')
            continue;

        // create text
        Rendering::Text text(std::string(1, c), font);

        // create character entity
        auto charEntity = registry.create();
        registry.add(charEntity, Rendering::Material());
        registry.add(charEntity, Rendering::Renderable(true, true));

        // create char mesh as normal
        auto &charMesh = registry.add(charEntity, text.mesh(Rendering::Text::TextAlignment::CENTRE));

        // add physics to text
        registry.add(charEntity, Physics::RigidBody2D());

        // add transform
        auto &charTransform = registry.add(charEntity, Core::Transform());
        charTransform.setTranslation(glm::vec2((i - s.size() / 2.0f) * 0.5f, 0.0f));

        // use char mesh to create collider
        // note: char mesh is concave so we use compound polygon collider
        auto charShape = Physics::CompoundPolygonColliderShape2D(charMesh);
        auto &charCollider = registry.add(charEntity, Physics::Collider2D(&charShape));

        // make text non slippery
        charCollider.setFriction(0.5f);
    }

    // run engine
    engine.run();

    return 0;
}