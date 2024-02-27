#include "../../include/Physics/ContactListener.h"
#include "../../include/Physics/RigidBody2D.h"
#include "../../include/Physics/BodyUserData.h"

#include <box2d/b2_contact.h>
#include <stdexcept>

Physics::ContactListener::ContactListener()
{
}

void Physics::ContactListener::setWorld(World::World *world)
{
    this->world = world;
}

void Physics::ContactListener::BeginContact(b2Contact *contact)
{
    auto [contactInfo, ignore] = contactHelper(contact);

    if (ignore)
    {
        return;
    }

    auto &registry = world->getRegistry();

    if (!registry.has<Physics::RigidBody2D>(contactInfo.entityA) || !registry.has<Physics::RigidBody2D>(contactInfo.entityB))
    {
        return;
    }

    auto &rigidBodyA = registry.get<Physics::RigidBody2D>(contactInfo.entityA);
    auto &rigidBodyB = registry.get<Physics::RigidBody2D>(contactInfo.entityB);

    rigidBodyA.beginContact(contactInfo);

    auto contactInfoB = contactInfo;
    contactInfoB.entityA = contactInfo.entityB;
    contactInfoB.entityB = contactInfo.entityA;

    rigidBodyB.beginContact(contactInfoB);
}

void Physics::ContactListener::EndContact(b2Contact *contact)
{
    auto [contactInfo, ignore] = contactHelper(contact);

    if (ignore)
    {
        return;
    }

    auto &registry = world->getRegistry();

    if (!registry.has<Physics::RigidBody2D>(contactInfo.entityA) || !registry.has<Physics::RigidBody2D>(contactInfo.entityB))
    {
        return;
    }

    auto &rigidBodyA = registry.get<Physics::RigidBody2D>(contactInfo.entityA);
    auto &rigidBodyB = registry.get<Physics::RigidBody2D>(contactInfo.entityB);

    rigidBodyA.endContact(contactInfo);

    auto contactInfoB = contactInfo;
    contactInfoB.entityA = contactInfo.entityB;
    contactInfoB.entityB = contactInfo.entityA;

    rigidBodyB.endContact(contactInfoB);
}

Physics::ContactListener::ContactData Physics::ContactListener::contactHelper(b2Contact *contact)
{
    if (world == nullptr)
    {
        throw std::runtime_error("ContactListener (contactHelper): world not set.");
    }

    auto fixtureA = contact->GetFixtureA();
    auto fixtureB = contact->GetFixtureB();

    auto bodyA = fixtureA->GetBody();
    auto bodyB = fixtureB->GetBody();

    if (bodyA == nullptr)
    {
        throw std::runtime_error("ContactListener (BeginContact): bodyA is null.");
    }

    if (bodyB == nullptr)
    {
        throw std::runtime_error("ContactListener (BeginContact): bodyB is null.");
    }

    auto userDataA = reinterpret_cast<BodyUserData *>(bodyA->GetUserData().pointer);
    auto userDataB = reinterpret_cast<BodyUserData *>(bodyB->GetUserData().pointer);

    if (userDataA == nullptr)
    {
        throw std::runtime_error("ContactListener (BeginContact): userDataA is null.");
    }

    if (userDataB == nullptr)
    {
        throw std::runtime_error("ContactListener (BeginContact): userDataB is null.");
    }

    // If either body is a query body, then we don't want to execute the collision callbacks.
    if (userDataA->ignoreCollisionCallbacks || userDataB->ignoreCollisionCallbacks)
    {
        return {ContactInfo{}, true};
    }

    auto entityA = userDataA->entity;
    auto entityB = userDataB->entity;

    ContactInfo contactInfo;
    contactInfo.entityA = entityA;
    contactInfo.entityB = entityB;
    contactInfo.friction = contact->GetFriction();
    contactInfo.restitution = contact->GetRestitution();
    contactInfo.restitutionThreshold = contact->GetRestitutionThreshold();
    contactInfo.tangentSpeed = contact->GetTangentSpeed();

    auto &manifold = *contact->GetManifold();
    contactInfo.normal = glm::vec2(manifold.localNormal.x, manifold.localNormal.y);
    contactInfo.point = glm::vec2(manifold.localPoint.x, manifold.localPoint.y);

    return {std::move(contactInfo), false};
}