#include "../../../include/Rendering/Texture/AnimationSystem.h"
#include "../../../include/Rendering/Material/Material.h"
#include "../../../include/Rendering/Material/ShaderMaterial.h"
#include "../../../include/Rendering/Texture/AnimatedTexture.h"

Rendering::AnimationSystem::AnimationSystem()
{
}

Rendering::AnimationSystem::~AnimationSystem()
{
}

void Rendering::AnimationSystem::update(World::World &world, const Core::Timestep &timestep)
{
    auto &registry = world.getRegistry();

    auto &materialEntities = registry.view<Material>();
    auto &shaderMaterialEntities = registry.view<ShaderMaterial>();

    // step all animated textures
    auto dt = timestep.getMilliseconds();

    for (auto &entity : materialEntities)
    {
        auto &material = registry.get<Material>(entity);

        if (material.isAnimated())
        {
            material.getAnimatedTexture()->step(dt);
        }
    }

    for (auto &entity : shaderMaterialEntities)
    {
        auto &material = registry.get<ShaderMaterial>(entity);

        if (material.isAnimated())
        {
            material.getAnimatedTexture()->step(dt);
        }
    }
}
