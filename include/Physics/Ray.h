#pragma once

#include "../ECS/Entity.h"
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
     * Represents a ray.
     */
    struct Ray
    {
        /**
         * Creates a new ray.
         *
         * @param origin The origin of the ray.
         * @param direction The direction of the ray.
         * @param length The length of the ray.
         */
        Ray(glm::vec2 origin, glm::vec2 direction, float length);

        /**
         * Creates a new ray.
         *
         * @param start The start of the ray.
         * @param end The end of the ray.
         */
        Ray(glm::vec2 start, glm::vec2 end);

        /**
         * The start of the ray.
         */
        glm::vec2 start;

        /**
         * The end of the ray.
         */
        glm::vec2 end;

        /**
         * The length of the ray.
         */
        float length;
    };

    /**
     * Represents a raycast hit.
     */
    struct RaycastHit
    {
        /**
         * The entity that was hit.
         */
        ECS::Entity entity;

        /**
         * The point of intersection with the collider.
         */
        glm::vec2 point;

        /**
         * The normal of the ray collision with the collider.
         */
        glm::vec2 normal;

        /**
         * The distance from the start of the ray to the hit.
         */
        float distance;

        /**
         * The fraction along the ray of the hit.
         */
        float fraction;
    };

    /**
     * Represents the type of raycast to perform.
     *
     * ALL: Raycast against all colliders in the world.
     * CLOSEST: Raycast against all colliders in the world and return the closest hit.
     * ANY: Raycast against the world and return the first found hit (may not be the closest).
     */
    enum RaycastType
    {
        ALL,
        CLOSEST,
        ANY,
    };

    /**
     * Represents a raycast callback for box2d.
     */
    class RaycastCallback : public b2RayCastCallback
    {
    public:
        /**
         * Creates a new raycast callback.
         *
         * @param world The world to use.
         * @param ray The ray being cast.
         * @param type The type of raycast to perform.
         * @param bodyToEntity The map of bodies to entities, will use these as entities in hit list.
         */
        RaycastCallback(const World::World &world, const Ray &ray, RaycastType type, const std::unordered_map<b2Body *, ECS::Entity> &bodyToEntity);

        /**
         * Reports a fixture hit by the ray.
         *
         * @param fixture The fixture that was hit.
         * @param point The point of intersection.
         * @param normal The normal of the ray collision.
         * @param fraction The fraction of the ray for the hit.
         */
        float ReportFixture(b2Fixture *fixture, const b2Vec2 &point, const b2Vec2 &normal, float fraction) override;

        /**
         * Gets the hits from the raycast.
         *
         * @returns The hits from the raycast.
         */
        const std::vector<RaycastHit> &getHits() const;

    private:
        const World::World &world;
        Ray ray;
        RaycastType type;

        /**
         * The map of bodies to entities.
         */
        const std::unordered_map<b2Body *, ECS::Entity> &bodyToEntity;

        /**
         * The hit list.
         */
        std::vector<RaycastHit> hits;
    };
}