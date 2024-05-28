#pragma once

#include <remi/World/World.h>
#include <remi/ECS/System.h>

/**
 * A system that rotates an entity.
 */
class RotateSystem : public ECS::System
{
public:
    /**
     * Creates a new RotateSystem.
     *
     * @param entity The entity to rotate.
     * @param speed The speed at which to rotate the entity. In radians per second.
     */
    RotateSystem(ECS::Entity entity, float speed) : entity(entity), speed(speed) {}

    void update(const ECS::System::SystemUpdateData &data) override
    {
        auto &world = data.world;
        auto &timestep = data.timestep;

        auto &registry = world.getRegistry();

        auto &transform = registry.get<Core::Transform>(entity);
        transform.rotate(speed * timestep.getSeconds());
    }

private:
    ECS::Entity entity;
    float speed;
};