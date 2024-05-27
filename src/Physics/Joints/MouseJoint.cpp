#include "../../../include/Physics/Joints/MouseJoint.h"

Physics::MouseJoint::MouseJoint(glm::vec2 target, float maxForce, float stiffness, float damping)
    : Joint(0, JointType::MOUSE)
{
    setTarget(target);
    setMaxForce(maxForce);
    setStiffness(stiffness);
    setDamping(damping);
}

void Physics::MouseJoint::setAnchorA(glm::vec2 anchorA)
{
    throw std::runtime_error("MouseJoint (setAnchorA): Mouse joint does not support setting anchor A.");
}

const glm::vec2 &Physics::MouseJoint::getAnchorA() const
{
    throw std::runtime_error("MouseJoint (getAnchorA): Mouse joint does not support getting anchor A.");
}

void Physics::MouseJoint::setAnchorB(glm::vec2 anchorB)
{
    throw std::runtime_error("MouseJoint (setAnchorB): Mouse joint does not support setting anchor B.");
}

const glm::vec2 &Physics::MouseJoint::getAnchorB() const
{
    throw std::runtime_error("MouseJoint (getAnchorB): Mouse joint does not support getting anchor B.");
}

void Physics::MouseJoint::setConnected(ECS::Entity connected)
{
    throw std::runtime_error("MouseJoint (setConnected): Mouse joint does not support setting connected entity.");
}

ECS::Entity Physics::MouseJoint::getConnected() const
{
    throw std::runtime_error("MouseJoint (getConnected): Mouse joint does not support getting connected entity.");
}

void Physics::MouseJoint::setCollideConnected(bool collideConnected)
{
    throw std::runtime_error("MouseJoint (setCollideConnected): Mouse joint does not support setting collide connected.");
}

bool Physics::MouseJoint::getCollideConnected() const
{
    throw std::runtime_error("MouseJoint (getCollideConnected): Mouse joint does not support getting collide connected.");
}

void Physics::MouseJoint::setTarget(glm::vec2 target)
{
    this->target = target;

    if (joint != nullptr)
    {
        joint->SetTarget(b2Vec2(target.x, target.y));
    }
}

const glm::vec2 &Physics::MouseJoint::getTarget() const
{
    return target;
}

void Physics::MouseJoint::setMaxForce(float maxForce)
{
    this->maxForce = maxForce;

    if (joint != nullptr)
    {
        joint->SetMaxForce(maxForce);
    }
}

float Physics::MouseJoint::getMaxForce() const
{
    return maxForce;
}

void Physics::MouseJoint::setStiffness(float stiffness)
{
    this->stiffness = stiffness;

    if (joint != nullptr)
    {
        joint->SetStiffness(stiffness);
    }
}

float Physics::MouseJoint::getStiffness() const
{
    return stiffness;
}

void Physics::MouseJoint::setDamping(float damping)
{
    this->damping = damping;

    if (joint != nullptr)
    {
        joint->SetDamping(damping);
    }
}

float Physics::MouseJoint::getDamping() const
{
    return damping;
}

void Physics::MouseJoint::setAutoUpdate(bool autoUpdate)
{
    this->autoUpdate = autoUpdate;
}

bool Physics::MouseJoint::getAutoUpdate() const
{
    return autoUpdate;
}

void Physics::MouseJoint::setJoint(b2Joint *joint)
{
    if (joint == nullptr)
    {
        this->joint = nullptr;
        return;
    }

    if (joint->GetType() != b2JointType::e_mouseJoint)
    {
        throw std::invalid_argument("MouseJoint (setJoint): joint must be a mouse joint");
    }

    this->joint = reinterpret_cast<b2MouseJoint *>(joint);
}

b2MouseJoint *Physics::MouseJoint::createBox2DJoint(World::World &world, ECS::Entity entity, b2World *box2dWorld, b2Body *owner, b2Body *connected)
{
    b2MouseJointDef jointDef;
    jointDef.bodyB = owner;
    jointDef.bodyA = connected;
    jointDef.target = b2Vec2(target.x, target.y);
    jointDef.maxForce = getMaxForce();
    jointDef.stiffness = getStiffness();
    jointDef.damping = getDamping();
    jointDef.collideConnected = false;

    auto userData = new JointUserData{entity, this->connected};
    jointDef.userData.pointer = reinterpret_cast<uintptr_t>(userData);

    auto joint = box2dWorld->CreateJoint(&jointDef);
    if (joint->GetType() != b2JointType::e_mouseJoint)
    {
        throw std::runtime_error("MouseJoint (createBox2DJoint): The created joint is not a mouse joint. Something has gone terribly wrong.");
    }

    return reinterpret_cast<b2MouseJoint *>(joint);
}

b2MouseJoint *Physics::MouseJoint::getJoint()
{
    return joint;
}

const b2MouseJoint *Physics::MouseJoint::getJoint() const
{
    return joint;
}