#include "../../../include/Rendering/Passes/PhysicsDebugPass.h"
#include "../../../include/Core/Transform.h"
#include "../../../include/Rendering/Material/Material.h"
#include "../../../include/Rendering/Renderable.h"
#include "../../../include/Config.h"

#include <box2d/b2_fixture.h>

Rendering::PhysicsDebugPass::PhysicsDebugPass(const Physics::PhysicsWorld *physicsWorld) : physicsWorld(physicsWorld)
{
}

Rendering::PhysicsDebugPass::~PhysicsDebugPass()
{
}

Rendering::RenderPassInput *Rendering::PhysicsDebugPass::execute(Rendering::RenderPassInput *input)
{
    checkInput<int>(input);

    auto &registry = *const_cast<ECS::Registry *>(input->registry);
    auto &renderer = *input->renderer;

    auto &bodies = physicsWorld->getBodies();
    auto &colliders = physicsWorld->getColliders();

    auto &renderTarget = *input->renderTarget;
    auto &textureManager = *input->textureManager;

    renderTarget.bind(textureManager);

    std::vector<ECS::Entity> entities;

    for (auto &[_, collider] : colliders)
    {
        auto &aabb = collider->GetAABB(0);

        auto e = registry.create();
        auto &transform = registry.add(e, Core::Transform());
        auto &mesh = registry.add(e, Rendering::Mesh2D(aabb.GetExtents().x * 2, aabb.GetExtents().y * 2));
        auto &material = registry.add(e, Rendering::Material());
        auto &renderable = registry.add(e, Rendering::Renderable(true, false));

        transform.setZIndex(Config::MAX_Z_INDEX);
        transform.setTranslation(glm::vec2(aabb.GetCenter().x, aabb.GetCenter().y));

        material.setColor(Rendering::Color(0.0f, 1.0f, 0.0f, 0.4f));

        // std::cout << "aabb: " << aabb.GetCenter().x << ", " << aabb.GetCenter().y << ", " << aabb.GetExtents().x << ", " << aabb.GetExtents().y << std::endl;

        entities.push_back(e);
    }

    renderer.batch(registry, renderer.getActiveCamera(registry), entities);

    registry.destroy(entities);
    renderTarget.unbind(textureManager);

    return input;
}