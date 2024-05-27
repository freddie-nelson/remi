#include "../../../include/Physics/Joints/FrictionJoint.h"

Physics::FrictionJoint::FrictionJoint(ECS::Entity connected, glm::vec2 worldAnchor)
    : Joint(connected, JointType::FRICTION)
{
    setWorldAnchor(worldAnchor);
}

void Physics::FrictionJoint::setAnchorA(glm::vec2 anchorA)
{
    Joint::setAnchorA(anchorA);
    useWorldAnchor = false;
}

void Physics::FrictionJoint::setAnchorB(glm::vec2 anchorB)
{
    Joint::setAnchorB(anchorB);
    useWorldAnchor = false;
}

void Physics::FrictionJoint::setWorldAnchor(glm::vec2 worldAnchor)
{
    this->worldAnchor = worldAnchor;
    useWorldAnchor = true;

    if (joint != nullptr)
    {
        setJoint(nullptr);
    }
}

const glm::vec2 &Physics::FrictionJoint::getWorldAnchor() const
{
    return worldAnchor;
}

void Physics::FrictionJoint::setMaxForce(float maxForce)
{
    this->maxForce = maxForce;

    if (joint != nullptr)
    {
        joint->SetMaxForce(maxForce);
    }
}

float Physics::FrictionJoint::getMaxForce() const
{
    return maxForce;
}

void Physics::FrictionJoint::setMaxTorque(float maxTorque)
{
    this->maxTorque = maxTorque;

    if (joint != nullptr)
    {
        joint->SetMaxTorque(maxTorque);
    }
}

float Physics::FrictionJoint::getMaxTorque() const
{
    return maxTorque;
}

void Physics::FrictionJoint::setJoint(b2Joint *joint)
{
    if (joint == nullptr)
    {
        this->joint = nullptr;
        return;
    }

    if (joint->GetType() != b2JointType::e_frictionJoint)
    {
        throw std::invalid_argument("FrictionJoint (setJoint): joint must be a friction joint");
    }

    this->joint = reinterpret_cast<b2FrictionJoint *>(joint);
}

b2FrictionJoint *Physics::FrictionJoint::createBox2DJoint(World::World &world, ECS::Entity entity, b2World *box2dWorld, b2Body *owner, b2Body *connected)
{
    b2FrictionJointDef jointDef;
    jointDef.Initialize(connected, owner, b2Vec2(worldAnchor.x, worldAnchor.y));
    jointDef.collideConnected = getCollideConnected();

    if (!useWorldAnchor)
    {
        jointDef.localAnchorA = b2Vec2(anchorA.x, anchorA.y);
        jointDef.localAnchorB = b2Vec2(anchorB.x, anchorB.y);
    }

    auto userData = new JointUserData{entity, this->connected};
    jointDef.userData.pointer = reinterpret_cast<uintptr_t>(userData);

    auto joint = box2dWorld->CreateJoint(&jointDef);
    if (joint->GetType() != b2JointType::e_frictionJoint)
    {
        throw std::runtime_error("FrictionJoint (createBox2DJoint): The created joint is not a friction joint. Something has gone terribly wrong.");
    }

    return reinterpret_cast<b2FrictionJoint *>(joint);
}

b2FrictionJoint *Physics::FrictionJoint::getJoint()
{
    return joint;
}

const b2FrictionJoint *Physics::FrictionJoint::getJoint() const
{
    return joint;
}
