#pragma once

#include "ColliderShape.h"
#include "../Rendering/Mesh/Mesh.h"
#include "../Core/Transform.h"

#include <box2d/b2_shape.h>
#include <box2d/b2_fixture.h>
#include <glm/vec2.hpp>
#include <vector>

namespace Physics
{

    /**
     * Represents a 2D collider.
     */
    class Collider2D
    {
    public:
        /**
         * Creates a new 2D collider.
         *
         * The shape will be cloned so you can delete the original, create it on the stack or reuse it.
         *
         * @param shape The shape of the collider.
         */
        Collider2D(ColliderShape2D *shape);

        /**
         * Creates a new 2D collider.
         *
         * @param other The 2D collider to copy.
         */
        Collider2D(const Collider2D &other);

        /**
         * Destroys the 2D collider.
         */
        ~Collider2D();

        /**
         * Gets the shape of the collider.
         *
         * @returns The shape of the collider.
         */
        const ColliderShape2D *getShape() const;

        /**
         * Sets the shape of the collider.
         *
         * Copies the shape so you can delete the original.
         *
         * @param shape The shape of the collider.
         */
        void setShape(ColliderShape2D *shape);

        /**
         * Gets the density of the collider.
         *
         * @returns The density of the collider.
         */
        float getDensity() const;

        /**
         * Sets the density of the collider.
         *
         * If the density is non zero then the mass of the body will be calculated from the area and the density.
         *
         * @param density The density of the collider.
         */
        void setDensity(float density);

        /**
         * Gets the friction of the collider.
         *
         * @returns The friction of the collider.
         */
        float getFriction() const;

        /**
         * Sets the friction of the collider.
         *
         * Usually in the range [0, 1].
         *
         * @param friction The friction of the collider.
         */
        void setFriction(float friction);

        /**
         * Gets the restitution (elasticity) of the collider.
         *
         * @returns The restitution of the collider.
         */
        float getRestitution() const;

        /**
         * Sets the restitution (elasticity) of the collider.
         *
         * Usually in the range [0, 1].
         *
         * @param restitution The restitution of the collider.
         */
        void setRestitution(float restitution);

        /**
         * Gets the restitution velocity threshold.
         *
         * Any collisions above this speed have restitution applied (will bounce).
         *
         * @returns The restitution velocity threshold.
         */
        float getRestitutionThreshold() const;

        /**
         * Sets the restitution velocity threshold.
         *
         * Any collisions above this speed have restitution applied (will bounce).
         *
         * @param restitutionThreshold The restitution velocity threshold.
         */
        void setRestitutionThreshold(float restitutionThreshold);

        /**
         * Gets whether or not the collider is a sensor.
         *
         * A sensor will not cause collisions but will still report them.
         *
         * @returns Whether or not the collider is a sensor.
         */
        bool getIsSensor() const;

        /**
         * Sets whether or not the collider is a sensor.
         *
         * A sensor will not cause collisions but will still report them.
         *
         * @param isSensor Whether or not the collider is a sensor.
         */
        void setIsSensor(bool isSensor);

        // TODO: add categories and masks

        /**
         * Gets the underlying Box2D fixture.
         *
         * This may be null if the fixture has not been created yet.
         *
         * @warning Do not use this unless you know what you are doing.
         *
         * @returns The underlying Box2D fixtures.
         */
        std::vector<b2Fixture *> *getFixtures();

        /**
         * Sets the underlying Box2D fixture.
         *
         * Setting the fixture to null will cause it to be recreated on the next physics update. This can be used to change the shape of the collider. This is done internally by `setShape`.
         *
         * @warning Do not use this unless you know what you are doing.
         *
         * @param fixture The underlying Box2D fixture.
         */
        void setFixtures(std::vector<b2Fixture *> *fixtures);

    protected:
        /**
         * The underlying Box2D fixtures.
         */
        std::vector<b2Fixture *> *fixtures = nullptr;

        /**
         * The shape of the collider.
         */
        ColliderShape2D *shape;

        /**
         * The density of the collider.
         *
         * If the density is non zero then the mass of the body will be calculated from the area and the density.
         */
        float density = 1.0f;

        /**
         * The friction of the collider.
         *
         * Usually in the range [0, 1].
         */
        float friction = 0.0f;

        /**
         * The restitution (elasticity) of the collider.
         *
         * Usually in the range [0, 1].
         */
        float restitution = 0.0f;

        /**
         * The restitution velocity threshold. Any collisions above this speed have restitution applied (will bounce).
         */
        float restitutionThreshold = 0.0f;

        /**
         * Whether or not the collider is a sensor.
         *
         * A sensor will not cause collisions but will still report them.
         */
        bool isSensor = false;
    };
}