#include "../../include/Physics/PhysicsWorld.h"
#include "../../include/Physics/RigidBody2D.h"
#include "../../include/Physics/Collider2D.h"
#include "../../include/Core/Transform.h"

#include <box2d/b2_body.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_fixture.h>
#include <unordered_set>
#include <string>

Physics::PhysicsWorld::PhysicsWorld(PhysicsWorldConfig config, const Core::SpaceTransformer *spaceTransformer) : spaceTransformer(spaceTransformer), world(b2Vec2(config.gravity.x, config.gravity.y))
{
    setConfig(config);
}

Physics::PhysicsWorld::~PhysicsWorld()
{
    // TODO: implement
}

void Physics::PhysicsWorld::fixedUpdate(World::World &world, const Core::Timestep &timestep)
{
    // create, destroy and update bodies with ECS values
    updateBodies(world);

    // step box2d world
    this->world.Step(timestep.getSeconds(), config.velocityIterations, config.positionIterations);

    // update ECS values with box2d values
    updateECSWithBox2DValues(world);
}

std::vector<Physics::RaycastHit> Physics::PhysicsWorld::raycast(const Ray &ray, RaycastType type)
{
    RaycastCallback callback(ray, type, bodyToEntity);

    b2Vec2 p1(ray.start.x, ray.start.y);
    b2Vec2 p2(ray.end.x, ray.end.y);

    world.RayCast(&callback, p1, p2);

    return callback.getHits();
}

void Physics::PhysicsWorld::setConfig(PhysicsWorldConfig config)
{
    this->config = config;
    setGravity(config.gravity);
    setVelocityIterations(config.velocityIterations);
    setPositionIterations(config.positionIterations);
}

const Physics::PhysicsWorldConfig &Physics::PhysicsWorld::getConfig()
{
    return config;
}

void Physics::PhysicsWorld::setGravity(glm::vec2 gravity)
{
    config.gravity = gravity;

    b2Vec2 g(gravity.x, gravity.y);
    world.SetGravity(g);
}

glm::vec2 Physics::PhysicsWorld::getGravity()
{
    return config.gravity;
}

void Physics::PhysicsWorld::setVelocityIterations(int iterations)
{
    if (iterations < 1)
        throw std::runtime_error("PhysicsWorld (setVelocityIterations): Velocity iterations must be at least 1.");

    config.velocityIterations = iterations;
}

int Physics::PhysicsWorld::getVelocityIterations()
{
    return config.velocityIterations;
}

void Physics::PhysicsWorld::setPositionIterations(int iterations)
{
    if (iterations < 1)
        throw std::runtime_error("PhysicsWorld (setPositionIterations): Position iterations must be at least 1.");

    config.positionIterations = iterations;
}

int Physics::PhysicsWorld::getPositionIterations()
{
    return config.positionIterations;
}

const std::unordered_map<ECS::Entity, b2Body *> &Physics::PhysicsWorld::getBodies() const
{
    return bodies;
}

const std::unordered_map<ECS::Entity, b2Fixture *> &Physics::PhysicsWorld::getColliders() const
{
    return colliders;
}

void Physics::PhysicsWorld::updateBodies(const World::World &world)
{
    auto &registry = world.getRegistry();

    auto entities = registry.view<Core::Transform, Physics::RigidBody2D>();
    std::unordered_set<ECS::Entity> entitySet(entities.begin(), entities.end());

    // check if any bodies need to be removed or added
    for (auto it = bodies.begin(); it != bodies.end();)
    {
        auto &entity = it->first;

        // entity is not in the registry or no longer a valid body
        if (!entitySet.contains(entity))
        {
            bodyToEntity.erase(it->second);
            it = bodies.erase(it);
        }
        // remove the entity from the set if it is still a valid body
        else
        {
            entitySet.erase(entity);
            ++it;
        }
    }

    // now all bodies in the set are new bodies
    // so add them to world
    createBodies(world, entitySet);

    // inject user values into box2d bodies
    updateBodiesWithECSValues(world, entitySet);
}

void Physics::PhysicsWorld::createBodies(const World::World &world, const std::unordered_set<ECS::Entity> &entitySet)
{
    auto &registry = world.getRegistry();
    auto &sceneGraph = world.getSceneGraph();

    b2BodyDef bodyDef;

    for (auto &e : entitySet)
    {
        if (bodies.contains(e))
        {
            continue;
        }

        // add body
        auto transform = Core::Transform(sceneGraph.getModelMatrix(e));
        auto &body = registry.get<Physics::RigidBody2D>(e);

        // set body def
        bodyDef.position.Set(transform.getTranslation().x, transform.getTranslation().y);
        bodyDef.angle = transform.getRotation();

        bodyDef.type = static_cast<b2BodyType>(body.type);

        bodyDef.linearVelocity.x = body.velocity.x;
        bodyDef.linearVelocity.y = body.velocity.y;

        bodyDef.angularVelocity = body.angularVelocity;

        bodyDef.linearDamping = body.linearDamping;
        bodyDef.angularDamping = body.angularDamping;

        bodyDef.allowSleep = body.allowSleep;
        bodyDef.awake = body.isAwake;

        bodyDef.fixedRotation = body.fixedRotation;

        bodyDef.bullet = body.isBullet;

        bodyDef.enabled = body.isEnabled;

        bodyDef.gravityScale = body.gravityScale;

        // create body
        bodies[e] = this->world.CreateBody(&bodyDef);
        bodyToEntity[bodies[e]] = e;

        // create collider
        if (registry.has<Physics::Collider2D>(e))
        {
            createBox2DCollider(world, e);
        }
    }
}

void Physics::PhysicsWorld::updateBodiesWithECSValues(const World::World &world, const std::unordered_set<ECS::Entity> &createdEntities)
{
    auto &registry = world.getRegistry();
    auto &sceneGraph = world.getSceneGraph();

    for (auto &[e, box2dBody] : bodies)
    {
        // skip created entities as they don't need updated
        if (createdEntities.contains(e))
        {
            continue;
        }

        auto transform = Core::Transform(sceneGraph.getModelMatrix(e));
        auto &body = registry.get<Physics::RigidBody2D>(e);

        // update transform
        auto &box2dPos = box2dBody->GetPosition();
        auto &translation = transform.getTranslation();
        auto rotation = transform.getRotation();

        if (box2dPos.x != translation.x || box2dPos.y != translation.y || box2dBody->GetAngle() != rotation)
        {
            box2dBody->SetTransform(b2Vec2(translation.x, translation.y), rotation);
        }

        // update velocity
        auto &box2dVel = box2dBody->GetLinearVelocity();

        if (box2dVel.x != body.velocity.x || box2dVel.y != body.velocity.y)
        {
            box2dBody->SetLinearVelocity(b2Vec2(body.velocity.x, body.velocity.y));
        }

        // update angular velocity
        box2dBody->SetAngularVelocity(body.angularVelocity);

        // update damping
        box2dBody->SetLinearDamping(body.linearDamping);
        box2dBody->SetAngularDamping(body.angularDamping);

        // update sleep
        box2dBody->SetSleepingAllowed(body.allowSleep);

        // update awake
        box2dBody->SetAwake(body.isAwake);

        // update fixed rotation
        box2dBody->SetFixedRotation(body.fixedRotation);

        // update bullet
        box2dBody->SetBullet(body.isBullet);

        // update enabled
        box2dBody->SetEnabled(body.isEnabled);

        // update gravity scale
        box2dBody->SetGravityScale(body.gravityScale);

        // update collider
        if (registry.has<Physics::Collider2D>(e))
        {
            auto &collider = registry.get<Physics::Collider2D>(e);

            if (!colliders.contains(e))
            {
                createBox2DCollider(world, e);
            }

            // update shape
            if (collider.isShapeDirty())
            {
                // collider needs completely re-constructed
                destroyBox2DCollider(e);
                createBox2DCollider(world, e);

                collider.informOfShapeUpdate();
            }

            // update fixture
            auto &fixture = colliders[e];

            fixture->SetDensity(collider.density);
            fixture->SetFriction(collider.friction);
            fixture->SetRestitution(collider.restitution);
            fixture->SetRestitutionThreshold(collider.restitutionThreshold);
            fixture->SetSensor(collider.isSensor);
        }
        // collider needs removed from body
        else if (colliders.contains(e))
        {
            destroyBox2DCollider(e);
        }
    }
}

void Physics::PhysicsWorld::createBox2DCollider(const World::World &world, ECS::Entity e)
{
    if (!bodies.contains(e))
    {
        throw std::runtime_error("PhysicsWorld (createBox2DCollider): Entity '" + std::to_string(e) + "' has no body.");
    }

    if (colliders.contains(e))
    {
        throw std::runtime_error("PhysicsWorld (createBox2DCollider): Entity '" + std::to_string(e) + "' already has a collider.");
    }

    auto &registry = world.getRegistry();
    auto &collider = registry.get<Physics::Collider2D>(e);

    b2Shape *shape = collider.getShape()->createBox2DShape();

    // create fixture
    b2FixtureDef fixtureDef;
    fixtureDef.shape = shape;
    fixtureDef.density = collider.density;
    fixtureDef.friction = collider.friction;
    fixtureDef.restitution = collider.restitution;
    fixtureDef.restitutionThreshold = collider.restitutionThreshold;
    fixtureDef.isSensor = collider.isSensor;

    // add fixture to body
    colliders[e] = bodies[e]->CreateFixture(&fixtureDef);
}

void Physics::PhysicsWorld::destroyBox2DCollider(ECS::Entity e)
{
    if (!colliders.contains(e))
    {
        throw std::runtime_error("PhysicsWorld (destroyBox2DCollider): Entity '" + std::to_string(e) + "' has no collider.");
    }

    bodies[e]->DestroyFixture(colliders[e]);
    colliders.erase(e);
}

void Physics::PhysicsWorld::updateECSWithBox2DValues(const World::World &world)
{
    auto &registry = world.getRegistry();
    auto &sceneGraph = world.getSceneGraph();

    for (auto &[e, box2dBody] : bodies)
    {
        auto &localTransform = registry.get<Core::Transform>(e);
        auto worldTransform = Core::Transform(sceneGraph.getModelMatrix(e));

        auto &body = registry.get<Physics::RigidBody2D>(e);

        // create box2d transform
        auto &box2dPos = box2dBody->GetPosition();
        auto box2dRotation = box2dBody->GetAngle();

        auto &translation = worldTransform.getTranslation();
        auto rotation = worldTransform.getRotation();

        if (box2dPos.x != translation.x || box2dPos.y != translation.y || box2dRotation != rotation)
        {
            auto localTranslation = spaceTransformer->transform(glm::vec2(box2dPos.x, box2dPos.y), e, Core::SpaceTransformer::Space::WORLD, Core::SpaceTransformer::Space::LOCAL);
            localTransform.setTranslation(localTranslation);

            auto localRotation = spaceTransformer->transformWorldRotationToLocal(box2dRotation, e);
            localTransform.setRotation(localRotation);
            // if (sceneGraph.hasParent(e))
            // {
            //     auto localTranslation = spaceTransformer->transform(glm::vec2(box2dPos.x, box2dPos.y), e, Core::SpaceTransformer::Space::WORLD, Core::SpaceTransformer::Space::LOCAL);
            //     localTransform.setTranslation(localTranslation);

            //     auto localRotation = spaceTransformer->transformWorldRotationToLocal(box2dRotation, e);
            //     localTransform.setRotation(localRotation);
            // }
            // else
            // {
            //     localTransform.setTranslation(glm::vec2(box2dPos.x, box2dPos.y));
            //     localTransform.setRotation(box2dRotation);
            // }
        }

        // update velocity
        auto &box2dVel = box2dBody->GetLinearVelocity();
        auto box2dAngVel = box2dBody->GetAngularVelocity();

        if (box2dVel.x != body.velocity.x || box2dVel.y != body.velocity.y)
        {
            body.velocity = glm::vec2(box2dVel.x, box2dVel.y);
        }

        // update angular velocity
        body.angularVelocity = box2dAngVel;
    }
}