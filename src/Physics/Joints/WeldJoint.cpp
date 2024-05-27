#include "../../../include/Physics/Joints/WeldJoint.h"

Physics::WeldJoint::WeldJoint(ECS::Entity connected, glm::vec2 worldAnchor)
    : Joint(connected, JointType::WELD)
{
    setWorldAnchor(worldAnchor);
}

void Physics::WeldJoint::setAnchorA(glm::vec2 anchorA)
{
    Joint::setAnchorA(anchorA);
    useWorldAnchor = false;
}

void Physics::WeldJoint::setAnchorB(glm::vec2 anchorB)
{
    Joint::setAnchorB(anchorB);
    useWorldAnchor = false;
}

void Physics::WeldJoint::setWorldAnchor(glm::vec2 worldAnchor)
{
    this->worldAnchor = worldAnchor;
    useWorldAnchor = true;

    if (joint != nullptr)
    {
        setJoint(nullptr);
    }
}

const glm::vec2 &Physics::WeldJoint::getWorldAnchor() const
{
    return worldAnchor;
}

void Physics::WeldJoint::setStiffness(float stiffness)
{
    this->stiffness = stiffness;

    if (joint != nullptr)
    {
        joint->SetStiffness(stiffness);
    }
}

float Physics::WeldJoint::getStiffness() const
{
    return stiffness;
}

void Physics::WeldJoint::setDamping(float damping)
{
    this->damping = damping;

    if (joint != nullptr)
    {
        joint->SetDamping(damping);
    }
}

float Physics::WeldJoint::getDamping() const
{
    return damping;
}

void Physics::WeldJoint::setReferenceAngle(float angle)
{
    referenceAngle = angle;
    referenceAngleSet = true;

    if (joint != nullptr)
    {
        setJoint(nullptr);
    }
}

float Physics::WeldJoint::getReferenceAngle() const
{
    if (joint != nullptr)
    {
        return joint->GetReferenceAngle();
    }

    return referenceAngle;
}

void Physics::WeldJoint::setJoint(b2Joint *joint)
{
    if (joint == nullptr)
    {
        this->joint = nullptr;
        return;
    }

    if (joint->GetType() != b2JointType::e_weldJoint)
    {
        throw std::invalid_argument("WeldJoint (setJoint): joint must be a weld joint");
    }

    this->joint = reinterpret_cast<b2WeldJoint *>(joint);
}

b2WeldJoint *Physics::WeldJoint::createBox2DJoint(World::World &world, ECS::Entity entity, b2World *box2dWorld, b2Body *owner, b2Body *connected)
{
    b2WeldJointDef jointDef;
    jointDef.Initialize(owner, connected, b2Vec2(worldAnchor.x, worldAnchor.y));

    jointDef.collideConnected = getCollideConnected();
    jointDef.stiffness = stiffness;
    jointDef.damping = damping;

    if (referenceAngleSet)
    {
        jointDef.referenceAngle = referenceAngle;
    }

    if (!useWorldAnchor)
    {
        jointDef.localAnchorA = b2Vec2(anchorA.x, anchorA.y);
        jointDef.localAnchorB = b2Vec2(anchorB.x, anchorB.y);
    }

    auto userData = new JointUserData{entity, this->connected};
    jointDef.userData.pointer = reinterpret_cast<uintptr_t>(userData);

    auto joint = box2dWorld->CreateJoint(&jointDef);
    if (joint->GetType() != b2JointType::e_weldJoint)
    {
        throw std::runtime_error("WeldJoint (createBox2DJoint): The created joint is not a weld joint. Something has gone terribly wrong.");
    }

    return reinterpret_cast<b2WeldJoint *>(joint);
}

b2WeldJoint *Physics::WeldJoint::getJoint()
{
    return joint;
}

const b2WeldJoint *Physics::WeldJoint::getJoint() const
{
    return joint;
}
