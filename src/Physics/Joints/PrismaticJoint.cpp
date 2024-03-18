#include "../../../include/Physics/Joints/PrismaticJoint.h"

Physics::PrismaticJoint::PrismaticJoint(ECS::Entity connected, glm::vec2 worldAnchor, glm::vec2 worldAxis)
    : Joint(connected, JointType::PRISMATIC)
{
    setWorldAnchor(worldAnchor);
    setWorldAxis(worldAxis);
}

void Physics::PrismaticJoint::setWorldAnchor(glm::vec2 worldAnchor)
{
    this->worldAnchor = worldAnchor;
    useWorldAnchor = true;

    if (joint != nullptr)
    {
        setJoint(nullptr);
    }
}

const glm::vec2 &Physics::PrismaticJoint::getWorldAnchor() const
{
    return worldAnchor;
}

void Physics::PrismaticJoint::setWorldAxis(glm::vec2 worldAxis)
{
    this->worldAxis = worldAxis;
    useWorldAxis = true;

    if (joint != nullptr)
    {
        setJoint(nullptr);
    }
}

const glm::vec2 &Physics::PrismaticJoint::getWorldAxis() const
{
    return worldAxis;
}

void Physics::PrismaticJoint::setLocalAxisA(glm::vec2 localAxisA)
{
    this->localAxisA = localAxisA;
    useWorldAxis = false;

    if (joint != nullptr)
    {
        setJoint(nullptr);
    }
}

const glm::vec2 &Physics::PrismaticJoint::getLocalAxisA() const
{
    return localAxisA;
}

void Physics::PrismaticJoint::setAnchorA(glm::vec2 anchorA)
{
    this->anchorA = anchorA;
    useWorldAnchor = false;

    if (joint != nullptr)
    {
        setJoint(nullptr);
    }
}

const glm::vec2 &Physics::PrismaticJoint::getAnchorA() const
{
    return anchorA;
}

void Physics::PrismaticJoint::setAnchorB(glm::vec2 anchorB)
{
    this->anchorB = anchorB;
    useWorldAnchor = false;

    if (joint != nullptr)
    {
        setJoint(nullptr);
    }
}

const glm::vec2 &Physics::PrismaticJoint::getAnchorB() const
{
    return anchorB;
}

void Physics::PrismaticJoint::setReferenceAngle(float referenceAngle)
{
    this->referenceAngle = referenceAngle;

    if (joint != nullptr)
    {
        setJoint(nullptr);
    }
}

float Physics::PrismaticJoint::getReferenceAngle() const
{
    return referenceAngle;
}

void Physics::PrismaticJoint::enableLimit(bool enable)
{
    isEnableLimit = enable;

    if (joint != nullptr)
    {
        joint->EnableLimit(enable);
    }
}

bool Physics::PrismaticJoint::isLimitEnabled() const
{
    return isEnableLimit;
}

void Physics::PrismaticJoint::setLowerTranslation(float lowerTranslation)
{
    this->lowerTranslation = lowerTranslation;

    if (joint != nullptr)
    {
        joint->SetLimits(lowerTranslation, upperTranslation);
    }
}

float Physics::PrismaticJoint::getLowerTranslation() const
{
    return lowerTranslation;
}

void Physics::PrismaticJoint::setUpperTranslation(float upperTranslation)
{
    this->upperTranslation = upperTranslation;

    if (joint != nullptr)
    {
        joint->SetLimits(lowerTranslation, upperTranslation);
    }
}

float Physics::PrismaticJoint::getUpperTranslation() const
{
    return upperTranslation;
}

void Physics::PrismaticJoint::enableMotor(bool enable)
{
    isEnableMotor = enable;

    if (joint != nullptr)
    {
        joint->EnableMotor(enable);
    }
}

bool Physics::PrismaticJoint::isMotorEnabled() const
{
    return isEnableMotor;
}

void Physics::PrismaticJoint::setMaxMotorForce(float force)
{
    maxMotorForce = force;

    if (joint != nullptr)
    {
        joint->SetMaxMotorForce(force);
    }
}

float Physics::PrismaticJoint::getMaxMotorForce() const
{
    return maxMotorForce;
}

void Physics::PrismaticJoint::setMotorSpeed(float speed)
{
    motorSpeed = speed;

    if (joint != nullptr)
    {
        joint->SetMotorSpeed(speed);
    }
}

float Physics::PrismaticJoint::getMotorSpeed() const
{
    return motorSpeed;
}

void Physics::PrismaticJoint::setJoint(b2Joint *joint)
{
    if (joint == nullptr)
    {
        throw std::invalid_argument("PrismaticJoint (setJoint): joint cannot be null");
    }

    if (joint->GetType() != b2JointType::e_prismaticJoint)
    {
        throw std::invalid_argument("PrismaticJoint (setJoint): joint must be a prismatic joint");
    }

    this->joint = reinterpret_cast<b2PrismaticJoint *>(joint);
}

b2PrismaticJoint *Physics::PrismaticJoint::createBox2DJoint(World::World &world, ECS::Entity entity, b2World *box2dWorld, b2Body *owner, b2Body *connected)
{
    b2PrismaticJointDef jointDef;
    jointDef.Initialize(owner, connected, b2Vec2(worldAnchor.x, worldAnchor.y), b2Vec2(worldAxis.x, worldAxis.y));

    jointDef.collideConnected = getCollideConnected();

    if (!useWorldAnchor)
    {
        jointDef.localAnchorA = b2Vec2(anchorA.x, anchorA.y);
        jointDef.localAnchorB = b2Vec2(anchorB.x, anchorB.y);
    }

    if (!useWorldAxis)
    {
        jointDef.localAxisA = b2Vec2(localAxisA.x, localAxisA.y);
    }

    jointDef.referenceAngle = referenceAngle;
    jointDef.enableLimit = isEnableLimit;
    jointDef.lowerTranslation = lowerTranslation;
    jointDef.upperTranslation = upperTranslation;
    jointDef.enableMotor = isEnableMotor;
    jointDef.motorSpeed = motorSpeed;
    jointDef.maxMotorForce = maxMotorForce;

    auto userData = new JointUserData{entity, this->connected};
    jointDef.userData.pointer = reinterpret_cast<uintptr_t>(userData);

    auto joint = box2dWorld->CreateJoint(&jointDef);
    if (joint->GetType() != b2JointType::e_prismaticJoint)
    {
        throw std::runtime_error("PrismaticJoint (createBox2DJoint): The created joint is not a prismatic joint. Something has gone terribly wrong.");
    }

    return reinterpret_cast<b2PrismaticJoint *>(joint);
}

b2PrismaticJoint *Physics::PrismaticJoint::getJoint()
{
    return joint;
}

const b2PrismaticJoint *Physics::PrismaticJoint::getJoint() const
{
    return joint;
}