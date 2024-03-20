#pragma once

#include "Joint.h"
#include "RevoluteJoint.h"
#include "PrismaticJoint.h"

#include <box2d/b2_gear_joint.h>

namespace Physics
{
    /**
     * A gear joint.
     *
     * The gear joint connects two joints together. The joints must be either a revolute or a prismatic joint.
     *
     * You must make sure that the two joints connected by the gear are not deleted before the gear joint is deleted. Otherwise you will get a crash.
     *
     * If your gear appears to be rotating the wrong way, negating the ratio may fix it.
     */
    class GearJoint : public Joint
    {
    public:
        /**
         * Creates a new pulley joint.
         *
         * Body B on both joints must be dynamic.
         *
         * @param connected The entity the joint is connected to, must be a body on `jointB`.
         * @param jointA The first joint to connect, the joint on the owning body.
         * @param jointB The second joint to connect, the joint on the connected body.
         * @param ratio The ratio of the gear.
         */
        GearJoint(ECS::Entity connected, Joint *jointA, Joint *jointB, float ratio = 1.0f);

        /**
         * Sets the ratio of the gear.
         *
         * @param ratio The ratio of the gear.
         */
        void setRatio(float ratio);

        /**
         * Gets the ratio of the gear.
         *
         * @returns The ratio of the gear.
         */
        float getRatio() const;

        /**
         * Sets the first joint to connect.
         *
         * Must be a revolute or prismatic joint.
         *
         * @param jointA The first joint to connect.
         */
        void setJointA(Joint *jointA);

        /**
         * Gets the first joint to connect.
         *
         * @returns The first joint to connect.
         */
        Joint *getJointA();

        /**
         * Sets the second joint to connect.
         *
         * Must be a revolute or prismatic joint.
         *
         * @param jointB The second joint to connect.
         */
        void setJointB(Joint *jointB);

        /**
         * Gets the second joint to connect.
         *
         * @returns The second joint to connect.
         */
        Joint *getJointB();

        /**
         * Sets the underlying Box2D joint.
         *
         * Must be a b2GearJoint.
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
        b2GearJoint *createBox2DJoint(World::World &world, ECS::Entity entity, b2World *box2dWorld, b2Body *owner, b2Body *connected) override;

        /**
         * Gets the underlying Box2D joint.
         *
         * May be nullptr if the joint has not been created yet.
         *
         * @warning Do not use this unless you know what you are doing.
         *
         * @returns The underlying Box2D joint.
         */
        b2GearJoint *getJoint() override;

        /**
         * Gets the underlying Box2D joint.
         *
         * May be nullptr if the joint has not been created yet.
         *
         * @warning Do not use this unless you know what you are doing.
         *
         * @returns The underlying Box2D joint.
         */
        const b2GearJoint *getJoint() const override;

    private:
        b2GearJoint *joint = nullptr;

        Joint *jointA;
        Joint *jointB;
        float ratio;
    };
}
