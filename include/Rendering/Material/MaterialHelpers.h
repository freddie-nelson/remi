#pragma once

#include "../../ECS/Registry.h"
#include "../../ECS/Entity.h"
#include "Material.h"
#include "ShaderMaterial.h"
#include "AnimatedMaterial.h"

namespace Rendering
{
    /**
     * Gets the material for the given entity.
     *
     * Assumes the entity has a Material or ShaderMaterial component.
     *
     * This will prioritize the ShaderMaterial component over the Material component.
     *
     * @param registry The registry to read components from.
     * @param entity The entity to get the material for.
     *
     * @returns The material for the given entity.
     */
    Material *getMaterial(const ECS::Registry &registry, ECS::Entity entity);
}