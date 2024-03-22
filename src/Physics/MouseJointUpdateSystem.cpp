#include "../../include/Physics/MouseJointUpdateSystem.h"

Physics::MouseJointUpdateSystem::MouseJointUpdateSystem(Input::Mouse *mouse, Core::SpaceTransformer *spaceTransformer)
    : mouse(mouse), spaceTransformer(spaceTransformer)
{
    if (mouse == nullptr)
    {
        throw std::invalid_argument("MouseJointUpdateSystem (constructor): Mouse input cannot be null.");
    }

    if (spaceTransformer == nullptr)
    {
        throw std::invalid_argument("MouseJointUpdateSystem (constructor): Space transformer cannot be null.");
    }
}

void Physics::MouseJointUpdateSystem::update(World::World &world, const Core::Timestep &timestep)
{
    auto mouse = this->mouse->getPosition(true);
    auto mouseWorld = this->spaceTransformer->transform(mouse, Core::SpaceTransformer::Space::SCREEN, Core::SpaceTransformer::Space::WORLD);

    auto &registry = world.getRegistry();
    auto entities = registry.view<MouseJoint>();

    for (auto entity : entities)
    {
        auto &joint = registry.get<MouseJoint>(entity);

        if (joint.getAutoUpdate())
        {
            joint.setTarget(mouseWorld);
        }
    }
}