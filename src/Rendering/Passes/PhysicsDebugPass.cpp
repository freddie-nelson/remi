#include "../../../include/Rendering/Passes/PhysicsDebugPass.h"
#include "../../../include/Core/Transform.h"
#include "../../../include/Rendering/Material/Material.h"
#include "../../../include/Rendering/Renderable.h"
#include "../../../include/Config.h"

#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>

Rendering::PhysicsDebugPass::PhysicsDebugPass(const Physics::PhysicsWorld *physicsWorld) : physicsWorld(physicsWorld)
{
}

Rendering::PhysicsDebugPass::~PhysicsDebugPass()
{
}

Rendering::RenderPassInput *Rendering::PhysicsDebugPass::execute(Rendering::RenderPassInput *input)
{
    checkInput<int>(input);

    auto &world = *input->world;
    auto &registry = world.getRegistry();
    auto &renderer = *input->renderer;

    auto &bodies = physicsWorld->getBodies();
    auto &colliders = physicsWorld->getColliders();

    auto &renderTarget = *input->renderTarget;
    auto &textureManager = *input->textureManager;

    renderTarget.bind(textureManager);

    std::vector<ECS::Entity> entities;

    for (auto &[_, fixtures] : colliders)
    {
        for (auto fixture : fixtures) {
            auto &aabb = fixture->GetAABB(0);

            auto e = registry.create();
            auto &mesh = registry.add(e, Rendering::Mesh2D(aabb.GetExtents().x * 2, aabb.GetExtents().y * 2));
            auto &transform = registry.add(e, Core::Transform());
            auto &material = registry.add(e, Rendering::Material());
            auto &renderable = registry.add(e, Rendering::Renderable(true, false));

            auto shape = fixture->GetShape();

            if (shape->GetType() == b2Shape::e_polygon)
            {
                auto polygon = reinterpret_cast<b2PolygonShape *>(shape);
                auto vertices = polygon->m_vertices;

                std::vector<glm::vec2> points;
                for (int i = 0; i < polygon->m_count; i++)
                {
                    points.push_back(glm::vec2(vertices[i].x, vertices[i].y));
                }

                mesh.createPolygon(points);
            }
            else if (shape->GetType() == b2Shape::e_circle)
            {
                auto circle = reinterpret_cast<b2CircleShape *>(shape);
                auto radius = circle->m_radius;

                mesh.createRegularPolygon(radius, 32);
            }

            transform.setZIndex(Config::MAX_Z_INDEX);
            transform.setTranslation(glm::vec2(aabb.GetCenter().x, aabb.GetCenter().y));
            transform.setRotation(fixture->GetBody()->GetAngle());

            if (fixture->IsSensor())
            {
                material.setColor(Rendering::Color(1.0f, 0.0f, 0.0f, 0.3f));
            }
            else
            {
                material.setColor(Rendering::Color(0.0f, 1.0f, 0.0f, 0.3f));
            }

            // std::cout << "aabb: " << aabb.GetCenter().x << ", " << aabb.GetCenter().y << ", " << aabb.GetExtents().x << ", " << aabb.GetExtents().y << std::endl;

            entities.push_back(e);
        }
    }

    renderer.batch(world, renderer.getActiveCamera(registry), entities);

    registry.destroy(entities);
    renderTarget.unbind(textureManager);

    return input;
}