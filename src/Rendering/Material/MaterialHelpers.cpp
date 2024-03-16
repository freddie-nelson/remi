#include "../../../include/Rendering/Material/MaterialHelpers.h"

Rendering::Material *Rendering::getMaterial(const ECS::Registry &registry, ECS::Entity entity)
{
    if (registry.has<ShaderMaterial>(entity))
    {
        return &registry.get<ShaderMaterial>(entity);
    }
    else if (registry.has<AnimatedMaterial>(entity))
    {
        return &registry.get<AnimatedMaterial>(entity);
    }
    else if (!registry.has<Material>(entity))
    {
        throw std::invalid_argument("Entity " + std::to_string(entity) + " does not have a material, shader material or animated material.");
    }
    else
    {
        return &registry.get<Material>(entity);
    }
}