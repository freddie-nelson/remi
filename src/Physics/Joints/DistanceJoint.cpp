#include "../../../include/Physics/Joints/DistanceJoint.h"

#include <stdexcept>
#include <iostream>

Physics::DistanceJoint::DistanceJoint(ECS::Entity connected, glm::vec2 anchorA, glm::vec2 anchorB) : Joint(connected)
{
    this->anchorA = anchorA;
    this->anchorB = anchorB;
}

float Physics::DistanceJoint::setLength(float length)
{
    this->length = length;

    if (joint != nullptr)
    {
        this->length = joint->SetLength(length);
    }

    return this->length;
}

float Physics::DistanceJoint::getLength() const
{
    return length;
}

float Physics::DistanceJoint::setMinLength(float minLength)
{
    this->minLength = minLength;

    if (joint != nullptr)
    {
        this->minLength = joint->SetMinLength(minLength);
    }

    return this->minLength;
}

float Physics::DistanceJoint::getMinLength() const
{
    return minLength;
}

float Physics::DistanceJoint::setMaxLength(float maxLength)
{
    this->maxLength = maxLength;

    if (joint != nullptr)
    {
        this->maxLength = joint->SetMaxLength(maxLength);
    }

    return this->maxLength;
}

float Physics::DistanceJoint::getMaxLength() const
{
    return maxLength;
}

void Physics::DistanceJoint::setStiffness(float stiffness)
{
    this->stiffness = stiffness;

    if (joint != nullptr)
    {
        joint->SetStiffness(stiffness);
    }
}

float Physics::DistanceJoint::getStiffness() const
{
    return stiffness;
}

void Physics::DistanceJoint::setDamping(float damping)
{
    this->damping = damping;

    if (joint != nullptr)
    {
        joint->SetDamping(damping);
    }
}

float Physics::DistanceJoint::getDamping() const
{
    return damping;
}

b2DistanceJoint *Physics::DistanceJoint::createBox2DJoint(World::World &world, ECS::Entity entity, b2World *box2dWorld, b2Body *owner, b2Body *connected)
{
    b2DistanceJointDef jointDef;
    jointDef.bodyA = owner;
    jointDef.bodyB = connected;
    jointDef.localAnchorA = b2Vec2(anchorA.x, anchorA.y);
    jointDef.localAnchorB = b2Vec2(anchorB.x, anchorB.y);
    jointDef.collideConnected = collideConnected;

    if (length != -1.0f)
    {
        jointDef.length = length;
    }

    if (minLength != -1.0f)
    {
        jointDef.minLength = minLength;
    }

    if (maxLength != -1.0f)
    {
        jointDef.maxLength = maxLength;
    }

    // calculate from anchor points
    if (length == -1.0f || minLength == -1.0f || maxLength == -1.0f)
    {
        auto &sceneGraph = world.getSceneGraph();
        auto worldAnchorA = glm::vec2(sceneGraph.getModelMatrix(entity) * glm::vec4(anchorA, 0, 1));
        auto worldAnchorB = glm::vec2(sceneGraph.getModelMatrix(this->connected) * glm::vec4(anchorB, 0, 1));

        auto worldAToB = worldAnchorB - worldAnchorA;
        auto lenWorldAToB = glm::length(worldAToB);

        if (length == -1.0f)
        {
            jointDef.length = lenWorldAToB;
        }

        if (minLength == -1.0f)
        {
            jointDef.minLength = jointDef.length;
        }

        if (maxLength == -1.0f)
        {
            jointDef.maxLength = jointDef.length;
        }
    }

    jointDef.stiffness = stiffness;
    jointDef.damping = damping;

    auto userData = new JointUserData{entity, this->connected};
    jointDef.userData.pointer = reinterpret_cast<uintptr_t>(userData);

    auto joint = box2dWorld->CreateJoint(&jointDef);
    if (joint->GetType() != b2JointType::e_distanceJoint)
    {
        throw std::runtime_error("DistanceJoint (createBox2DJoint): The created joint is not a distance joint. Something has gone terribly wrong.");
    }

    return reinterpret_cast<b2DistanceJoint *>(joint);
}

void Physics::DistanceJoint::setJoint(b2Joint *joint)
{
    if (joint->GetType() != b2JointType::e_distanceJoint)
    {
        throw std::invalid_argument("DistanceJoint (setJoint): Joint must be a distance joint.");
    }

    this->joint = reinterpret_cast<b2DistanceJoint *>(joint);
}

b2DistanceJoint *Physics::DistanceJoint::getJoint()
{
    return joint;
}

const b2DistanceJoint *Physics::DistanceJoint::getJoint() const
{
    return joint;
}
