#include "../../../include/Physics/Joints/MotorJoint.h"

Physics::MotorJoint::MotorJoint(ECS::Entity connected)
    : Joint(connected, JointType::MOTOR)
{
}

void Physics::MotorJoint::setLinearOffset(glm::vec2 linearOffset)
{
    this->linearOffset = linearOffset;

    if (joint != nullptr)
    {
        joint->SetLinearOffset({linearOffset.x, linearOffset.y});
    }
}

const glm::vec2 &Physics::MotorJoint::getLinearOffset() const
{
    return linearOffset;
}

void Physics::MotorJoint::setAngularOffset(float angularOffset)
{
    this->angularOffset = angularOffset;

    if (joint != nullptr)
    {
        joint->SetAngularOffset(angularOffset);
    }
}

float Physics::MotorJoint::getAngularOffset() const
{
    return angularOffset;
}

void Physics::MotorJoint::setMaxForce(float maxForce)
{
    this->maxForce = maxForce;

    if (joint != nullptr)
    {
        joint->SetMaxForce(maxForce);
    }
}

float Physics::MotorJoint::getMaxForce() const
{
    return maxForce;
}

void Physics::MotorJoint::setMaxTorque(float maxTorque)
{
    this->maxTorque = maxTorque;

    if (joint != nullptr)
    {
        joint->SetMaxTorque(maxTorque);
    }
}

float Physics::MotorJoint::getMaxTorque() const
{
    return maxTorque;
}

void Physics::MotorJoint::setCorrectionFactor(float correctionFactor)
{
    this->correctionFactor = correctionFactor;

    if (joint != nullptr)
    {
        joint->SetCorrectionFactor(correctionFactor);
    }
}

float Physics::MotorJoint::getCorrectionFactor() const
{
    return correctionFactor;
}

void Physics::MotorJoint::setJoint(b2Joint *joint)
{
    if (joint == nullptr)
    {
        this->joint = nullptr;
        return;
    }

    if (joint->GetType() != b2JointType::e_motorJoint)
    {
        throw std::runtime_error("MotorJoint (setJoint): The joint is not a motor joint. Something has gone terribly wrong.");
    }

    this->joint = reinterpret_cast<b2MotorJoint *>(joint);
}

b2MotorJoint *Physics::MotorJoint::createBox2DJoint(World::World &world, ECS::Entity entity, b2World *box2dWorld, b2Body *owner, b2Body *connected)
{
    b2MotorJointDef jointDef;
    jointDef.Initialize(owner, connected);

    jointDef.collideConnected = getCollideConnected();
    jointDef.linearOffset = {linearOffset.x, linearOffset.y};
    jointDef.angularOffset = angularOffset;
    jointDef.maxForce = maxForce;
    jointDef.maxTorque = maxTorque;
    jointDef.correctionFactor = correctionFactor;

    auto userData = new JointUserData{entity, this->connected};
    jointDef.userData.pointer = reinterpret_cast<uintptr_t>(userData);

    auto joint = box2dWorld->CreateJoint(&jointDef);
    if (joint->GetType() != b2JointType::e_motorJoint)
    {
        throw std::runtime_error("MotorJoint (createBox2DJoint): The created joint is not a motor joint. Something has gone terribly wrong.");
    }

    return reinterpret_cast<b2MotorJoint *>(joint);
}

b2MotorJoint *Physics::MotorJoint::getJoint()
{
    return joint;
}

const b2MotorJoint *Physics::MotorJoint::getJoint() const
{
    return joint;
}