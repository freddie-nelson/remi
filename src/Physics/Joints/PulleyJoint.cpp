#include "../../../include/Physics/Joints/PulleyJoint.h"

Physics::PulleyJoint::PulleyJoint(ECS::Entity connected, glm::vec2 groundAnchorA, glm::vec2 groundAnchorB, glm::vec2 anchorA, glm::vec2 anchorB, float ratio)
    : Joint(connected, JointType::PULLEY)
{
    setGroundAnchorA(groundAnchorA);
    setGroundAnchorB(groundAnchorB);
    setAnchorA(anchorA);
    setAnchorB(anchorB);
    setRatio(ratio);
}

void Physics::PulleyJoint::setGroundAnchorA(glm::vec2 groundAnchorA)
{
    this->groundAnchorA = groundAnchorA;

    if (joint != nullptr)
    {
        setJoint(nullptr);
    }
}

const glm::vec2 &Physics::PulleyJoint::getGroundAnchorA() const
{
    return groundAnchorA;
}

void Physics::PulleyJoint::setGroundAnchorB(glm::vec2 groundAnchorB)
{
    this->groundAnchorB = groundAnchorB;

    if (joint != nullptr)
    {
        setJoint(nullptr);
    }
}

const glm::vec2 &Physics::PulleyJoint::getGroundAnchorB() const
{
    return groundAnchorB;
}

void Physics::PulleyJoint::setRatio(float ratio)
{
    if (ratio <= 0)
    {
        throw std::invalid_argument("PulleyJoint (setRatio): ratio must be greater than 0");
    }

    this->ratio = ratio;

    if (joint != nullptr)
    {
        setJoint(nullptr);
    }
}

float Physics::PulleyJoint::getRatio() const
{
    return ratio;
}

void Physics::PulleyJoint::setLengthA(float lengthA)
{
    this->lengthA = lengthA;
    lengthASet = true;

    if (joint != nullptr)
    {
        setJoint(nullptr);
    }
}

float Physics::PulleyJoint::getLengthA() const
{
    if (joint != nullptr)
    {
        return joint->GetLengthA();
    }

    return lengthA;
}

void Physics::PulleyJoint::resetLengthA()
{
    lengthASet = false;

    if (joint != nullptr)
    {
        setJoint(nullptr);
    }
}

void Physics::PulleyJoint::setLengthB(float lengthB)
{
    this->lengthB = lengthB;
    lengthBSet = true;

    if (joint != nullptr)
    {
        setJoint(nullptr);
    }
}

float Physics::PulleyJoint::getLengthB() const
{
    if (joint != nullptr)
    {
        return joint->GetLengthB();
    }

    return lengthB;
}

void Physics::PulleyJoint::resetLengthB()
{
    lengthBSet = false;

    if (joint != nullptr)
    {
        setJoint(nullptr);
    }
}

void Physics::PulleyJoint::setJoint(b2Joint *joint)
{
    if (joint == nullptr)
    {
        this->joint = nullptr;
        return;
    }

    if (joint->GetType() != b2JointType::e_pulleyJoint)
    {
        throw std::invalid_argument("PulleyJoint (setJoint): joint must be a pulley joint");
    }

    this->joint = static_cast<b2PulleyJoint *>(joint);
}

b2PulleyJoint *Physics::PulleyJoint::createBox2DJoint(World::World &world, ECS::Entity entity, b2World *box2dWorld, b2Body *owner, b2Body *connected)
{
    b2PulleyJointDef jointDef;
    jointDef.Initialize(owner, connected, b2Vec2(groundAnchorA.x, groundAnchorA.y), b2Vec2(groundAnchorB.x, groundAnchorB.y), b2Vec2(anchorA.x, anchorA.y), b2Vec2(anchorB.x, anchorB.y), ratio);

    if (lengthASet)
    {
        jointDef.lengthA = lengthA;
    }

    if (lengthBSet)
    {
        jointDef.lengthB = lengthB;
    }

    auto userData = new JointUserData{entity, this->connected};
    jointDef.userData.pointer = reinterpret_cast<uintptr_t>(userData);

    auto joint = box2dWorld->CreateJoint(&jointDef);
    if (joint->GetType() != b2JointType::e_pulleyJoint)
    {
        throw std::runtime_error("PulleyJoint (createBox2DJoint): The created joint is not a pulley joint. Something has gone terribly wrong.");
    }

    return reinterpret_cast<b2PulleyJoint *>(joint);
}

b2PulleyJoint *Physics::PulleyJoint::getJoint()
{
    return joint;
}

const b2PulleyJoint *Physics::PulleyJoint::getJoint() const
{
    return joint;
}
