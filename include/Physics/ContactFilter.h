#include "../ECS/Entity.h"
#include "../World/World.h"

#include <box2d/b2_world_callbacks.h>
#include <functional>

namespace Physics
{
    using ContactFilterCallback = std::function<bool(const World::World &, ECS::Entity, ECS::Entity)>;

    /**
     * Represents a contact filter for the physics world.
     *
     * This can be used for fine grained control over which entities collide with each other.
     */
    class ContactFilter : public b2ContactFilter
    {
    public:
        /**
         * Creates a new contact filter.
         *
         * @param world The world to use.
         * @param bodyToEntity The map of bodies to entities.
         */
        ContactFilter(const World::World &world, const std::unordered_map<b2Body *, ECS::Entity> &bodyToEntity);

        /**
         * Called when two fixtures should collide.
         *
         * @param fixtureA The first fixture.
         * @param fixtureB The second fixture.
         *
         * @returns True if the fixtures should collide, false otherwise.
         */
        bool ShouldCollide(b2Fixture *fixtureA, b2Fixture *fixtureB) override;

        /**
         * Sets the callback to use for filtering contacts.
         *
         * @param callback The callback to use.
         */
        void setCallback(ContactFilterCallback callback);

    private:
        const World::World &world;
        const std::unordered_map<b2Body *, ECS::Entity> &bodyToEntity;

        ContactFilterCallback callback = nullptr;
    };
}