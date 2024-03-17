#include "../../include/Physics/PhysicsWorld.h"
#include "../../include/Physics/RigidBody2D.h"
#include "../../include/Physics/Collider2D.h"
#include "../../include/Core/Transform.h"
#include "../../include/Physics/QueryCallback.h"
#include "../../include/Physics/ColliderShape.h"
#include "../../include/Physics/BodyUserData.h"
#include "../../include/Physics/Joints/DistanceJoint.h"
#include "../../include/Physics/Joints/RevoluteJoint.h"

#include <box2d/b2_body.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_circle_shape.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_contact.h>
#include <cstdint>
#include <unordered_set>
#include <string>
#include <stdexcept>

Physics::PhysicsWorld::PhysicsWorld(PhysicsWorldConfig config, const World::World *entityWorld, const Core::SpaceTransformer *spaceTransformer) : entityWorld(entityWorld), spaceTransformer(spaceTransformer), world(b2Vec2(config.gravity.x, config.gravity.y))
{
    setConfig(config);

    world.SetContactListener(&contactListener);
}

Physics::PhysicsWorld::~PhysicsWorld()
{
    // TODO: implement
}

void Physics::PhysicsWorld::fixedUpdate(World::World &world, const Core::Timestep &timestep)
{
    // update contact listener world
    contactListener.setWorld(&world);

    // create, destroy and update bodies with ECS values
    updateBodies(world);

    // create, destroy and update joints with ECS values
    updateJoints(world, timestep);

    // step box2d world
    this->world.Step(timestep.getSeconds(), config.velocityIterations, config.positionIterations);

    // update ECS values with box2d values
    updateECSWithBox2DValues(world);
}

std::vector<Physics::RaycastHit> Physics::PhysicsWorld::raycast(const Ray &ray, RaycastType type)
{
    RaycastCallback callback(*entityWorld, ray, type, bodyToEntity);

    b2Vec2 p1(ray.start.x, ray.start.y);
    b2Vec2 p2(ray.end.x, ray.end.y);

    world.RayCast(&callback, p1, p2);

    return callback.getHits();
}

std::vector<ECS::Entity> Physics::PhysicsWorld::query(const Core::AABB &aabb)
{
    QueryCallback callback(*entityWorld, aabb, bodyToEntity);

    b2AABB box;
    box.lowerBound = b2Vec2(aabb.getMin().x, aabb.getMin().y);
    box.upperBound = b2Vec2(aabb.getMax().x, aabb.getMax().y);

    world.QueryAABB(&callback, box);

    return callback.getResults();
}

std::vector<ECS::Entity> Physics::PhysicsWorld::query(const Core::BoundingCircle &circle)
{
    const Core::AABB aabb(circle.getCentre(), circle.getRadius());

    QueryCallback callback(*entityWorld, aabb, bodyToEntity);

    b2AABB box;
    box.lowerBound = b2Vec2(aabb.getMin().x, aabb.getMin().y);
    box.upperBound = b2Vec2(aabb.getMax().x, aabb.getMax().y);

    world.QueryAABB(&callback, box);
    auto &results = callback.getResults();

    b2CircleShape shape;
    shape.m_radius = circle.getRadius();
    shape.m_p = b2Vec2(circle.getCentre().x, circle.getCentre().y);

    b2BodyDef bodyDef;

    // needs to be dynamic to collide with static bodies too
    bodyDef.type = b2_dynamicBody;

    auto userData = new BodyUserData(0, true);
    bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(userData);

    b2Body *circleBody = world.CreateBody(&bodyDef);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.isSensor = true;
    fixtureDef.density = 0.0f;

    circleBody->CreateFixture(&fixtureDef);

    // step world by 0 to get the results
    world.Step(0, 0, 0);

    std::vector<ECS::Entity> circleResults;

    for (auto &e : results)
    {
        if (!bodies.contains(e))
        {
            throw std::runtime_error("PhysicsWorld (query): Entity '" + std::to_string(e) + "' has no body.");
        }

        auto body = bodies[e];
        auto contacts = body->GetContactList();

        for (auto contact = contacts; contact != nullptr; contact = contact->next)
        {
            if (contact->other == circleBody && contact->contact->IsTouching())
            {
                circleResults.push_back(e);
                break;
            }
        }
    }

    world.DestroyBody(circleBody);
    delete userData;

    return circleResults;
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

const std::unordered_map<ECS::Entity, std::vector<b2Fixture *>> &Physics::PhysicsWorld::getColliders() const
{
    return colliders;
}

void Physics::PhysicsWorld::updateBodies(World::World &world)
{
    auto &registry = world.getRegistry();

    auto &entities = registry.view<Core::Transform, Physics::RigidBody2D>();
    std::unordered_set<ECS::Entity> entitySet(entities.begin(), entities.end());

    std::vector<b2Body *> bodiesToDestroy;

    // check if any bodies need to be removed or added
    for (auto &[entity, body] : bodies)
    {
        // entity is not in the registry or no longer a valid body
        if (!entitySet.contains(entity))
        {
            bodiesToDestroy.push_back(body);
        }
        // remove the entity from the set if it is still a valid body
        // it does not need to be created
        else
        {
            entitySet.erase(entity);
        }
    }

    // destroy bodies
    for (auto &body : bodiesToDestroy)
    {
        destroyBody(world, body);
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

        bodyDef.type = static_cast<b2BodyType>(body.getType());

        bodyDef.linearVelocity.x = body.getVelocity().x;
        bodyDef.linearVelocity.y = body.getVelocity().y;

        bodyDef.angularVelocity = body.getAngularVelocity();

        bodyDef.linearDamping = body.getLinearDamping();
        bodyDef.angularDamping = body.getAngularDamping();

        bodyDef.allowSleep = body.getAllowSleep();
        bodyDef.awake = body.getIsAwake();

        bodyDef.fixedRotation = body.getFixedRotation();

        bodyDef.bullet = body.getIsBullet();

        bodyDef.enabled = body.getIsEnabled();

        bodyDef.gravityScale = body.getGravityScale();

        bodyDef.userData.pointer = reinterpret_cast<uintptr_t>(new BodyUserData(e));

        // create body
        bodies[e] = this->world.CreateBody(&bodyDef);
        bodyToEntity[bodies[e]] = e;

        body.setBody(bodies[e]);

        // create collider
        if (registry.has<Physics::Collider2D>(e))
        {
            createBox2DCollider(world, e);
        }
    }
}

void Physics::PhysicsWorld::destroyBody(World::World &world, b2Body *body)
{
    auto data = reinterpret_cast<BodyUserData *>(body->GetUserData().pointer);
    auto entity = data->entity;

    // destroy bodies joints
    if (joints.contains(entity))
    {
        auto &entityJoints = joints[entity];

        for (auto &[type, joint] : entityJoints)
        {
            destroyJoint(world, entity, type, false);
        }
    }

    // destroy collider
    if (colliders.contains(entity))
    {
        destroyBox2DCollider(entity);
    }

    // remove from maps
    bodies.erase(bodyToEntity[body]);
    bodyToEntity.erase(body);

    // delete user data
    delete data;

    // destroy body
    this->world.DestroyBody(body);
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

        // update collider
        if (registry.has<Physics::Collider2D>(e))
        {
            auto &collider = registry.get<Physics::Collider2D>(e);

            // if (collider.getShape()->getType() > Physics::ColliderShapeType::CHAIN)
            // {
            //     std::cout << "collider: " << e << std::endl;
            //     std::cout << "collider shape type: " << collider.getShape()->getType() << std::endl;
            //     std::cout << "density: " << collider.getDensity() << std::endl;
            //     std::cout << "friction: " << collider.getFriction() << std::endl;
            //     std::cout << "restitution: " << collider.getRestitution() << std::endl;
            //     std::cout << "restitution threshold: " << collider.getRestitutionThreshold() << std::endl;
            //     std::cout << "is sensor: " << collider.getIsSensor() << std::endl;

            //     throw std::runtime_error("PhysicsWorld (updateBodiesWithECSValues): Collider shape type not implemented.");
            // }

            // collider needs recreated
            if (collider.getFixtures() == nullptr)
            {
                destroyBox2DCollider(e);
                createBox2DCollider(world, e);
            }
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

    auto colliderShape = collider.getShape();

    std::vector<b2Fixture *> fixtures;

    // create fixture
    if (colliderShape->getType() == Physics::ColliderShapeType::CONCAVE_POLYGON)
    {
        auto concaveShape = reinterpret_cast<const Physics::ConcavePolygonColliderShape2D *>(colliderShape);
        auto shapeCount = concaveShape->getShapeCount();
        auto shapes = concaveShape->createBox2DShape();

        for (size_t i = 0; i < shapeCount; i++)
        {
            b2FixtureDef fixtureDef;
            fixtureDef.shape = &shapes[i];
            fixtureDef.density = collider.getDensity();
            fixtureDef.friction = collider.getFriction();
            fixtureDef.restitution = collider.getRestitution();
            fixtureDef.restitutionThreshold = collider.getRestitutionThreshold();
            fixtureDef.isSensor = collider.getIsSensor();

            fixtures.push_back(bodies[e]->CreateFixture(&fixtureDef));
        }

        delete[] shapes;
    }
    else
    {
        b2Shape *shape = colliderShape->createBox2DShape();

        b2FixtureDef fixtureDef;
        fixtureDef.shape = shape;
        fixtureDef.density = collider.getDensity();
        fixtureDef.friction = collider.getFriction();
        fixtureDef.restitution = collider.getRestitution();
        fixtureDef.restitutionThreshold = collider.getRestitutionThreshold();
        fixtureDef.isSensor = collider.getIsSensor();

        fixtures.push_back(bodies[e]->CreateFixture(&fixtureDef));

        delete shape;
    }

    colliders.insert_or_assign(e, std::move(fixtures));
    collider.setFixtures(&colliders[e]);
}

void Physics::PhysicsWorld::destroyBox2DCollider(ECS::Entity e)
{
    if (!colliders.contains(e))
    {
        throw std::runtime_error("PhysicsWorld (destroyBox2DCollider): Entity '" + std::to_string(e) + "' has no collider.");
    }

    for (auto &fixture : colliders[e])
    {
        bodies[e]->DestroyFixture(fixture);
    }

    colliders.erase(e);
}

void Physics::PhysicsWorld::updateECSWithBox2DValues(const World::World &world)
{
    auto &registry = world.getRegistry();
    auto &sceneGraph = world.getSceneGraph();

    for (auto &[e, box2dBody] : bodies)
    {
        // user code could have removed the transform or rigid body
        // these bodies will be destroyed in the next update
        if (!registry.has<Core::Transform>(e) || !registry.has<Physics::RigidBody2D>(e))
        {
            continue;
        }

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
        }

        // update velocity
        auto &box2dVel = box2dBody->GetLinearVelocity();
        auto box2dAngVel = box2dBody->GetAngularVelocity();

        if (box2dVel.x != body.getVelocity().x || box2dVel.y != body.getVelocity().y)
        {
            body.setVelocity(glm::vec2(box2dVel.x, box2dVel.y));
        }

        // update angular velocity
        body.setAngularVelocity(box2dAngVel);

        // update awake
        body.setIsAwake(box2dBody->IsAwake());
    }
}

void Physics::PhysicsWorld::updateJoints(World::World &world, const Core::Timestep &timestep)
{
    auto &registry = world.getRegistry();
    auto &sceneGraph = world.getSceneGraph();

    destroyInvalidJoints(world, timestep);
    createJoints(world);
}

void Physics::PhysicsWorld::destroyInvalidJoints(World::World &world, const Core::Timestep &timestep)
{
    std::vector<DestroyJointData> jointsToDestroy;

    // destroy joints that are no longer valid
    // or need recreated
    for (auto &[e, body] : bodies)
    {
        // joints have not been created for the body yet so no
        // need to destroy any
        if (!joints.contains(e) || joints[e].size() == 0)
        {
            continue;
        }

        auto &createdJoints = joints[e];
        auto entityJoints = getJoints(world, e);

        for (auto &[type, box2dJoint] : createdJoints)
        {
            // entity no longer has joint component so destroy joint
            if (!entityJoints.contains(type))
            {
                jointsToDestroy.push_back({e, type, false});
                continue;
            }

            auto &entityJoint = entityJoints[type];

            // if statement could just be or but looks more readable this way (compiler will optimise away anyway)
            // need to destroy joint so it can be recreated
            if (entityJoint->getJoint() == nullptr)
            {
                jointsToDestroy.push_back({e, type, false});
            }
            // need to destroy joint as connected body no longer exists
            else if (!bodies.contains(entityJoint->getConnected()))
            {
                jointsToDestroy.push_back({e, type, true});
            }
            // joint is past breaking force and so must be destroyed
            else if (entityJoint->getBreakForce() != 0.0f && glm::length(entityJoint->getReactionForce(timestep)) > entityJoint->getBreakForce())
            {
                jointsToDestroy.push_back({e, type, true});
            }
            // joitn is past breaking torque so must be destroyed
            else if (entityJoint->getBreakTorque() && entityJoint->getReactionTorque(timestep) > entityJoint->getBreakTorque())
            {
                jointsToDestroy.push_back({e, type, true});
            }
        }
    }

    destroyJoints(world, jointsToDestroy);
}

void Physics::PhysicsWorld::createJoints(World::World &world)
{
    for (auto &[e, body] : bodies)
    {
        auto entityJoints = getJoints(world, e);

        for (auto &[type, entityJoint] : entityJoints)
        {
            auto joint = entityJoint->getJoint();

            // doesn't need created
            if (joint != nullptr)
            {
                continue;
            }

            // check if joint already exists
            if (joints.contains(e) && joints[e].contains(type))
            {
                throw std::runtime_error("PhysicsWorld: (createJoints): Joint already exists for entity '" + std::to_string(e) + "'. Should never call `createJoints` before `destroyInvalidJoints`.");
            }

            // check if connected entity is a valid body
            auto connected = entityJoint->getConnected();
            if (!bodies.contains(connected))
            {
                throw std::runtime_error("PhysicsWorld (createJoints): Connected entity on joint for entity '" + std::to_string(e) + "' is not a valid body. Should never call `createJoints` before `destroyInvalidJoints`.");
            }

            // create joints map entry if needed
            if (!joints.contains(e))
            {
                joints.emplace(e, std::unordered_map<JointType, b2Joint *>());
            }

            // create joint
            joints[e][type] = entityJoint->createBox2DJoint(world, e, &this->world, body, bodies[connected]);
            entityJoint->setJoint(joints[e][type]);
        }
    }
}

void Physics::PhysicsWorld::destroyJoint(World::World &world, ECS::Entity e, JointType type, bool removeComponent)
{
    auto &registry = world.getRegistry();

    // joint doesn't exist
    if (!joints.contains(e) || !joints[e].contains(type))
    {
        return;
    }

    auto &joint = joints[e][type];
    auto userData = reinterpret_cast<JointUserData *>(joint->GetUserData().pointer);

    // nullify joint on component
    getJoint(world, e, type)->setJoint(nullptr);

    // connected body does not exist so joint is invalid
    // we must remove the joint component from the ecs
    if (removeComponent && hasJoint(world, e, type))
    {
        removeJointComponent(world, e, type);
    }

    // delete user data
    delete userData;

    // destroy joint
    this->world.DestroyJoint(joint);

    // remove from joints map
    joints[e].erase(type);
}

void Physics::PhysicsWorld::destroyJoints(World::World &world, const std::vector<DestroyJointData> &jointsToDestroy)
{
    for (auto &[e, type, removeComponent] : jointsToDestroy)
    {
        destroyJoint(world, e, type, removeComponent);
    }
}

void Physics::PhysicsWorld::removeJointComponent(World::World &world, ECS::Entity e, JointType type)
{
    auto &registry = world.getRegistry();

    if (!hasJoint(world, e, type))
    {
        return;
    }

    switch (type)
    {
    case JointType::DISTANCE:
        registry.remove<Physics::DistanceJoint>(e);
        break;
    case JointType::REVOLUTE:
        registry.remove<Physics::RevoluteJoint>(e);
        break;

    default:
        throw std::runtime_error("PhysicsWorld (removeJointComponent): JointType not implemented.");
    }
}

std::unordered_map<Physics::JointType, Physics::Joint *> Physics::PhysicsWorld::getJoints(World::World &world, ECS::Entity e)
{
    auto &registry = world.getRegistry();

    std::unordered_map<JointType, Joint *> joints;

    if (registry.has<DistanceJoint>(e))
    {
        joints.emplace(JointType::DISTANCE, &registry.get<DistanceJoint>(e));
    }

    if (registry.has<RevoluteJoint>(e))
    {
        joints.emplace(JointType::REVOLUTE, &registry.get<RevoluteJoint>(e));
    }

    return joints;
}

bool Physics::PhysicsWorld::hasJoint(World::World &world, ECS::Entity e, JointType type)
{
    auto &registry = world.getRegistry();

    switch (type)
    {
    case JointType::DISTANCE:
        return registry.has<Physics::DistanceJoint>(e);
    case JointType::REVOLUTE:
        return registry.has<Physics::RevoluteJoint>(e);
    default:
        throw std::invalid_argument("PhysicsWorld (hasJoint): JointType not implemented.");
    }

    return false;
}

Physics::Joint *Physics::PhysicsWorld::getJoint(World::World &world, ECS::Entity e, JointType type)
{
    auto &registry = world.getRegistry();

    switch (type)
    {
    case JointType::DISTANCE:
        return &registry.get<Physics::DistanceJoint>(e);
    case JointType::REVOLUTE:
        return &registry.get<Physics::RevoluteJoint>(e);
    default:
        throw std::invalid_argument("PhysicsWorld (getJoint): JointType not implemented.");
    }

    return nullptr;
}