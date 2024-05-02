#include "../../../include/Physics/Joints/WheelJoint.h"

Physics::WheelJoint::WheelJoint(ECS::Entity connected, glm::vec2 anchor, glm::vec2 axis)
    : Joint(connected, JointType::WHEEL), anchor(anchor), axis(axis)
{
}

void Physics::WheelJoint::setAnchor(glm::vec2 anchor)
{
    this->anchor = anchor;
    joint = nullptr;
}

const glm::vec2 &Physics::WheelJoint::getAnchor() const
{
    return anchor;
}

void Physics::WheelJoint::setAxis(glm::vec2 axis)
{
    this->axis = axis;
    joint = nullptr;
}

const glm::vec2 &Physics::WheelJoint::getAxis() const
{
    return axis;
}

void Physics::WheelJoint::enableLimit(bool enable)
{
    isEnableLimit = enable;

    if (joint != nullptr)
    {
        joint->EnableLimit(enable);
    }
}

bool Physics::WheelJoint::isLimitEnabled() const
{
    return isEnableLimit;
}

void Physics::WheelJoint::setLowerTranslation(float lowerTranslation)
{
    this->lowerTranslation = lowerTranslation;

    if (joint != nullptr)
    {
        joint->SetLimits(lowerTranslation, joint->GetUpperLimit());
    }
}

float Physics::WheelJoint::getLowerTranslation() const
{
    return lowerTranslation;
}

void Physics::WheelJoint::setUpperTranslation(float upperTranslation)
{
    this->upperTranslation = upperTranslation;

    if (joint != nullptr)
    {

        joint->SetLimits(joint->GetLowerLimit(), upperTranslation);
    }
}

float Physics::WheelJoint::getUpperTranslation() const
{
    return upperTranslation;
}

void Physics::WheelJoint::enableMotor(bool enable)
{
    isEnableMotor = enable;

    if (joint != nullptr)
    {
        joint->EnableMotor(enable);
    }
}

bool Physics::WheelJoint::isMotorEnabled() const
{
    return isEnableMotor;
}

void Physics::WheelJoint::setMaxMotorTorque(float torque)
{
    maxMotorTorque = torque;

    if (joint != nullptr)
    {
        joint->SetMaxMotorTorque(torque);
    }
}

float Physics::WheelJoint::getMaxMotorTorque() const
{
    return maxMotorTorque;
}

void Physics::WheelJoint::setMotorSpeed(float speed)
{
    motorSpeed = speed;

    if (joint != nullptr)
    {
        joint->SetMotorSpeed(speed);
    }
}

float Physics::WheelJoint::getMotorSpeed() const
{
    return motorSpeed;
}

void Physics::WheelJoint::setStiffness(float stiffness)
{
    this->stiffness = stiffness;

    if (joint != nullptr)
    {
        joint->SetStiffness(stiffness);
    }
}

float Physics::WheelJoint::getStiffness() const
{
    return stiffness;
}

void Physics::WheelJoint::setDamping(float damping)
{
    this->damping = damping;

    if (joint != nullptr)
    {
        joint->SetDamping(damping);
    }
}

float Physics::WheelJoint::getDamping() const
{
    return damping;
}

void Physics::WheelJoint::setJoint(b2Joint *joint)
{
    if (joint == nullptr)
    {
        this->joint = nullptr;
        return;
    }

    if (joint->GetType() != b2JointType::e_wheelJoint)
    {
        throw std::invalid_argument("WheelJoint (setJoint): joint must be a wheel joint");
    }

    this->joint = reinterpret_cast<b2WheelJoint *>(joint);
}

b2WheelJoint *Physics::WheelJoint::createBox2DJoint(World::World &world, ECS::Entity entity, b2World *box2dWorld, b2Body *owner, b2Body *connected)
{
    b2WheelJointDef jointDef;
    jointDef.Initialize(connected, owner, b2Vec2(anchor.x, anchor.y), b2Vec2(axis.x, axis.y));

    jointDef.enableLimit = isEnableLimit;
    jointDef.lowerTranslation = lowerTranslation;
    jointDef.upperTranslation = upperTranslation;

    jointDef.enableMotor = isEnableMotor;
    jointDef.maxMotorTorque = maxMotorTorque;
    jointDef.motorSpeed = motorSpeed;

    jointDef.stiffness = stiffness;
    jointDef.damping = damping;

    auto userData = new JointUserData{entity, this->connected};
    jointDef.userData.pointer = reinterpret_cast<uintptr_t>(userData);

    auto joint = box2dWorld->CreateJoint(&jointDef);
    if (joint->GetType() != b2JointType::e_wheelJoint)
    {
        throw std::runtime_error("WheelJoint (createBox2DJoint): The created joint is not a wheel joint. Something has gone terribly wrong.");
    }

    return reinterpret_cast<b2WheelJoint *>(joint);
}

b2WheelJoint *Physics::WheelJoint::getJoint()
{
    return joint;
}

const b2WheelJoint *Physics::WheelJoint::getJoint() const
{
    return joint;
}
