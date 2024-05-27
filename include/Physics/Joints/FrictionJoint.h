#include "./Joint.h"

#include <box2d/b2_friction_joint.h>

namespace Physics
{
    /**
     * A friction joint.
     *
     * The friction joint is used for top-down friction.
     *
     * This joint provides 2d translational and angular friction.
     *
     * The owning entity is the entity to have friction applied to it, the connected entity is often a ground/static entity.
     */
    class FrictionJoint : public Joint
    {
    public:
        /**
         * Creates a friction joint.
         *
         * @param connected The entity the joint is connected to.
         * @param worldAnchor The world anchor point of the friction joint.
         */
        FrictionJoint(ECS::Entity connected, glm::vec2 worldAnchor = glm::vec2(0));

        /**
         * Sets the anchor point on the first body (the one owning the joint component).
         *
         * This is in local space of the first body.
         *
         * @param anchor The anchor point.
         */
        void setAnchorA(glm::vec2 anchorA);

        /**
         * Sets the anchor point on the second body (the connected body).
         *
         * This is in local space of the second body.
         *
         * @param anchor The anchor point.
         */
        void setAnchorB(glm::vec2 anchorB);

        /**
         * Sets the world anchor point of the friction joint.
         *
         * @param worldAnchor The world anchor point of the friction joint.
         */
        void setWorldAnchor(glm::vec2 worldAnchor);

        /**
         * Gets the world anchor point of the friction joint.
         *
         * @returns The world anchor point of the friction joint.
         */
        const glm::vec2 &getWorldAnchor() const;

        /**
         * Sets the maximum force the friction joint can apply.
         *
         * @param maxForce The maximum force the friction joint can apply.
         */
        void setMaxForce(float maxForce);

        /**
         * Gets the maximum force the friction joint can apply.
         *
         * @returns The maximum force the friction joint can apply.
         */
        float getMaxForce() const;

        /**
         * Sets the maximum torque the friction joint can apply.
         *
         * @param maxTorque The maximum torque the friction joint can apply.
         */
        void setMaxTorque(float maxTorque);

        /**
         * Gets the maximum torque the friction joint can apply.
         *
         * @returns The maximum torque the friction joint can apply.
         */
        float getMaxTorque() const;

        /**
         * Sets the underlying Box2D joint.
         *
         * Must be a b2FrictionJoint.
         *
         * @warning Do not use this unless you know what you are doing.
         *
         * @param joint The Box2D joint.
         */
        void setJoint(b2Joint *joint) override;

        /**
         * Creates the box2d weld joint.
         *
         * This does not set the joint component's joint variable.
         *
         * @param world The world the entity is in.
         * @param entity The entity that owns the joint.
         * @param box2dWorld The box2d world to create the joint in.
         * @param owner The box2d body of the owning entity.
         * @param connected The box2d body of the connected entity.
         *
         * @returns The box2d friction joint.
         */
        b2FrictionJoint *createBox2DJoint(World::World &world, ECS::Entity entity, b2World *box2dWorld, b2Body *owner, b2Body *connected) override;

        /**
         * Gets the underlying Box2D joint.
         *
         * May be nullptr if the joint has not been created yet.
         *
         * @warning Do not use this unless you know what you are doing.
         *
         * @returns The underlying Box2D joint.
         */
        b2FrictionJoint *getJoint() override;

        /**
         * Gets the underlying Box2D joint.
         *
         * May be nullptr if the joint has not been created yet.
         *
         * @warning Do not use this unless you know what you are doing.
         *
         * @returns The underlying Box2D joint.
         */
        const b2FrictionJoint *getJoint() const override;

    private:
        b2FrictionJoint *joint = nullptr;

        bool useWorldAnchor = false;
        glm::vec2 worldAnchor = glm::vec2(0);

        float maxForce = 0;
        float maxTorque = 0;
    };
}