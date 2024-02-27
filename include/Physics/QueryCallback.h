#pragma once

#include "../ECS/Entity.h"
#include "../Core/AABB/AABB.h"
#include "../Core/BoundingCircle.h"
#include "../World/World.h"

#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_fixture.h>

namespace Physics
{
    /**
     * Represents a query callback for box2d.
     */
    class QueryCallback : public b2QueryCallback
    {
    public:
        /**
         * Creates a new raycast callback.
         *
         * @param world The world to use.
         * @param ray The ray being cast.
         * @param bodyToEntity The map of bodies to entities, will use these as entities in query results.
         */
        QueryCallback(const World::World &world, const Core::AABB &aabb, const std::unordered_map<b2Body *, ECS::Entity> &bodyToEntity);

        /**
         * Reports a fixture inside the bounding shape.
         *
         * @param fixture The fixture that was hit.
         */
        bool ReportFixture(b2Fixture *fixture) override;

        /**
         * Gets the entities found inside the bounding shape.
         *
         * @returns A vector of query results.
         */
        const std::vector<ECS::Entity> &getResults() const;

    private:
        const World::World &world;
        const Core::AABB &aabb;

        /**
         * The map of bodies to entities.
         */
        const std::unordered_map<b2Body *, ECS::Entity> &bodyToEntity;

        /**
         * The results.
         */
        std::vector<ECS::Entity> results;
    };
}