#include "../../../include/Physics/Joints/GearJoint.h"

Physics::GearJoint::GearJoint(ECS::Entity connected, Joint *jointA, Joint *jointB, float ratio)
    : Joint(connected, JointType::GEAR)
{
    setJointA(jointA);
    setJointB(jointB);
    setRatio(ratio);
}

void Physics::GearJoint::setRatio(float ratio)
{
    if (ratio == 0)
    {
        throw std::invalid_argument("GearJoint (setRatio): ratio cannot be 0");
    }

    this->ratio = ratio;

    if (joint != nullptr)
    {
        joint->SetRatio(ratio);
    }
}

float Physics::GearJoint::getRatio() const
{
    return ratio;
}

void Physics::GearJoint::setJointA(Joint *jointA)
{
    if (jointA == nullptr)
    {
        throw std::invalid_argument("GearJoint (setJointA): jointA cannot be null");
    }

    if (jointA->getType() != JointType::REVOLUTE && jointA->getType() != JointType::PRISMATIC)
    {
        throw std::invalid_argument("GearJoint (setJointA): jointA must be a revolute or prismatic joint");
    }

    this->jointA = jointA;

    if (joint != nullptr)
    {
        setJoint(nullptr);
    }
}

Physics::Joint *Physics::GearJoint::getJointA()
{
    return jointA;
}

void Physics::GearJoint::setJointB(Joint *jointB)
{
    if (jointB == nullptr)
    {
        throw std::invalid_argument("GearJoint (setJointB): jointB cannot be null");
    }

    if (jointB->getType() != JointType::REVOLUTE && jointB->getType() != JointType::PRISMATIC)
    {
        throw std::invalid_argument("GearJoint (setJointB): jointB must be a revolute or prismatic joint");
    }

    this->jointB = jointB;

    if (joint != nullptr)
    {
        setJoint(nullptr);
    }
}

Physics::Joint *Physics::GearJoint::getJointB()
{
    return jointB;
}

void Physics::GearJoint::setJoint(b2Joint *joint)
{
    if (joint == nullptr)
    {
        this->joint = nullptr;
        return;
    }

    if (joint->GetType() != b2JointType::e_gearJoint)
    {
        throw std::invalid_argument("GearJoint (setJoint): joint must be a gear joint");
    }

    this->joint = static_cast<b2GearJoint *>(joint);
}

b2GearJoint *Physics::GearJoint::createBox2DJoint(World::World &world, ECS::Entity entity, b2World *box2dWorld, b2Body *owner, b2Body *connected)
{
    b2GearJointDef jointDef;
    jointDef.bodyA = owner;
    jointDef.bodyB = connected;
    jointDef.ratio = ratio;

    if (jointA->getJoint() == nullptr)
    {
        throw std::invalid_argument("GearJoint (createBox2DJoint): jointA has not been created yet.");
    }

    if (jointB->getJoint() == nullptr)
    {
        throw std::invalid_argument("GearJoint (createBox2DJoint): jointB has not been created yet.");
    }

    jointDef.joint1 = jointA->getJoint();
    jointDef.joint2 = jointB->getJoint();

    if (jointA->getJoint()->GetBodyB()->GetType() != b2BodyType::b2_dynamicBody)
    {
        throw std::invalid_argument("GearJoint (createBox2DJoint): jointA must be connected to a dynamic body.");
    }

    if (jointB->getJoint()->GetBodyB()->GetType() != b2BodyType::b2_dynamicBody)
    {
        throw std::invalid_argument("GearJoint (createBox2DJoint): jointB must be connected to a dynamic body.");
    }

    auto userData = new JointUserData{entity, this->connected};
    jointDef.userData.pointer = reinterpret_cast<uintptr_t>(userData);

    auto joint = box2dWorld->CreateJoint(&jointDef);
    if (joint->GetType() != b2JointType::e_gearJoint)
    {
        throw std::runtime_error("GearJoint (createBox2DJoint): The created joint is not a gear joint. Something has gone terribly wrong.");
    }

    return reinterpret_cast<b2GearJoint *>(joint);
}

b2GearJoint *Physics::GearJoint::getJoint()
{
    return joint;
}

const b2GearJoint *Physics::GearJoint::getJoint() const
{
    return joint;
}
