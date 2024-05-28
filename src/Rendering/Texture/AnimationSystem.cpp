#include "../../../include/Rendering/Texture/AnimationSystem.h"
#include "../../../include/Rendering/Material/AnimatedMaterial.h"

Rendering::AnimationSystem::AnimationSystem()
{
}

Rendering::AnimationSystem::~AnimationSystem()
{
}

void Rendering::AnimationSystem::update(const ECS::System::SystemUpdateData &data)
{
    auto &world = data.world;
    auto &timestep = data.timestep;

    auto &registry = world.getRegistry();

    auto &entities = registry.view<AnimatedMaterial>();

    // step all animated textures
    auto dt = timestep.getSeconds();

    for (auto &entity : entities)
    {
        auto &animatedMaterial = registry.get<AnimatedMaterial>(entity);
        animatedMaterial.step(dt);
    }
}
