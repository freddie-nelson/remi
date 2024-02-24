#pragma once

#include "../World/World.h"
#include "../Core/SpaceTransformer.h"
#include "Ray.h"
#include "ContactListener.h"
#include "BodyUserData.h"
#include "./Joints/Joint.h"

#include <unordered_map>
#include <vector>
#include <box2d/b2_world.h>
#include <box2d/b2_joint.h>
#include <glm/vec2.hpp>

// TODO: multithread the physics world
// could do this by running the box2d world in a separate thread and then just updating the entity components on fixedUpdate based on physics world state
// and sending any new data to the physics world thread

namespace Physics
{
    /**
     * The physics world config.
     *
     * Configures how the physics world will behave.
     */
    struct PhysicsWorldConfig
    {
        /**
         * The gravity of the world.
         */
        glm::vec2 gravity = glm::vec2(0.0f, -10.0f);

        /**
         * The number of velocity iterations to use.
         *
         * Recommended to use 8.
         */
        int velocityIterations = 8;

        /**
         * The number of position iterations to use.
         *
         * Recommended to use 3.
         */
        int positionIterations = 3;
    };

    /**
     * Represents the physics world.
     *
     * Stores and updates the physics world.
     */
    class PhysicsWorld : public World::System
    {
    public:
        /**
         * Creates a new physics world.
         */
        PhysicsWorld(PhysicsWorldConfig config, const Core::SpaceTransformer *spaceTransformer);

        /**
         * Destroys the physics world.
         */
        ~PhysicsWorld();

        /**
         * Updates the physics world.
         *
         * @param world The world to use.
         * @param timestep The timestep since the last update.
         */
        void fixedUpdate(World::World &world, const Core::Timestep &timestep) override;

        // ! TODO: support filtering for raycasts (by entity? by collision filter? etc)

        /**
         * Raycasts against all bodies with colliders in the world.
         *
         * If a ray starts inside a collider, that collider will not be included in the results.
         *
         * @param ray The ray to cast.
         * @param type The type of raycast to perform.
         *
         * @returns A vector of raycast hits.
         */
        std::vector<RaycastHit> raycast(const Ray &ray, RaycastType type = RaycastType::ALL);

        /**
         * Queries the world for all entities with colliders that are within the given AABB.
         *
         * @param aabb The AABB to query.
         *
         * @returns A vector of query results.
         */
        std::vector<ECS::Entity> query(const Core::AABB &aabb);

        /**
         * Queries the world for all entities with colliders that are within the given AABB.
         *
         * @param circle The bounding circle to query.
         *
         * @returns A vector of query results.
         */
        std::vector<ECS::Entity> query(const Core::BoundingCircle &circle);

        /**
         * Sets the physics world's config.
         *
         * @param config The config object.
         */
        void setConfig(PhysicsWorldConfig config);

        /**
         * Gets the physics world's config.
         *
         * @returns The physics world's config.
         */
        const PhysicsWorldConfig &getConfig();

        /**
         * Sets the world's gravity vector.
         *
         * @param gravity The world's gravity.
         */
        void setGravity(glm::vec2 gravity);

        /**
         * Gets the world's gravity vector.
         *
         * @returns The world's gravity vector.
         */
        glm::vec2 getGravity();

        /**
         * Sets the number of velocity iterations to use.
         *
         * @param iterations The number of iterations to use.
         */
        void setVelocityIterations(int iterations);

        /**
         * Gets the number of velocity iterations to use.
         *
         * @returns The number of velocity iterations to use.
         */
        int getVelocityIterations();

        /**
         * Sets the number of position iterations to use.
         *
         * @param iterations The number of iterations to use.
         */
        void setPositionIterations(int iterations);

        /**
         * Gets the number of position iterations to use.
         *
         * @returns The number of position iterations to use.
         */
        int getPositionIterations();

        /**
         * Gets the box2d bodies.
         *
         * @returns The box2d bodies.
         */
        const std::unordered_map<ECS::Entity, b2Body *> &getBodies() const;

        /**
         * Gets the box2d colliders.
         *
         * @returns The box2d colliders.
         */
        const std::unordered_map<ECS::Entity, std::vector<b2Fixture *>> &getColliders() const;

    private:
        PhysicsWorldConfig config;

        const Core::SpaceTransformer *spaceTransformer;

        /**
         * The box2d world.
         */
        b2World world;

        /**
         * The entity to body map.
         */
        std::unordered_map<ECS::Entity, b2Body *> bodies;

        /**
         * The entity to colliders map.
         *
         * This is a map of entities to their colliders (fixtures).
         */
        std::unordered_map<ECS::Entity, std::vector<b2Fixture *>> colliders;

        /**
         * The body to entity map.
         *
         * Should only be used within the physics world class.
         */
        std::unordered_map<b2Body *, ECS::Entity> bodyToEntity;

        /**
         * The joints map.
         *
         * This is the joints owned by each entity. Entities can only own one of each joint type.
         */
        std::unordered_map<ECS::Entity, std::unordered_map<JointType, b2Joint *>> joints;

        /**
         * The contact listener for the world.
         */
        ContactListener contactListener;

        /**
         * Updates the bodies map.
         *
         * Will add or remove bodies from the map if they are added or removed from the registry.
         *
         * Will also update the bodies with the latest data from the registry.
         *
         * @param world The world to use.
         */
        void updateBodies(World::World &world);

        /**
         * Creates the bodies for the entities.
         *
         * This will only create bodies for entities that are not already in the bodies map.
         *
         * @param world The world to use.
         * @param entitySet The set of entities to create bodies for.
         */
        void createBodies(const World::World &world, const std::unordered_set<ECS::Entity> &entitySet);

        /**
         * Destroys the given box2d body.
         *
         * This will delete the bodies user data and remove the body from the associated maps.
         *
         * @param world The world to use.
         * @param body The body to destroy.
         */
        void destroyBody(World::World &world, b2Body *body);

        /**
         * Injects the data from the ecs components into the box2d bodies.
         *
         * @param world The world to use.
         * @param createdEntities The set of entities that were created this frame, these don't need updated.
         */
        void updateBodiesWithECSValues(const World::World &world, const std::unordered_set<ECS::Entity> &createdEntities);

        /**
         * Creates a Box2D collider for the entity.
         *
         * @param world The world to use.
         * @param entity The entity to create the collider for.
         */
        void createBox2DCollider(const World::World &world, ECS::Entity entity);

        /**
         * Destroys a Box2D collider for the entity.
         *
         * Removes the collider from the world and the colliders map.
         *
         * @param entity The entity to destroy the collider for.
         */
        void destroyBox2DCollider(ECS::Entity entity);

        /**
         * Updates the Box2D world with the values from the ECS.
         *
         * @param world The world to use.
         */
        void updateECSWithBox2DValues(const World::World &world);

        /**
         * Updates the joints for the world.
         *
         * @param world The world to use.
         * @param timestep The timestep.
         */
        void updateJoints(World::World &world, const Core::Timestep &timestep);

        /**
         * Destroys all joints that are no longer valid.
         *
         * A joint is no longer valid if the corresponding component has a `nullptr` box2d joint, or the connected body no longer exists.
         *
         * @param world The world to use.
         * @param timestep The timestep.
         */
        void destroyInvalidJoints(World::World &world, const Core::Timestep &timestep);

        /**
         * Creates the given joints.
         *
         * @param world The world to use.
         */
        void createJoints(World::World &world);

        /**
         * Destroys the given box2d joint.
         *
         * This will remove the joint from the associated maps.
         *
         * This will also remove the joint component from the owning entity.
         *
         * If the entity does not have that joint type then nothing will happen.
         *
         * @param world The world to use.
         * @param e The entity that owns the joint.
         * @param type The type of joint to destroy.
         * @param removeComponent Wether or not to remove the joint component from the entity.
         */
        void destroyJoint(World::World &world, ECS::Entity e, JointType type, bool removeComponent);

        /**
         * Info for `destroyJoints`.
         */
        struct DestroyJointData
        {
            ECS::Entity e;
            JointType type;
            bool removeComponent;
        };

        /**
         * Destroys the joint given for each entity and type of joint.
         *
         * If the entity does not have that joint type then nothing will happen.
         *
         * @param world The world to use.
         * @param jointsToDestroy The joints to destroy.
         */
        void destroyJoints(World::World &world, const std::vector<DestroyJointData> &jointsToDestroy);

        /**
         * Removes the joint component from the entity in the reigstry for the given joint type.
         *
         * If the entity does not have the joint component nothing will happen.
         *
         * @param world The world to use.
         * @param e The entity to remove the joint from.
         * @param type The type of joint to remove.
         */
        void removeJointComponent(World::World &world, ECS::Entity e, JointType type);

        /**
         * Gets all the joints for the given entity.
         *
         * @param world The world to use.
         * @param e The entity to get the joints for.
         *
         * @returns All the joints for the entity.
         */
        std::unordered_map<JointType, Joint *> getJoints(World::World &world, ECS::Entity e);

        /**
         * Checks if the given entity has the given joint type as a component.
         *
         * @param world The world to use.
         * @param e The entity to check.
         * @param type The type of joint to check for.
         *
         * @returns True if the entity has the joint type, false otherwise.
         */
        bool hasJoint(World::World &world, ECS::Entity e, JointType type);

        /**
         * Gets the joint component for the given entity.
         *
         * @param world The world to use.
         * @param e The entity to get the joint for.
         * @param type The type of joint to get.
         *
         * @returns The joint component for the entity.
         *
         * @throws std::runtime_error if the entity does not have the joint type.
         */
        Physics::Joint *getJoint(World::World &world, ECS::Entity e, JointType type);
    };
}