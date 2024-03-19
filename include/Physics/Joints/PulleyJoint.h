#pragma once

#include "Joint.h"

#include <box2d/b2_pulley_joint.h>

namespace Physics
{
    /**
     * A pulley joint.
     *
     * The pulley connects 2 bodies to the ground and to eachother. As one body goes up the other goes down.
     */
    class PulleyJoint : public Joint
    {
    public:
        /**
         * Creates a new pulley joint.
         *
         * @param connected The entity the joint is connected to.
         * @param groundAnchorA The ground anchor point on the first body.
         * @param groundAnchorB The ground anchor point on the second body.
         * @param anchorA The anchor point on the first body (the one owning the joint component).
         * @param anchorB The anchor point on the second body (the connected entity).
         * @param ratio The ratio of the pulley.
         */
        PulleyJoint(ECS::Entity connected, glm::vec2 groundAnchorA, glm::vec2 groundAnchorB, glm::vec2 anchorA = glm::vec2(0), glm::vec2 anchorB = glm::vec2(0), float ratio = 1.0f);

        /**
         * Sets the ground anchor point on the first body.
         *
         * @param groundAnchorA The ground anchor point on the first body.
         */
        void setGroundAnchorA(glm::vec2 groundAnchorA);

        /**
         * Gets the ground anchor point on the first body.
         *
         * @returns The ground anchor point on the first body.
         */
        const glm::vec2 &getGroundAnchorA() const;

        /**
         * Sets the ground anchor point on the second body.
         *
         * @param groundAnchorB The ground anchor point on the second body.
         */
        void setGroundAnchorB(glm::vec2 groundAnchorB);

        /**
         * Gets the ground anchor point on the second body.
         *
         * @returns The ground anchor point on the second body.
         */
        const glm::vec2 &getGroundAnchorB() const;

        /**
         * Sets the ratio of the pulley.
         *
         * @param ratio The ratio of the pulley.
         */
        void setRatio(float ratio);

        /**
         * Gets the ratio of the pulley.
         *
         * @returns The ratio of the pulley.
         */
        float getRatio() const;

        /**
         * Sets the length of the pulley segment attached to the first body.
         *
         * @param lengthA The length of the first segment.
         */
        void setLengthA(float lengthA);

        /**
         * Gets the length of the puller segment attached to the first body.
         *
         * @returns The length of the first segment.
         */
        float getLengthA() const;

        /**
         * Resets the length of the pulley segment attached to the first body.
         *
         * This will cause the length to be calculated automatically.
         */
        void resetLengthA();

        /**
         * Sets the length of the pulley segment attached to the second body.
         *
         * @param lengthB The length of the second segment.
         */
        void setLengthB(float lengthB);

        /**
         * Gets the length of the puller segment attached to the second body.
         *
         * @returns The length of the second segment.
         */
        float getLengthB() const;

        /**
         * Resets the length of the pulley segment attached to the second body.
         *
         * This will cause the length to be calculated automatically.
         */
        void resetLengthB();

        /**
         * Sets the underlying Box2D joint.
         *
         * Must be a b2PulleyJoint.
         *
         * @warning Do not use this unless you know what you are doing.
         *
         * @param joint The Box2D joint.
         */
        void setJoint(b2Joint *joint) override;

        /**
         * Creates the box2d distance joint.
         *
         * This does not set the joint component's joint variable.
         *
         * @param world The world the entity is in.
         * @param entity The entity that owns the joint.
         * @param box2dWorld The box2d world to create the joint in.
         * @param owner The box2d body of the owning entity.
         * @param connected The box2d body of the connected entity.
         *
         * @returns The box2d distance joint.
         */
        b2PulleyJoint *createBox2DJoint(World::World &world, ECS::Entity entity, b2World *box2dWorld, b2Body *owner, b2Body *connected) override;

        /**
         * Gets the underlying Box2D joint.
         *
         * May be nullptr if the joint has not been created yet.
         *
         * @warning Do not use this unless you know what you are doing.
         *
         * @returns The underlying Box2D joint.
         */
        b2PulleyJoint *getJoint() override;

        /**
         * Gets the underlying Box2D joint.
         *
         * May be nullptr if the joint has not been created yet.
         *
         * @warning Do not use this unless you know what you are doing.
         *
         * @returns The underlying Box2D joint.
         */
        const b2PulleyJoint *getJoint() const override;

    private:
        b2PulleyJoint *joint = nullptr;

        glm::vec2 groundAnchorA;
        glm::vec2 groundAnchorB;
        float ratio = 1.0f;

        bool lengthASet = false;
        float lengthA = 0.0f;

        bool lengthBSet = false;
        float lengthB = 0.0f;
    };
}
