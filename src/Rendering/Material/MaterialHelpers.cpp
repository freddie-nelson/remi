#include "../../../include/Rendering/Material/MaterialHelpers.h"

Rendering::Material *Rendering::getMaterial(const ECS::Registry &registry, ECS::Entity entity)
{
    if (registry.has<ShaderMaterial>(entity))
    {
        return &registry.get<ShaderMaterial>(entity);
    }
    else
    {
        return &registry.get<Material>(entity);
    }
}