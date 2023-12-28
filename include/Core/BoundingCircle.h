#pragma once

#include "./AABB/AABB.h"
#include "./Transform.h"

#include <glm/glm.hpp>

namespace Core
{
    /**
     * Represents a bounding circle.
     *
     * This can be used for fast intersection tests when AABB's are not suitable.
     *
     * i.e. This is used to quickly cull entities that can't be seen by the active camera.
     *
     * The radius of a bounding circle cannot be negative.
     */
    class BoundingCircle
    {
    public:
        /**
         * Creates a bounding circle with the centre at (0, 0) and radius 0.
         */
        BoundingCircle();

        /**
         * Creates a bounding circle with the given centre and radius.
         *
         * @param centre The centre of the circle.
         * @param radius The radius of the circle.
         */
        BoundingCircle(glm::vec2 centre, float radius);

        /**
         * Creates the minimum bounding circle for the given AABB.
         *
         * @param aabb The aabb to contain.
         */
        BoundingCircle(const AABB &aabb);

        /**
         * Creates the minimum bounding circle for the given AABB transformed with the given transform.
         *
         * @param aabb the aabb to contain.
         * @param transform The AABB's transformation matrix.
         */
        BoundingCircle(const AABB &aabb, const Transform &transform);

        /**
         * Checks whether this bounding circle intersects with the given bounding circle.
         *
         * @param b The other bounding circle.
         *
         * @returns Whether or not the bounding circle intersect.
         */
        bool intersects(const BoundingCircle &b) const;

        /**
         * Sets the centre and radius of the bounding circle.
         *
         * @param centre The centre of the circle.
         * @param radius The radius of the circle.
         */
        void set(glm::vec2 centre, float radius);

        /**
         * Sets the bounding circle to the minimum bounding circle for the given AABB.
         *
         * @param aabb The aabb to contain.
         */
        void set(const AABB &aabb);

        /**
         * Sets the bounding circle to the minimum bounding circle for the given transformed AABB.
         *
         * @param aabb The aabb to contain.
         * @param transform The AABB's transformation matrix
         */
        void set(const AABB &aabb, const Transform &transform);

        /**
         * Sets the centre of the bounding circle.
         *
         * @param centre The centre of the bounding circle.
         */
        void setCentre(glm::vec2 centre);

        /**
         * Gets the centre of the bounding circle.
         *
         * @returns The centre of the bounding circle.
         */
        glm::vec2 getCentre() const;

        /**
         * Sets the radius of the bounding circle.
         *
         * @param radius The radius of the bounding circle.
         */
        void setRadius(float radius);

        /**
         * Gets the radius of the bounding circle.
         *
         * @returns The radius of the bounding cirlce.
         */
        float getRadius() const;

        /**
         * Gets the squared radius of the bounding circle.
         *
         * @returns The squared radius of the bounding circle.
         */
        float getSqrRadius() const;

    private:
        /**
         * The centre of the circle.
         */
        glm::vec2 centre;

        /**
         * The radius of the circle.
         */
        float radius;

        /**
         * The squared radius of the circle.
         */
        float sqrRadius;
    };
}