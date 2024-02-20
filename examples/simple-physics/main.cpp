#include <remi/Engine.h>
#include <remi/Rendering/Camera/Camera.h>
#include <remi/Rendering/Camera/ActiveCamera.h>
#include <remi/Rendering/Mesh/Mesh.h>
#include <remi/Rendering/Material/Material.h>
#include <remi/Rendering/Renderable.h>
#include <remi/Core/Transform.h>
#include <remi/Physics/RigidBody2D.h>
#include <remi/Physics/Collider2D.h>

#include <Fps.h>
#include <BasicCamera.h>

#include <random>

int main()
{
    srand(time(0));

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

    // create boxes
    size_t count = 50;
    float size = 0.5f;
    size_t xRange = 12;
    size_t yRange = 4;

    for (size_t i = 0; i < count; i++)
    {
        auto box = registry.create();
        auto &boxMesh = registry.add(box, Rendering::Mesh2D(size, size));
        registry.add(box, Rendering::Material());
        registry.add(box, Rendering::Renderable(true, false));

        // make box dynamic rigidbody
        auto &boxBody = registry.add(box, Physics::RigidBody2D());
        boxBody.setType(Physics::RigidBodyType::DYNAMIC);

        // use box mesh to create collider
        auto boxShape = new Physics::PolygonColliderShape2D(boxMesh);
        auto &boxCollider = registry.add(box, Physics::Collider2D(boxShape));

        // make box non slippery
        boxCollider.setFriction(0.5f);

        // shape is clone in so we can delete it
        delete boxShape;

        // set position
        auto &t = registry.add(box, Core::Transform());
        t.translate(glm::vec2((rand() % xRange) - xRange / 2.0f, rand() % yRange));
    }

    // run engine
    engine.run();

    return 0;
}