#pragma once

#include "../Transform.h"

#include <glm/glm.hpp>
#include <vector>

namespace Core
{
    class BoundingCircle;

    /**
     * Represents an axis aligned bounding box.
     */
    class AABB
    {
    public:
        /**
         * Creates a new AABB.
         *
         * The AABB will be set to (0, 0) for both the minimum and maximum points.
         */
        AABB();

        /**
         * Creates a new AABB.
         *
         * @param min The minimum point of the AABB.
         * @param max The maximum point of the AABB.
         *
         * @throws std::invalid_argument If the AABB is not valid.
         */
        AABB(glm::vec2 min, glm::vec2 max);

        /**
         * Creates a new AABB.
         *
         * The AABB will be the minimum AABB that contains all of the given points.
         *
         * @param points The points to create the AABB for.
         */
        AABB(const std::vector<glm::vec2> &points);

        /**
         * Creates a new AABB.
         *
         * The AABB will be the minimum AABB that contains the given circle.
         *
         * @param centre The centre of the circle.
         * @param radius The radius of the circle.
         */
        AABB(const glm::vec2 &centre, float radius);

        /**
         * Gets the minimum point of the AABB.
         *
         * @returns The minimum point of the AABB.
         */
        const glm::vec2 &getMin() const;

        /**
         * Sets the minimum point of the AABB.
         *
         * @param min The minimum point of the AABB.
         *
         * @throws std::invalid_argument If the AABB is not valid.
         */
        void setMin(glm::vec2 min);

        /**
         * Gets the maximum point of the AABB.
         *
         * @returns The maximum point of the AABB.
         */
        const glm::vec2 &getMax() const;

        /**
         * Sets the maximum point of the AABB.
         *
         * @param max The maximum point of the AABB.
         *
         * @throws std::invalid_argument If the AABB is not valid.
         */
        void setMax(glm::vec2 max);

        /**
         * Sets the AABB from the given points.
         *
         * The AABB will be the minimum AABB that contains all of the given points.
         *
         * @param points The points to create the AABB from.
         */
        void setFromPoints(const std::vector<glm::vec2> &points);

        /**
         * Sets the AABB from the given circle.
         *
         * The AABB will be the minimum AABB that contains the given circle.
         *
         * @param centre The centre of the circle.
         * @param radius The radius of the circle.
         */
        void setFromCircle(const glm::vec2 &centre, float radius);

        /**
         * Gets the width of the AABB.
         */
        unsigned int getWidth() const;

        /**
         * Gets the height of the AABB.
         */
        unsigned int getHeight() const;

        /**
         * Gets the centre of the AABB.
         */
        const glm::vec2 &getCentre() const;

        /**
         * Gets the surface area of the AABB.
         *
         * @returns The surface area of the AABB.
         */
        float getSurfaceArea() const;

        /**
         * Merge this AABB with the given AABB.
         *
         * This will be the minimum AABB that contains both AABBs.
         *
         * This is also known as the union of the AABBs.
         *
         * @param other The AABB to merge with.
         *
         * @returns The merged AABB.
         */
        AABB merge(const AABB &other);

        /**
         * Checks if the given AABB is completely contained within this AABB.
         *
         * @param other The AABB to check.
         *
         * @returns True if the given AABB is completely contained within this AABB, false otherwise.
         */
        bool contains(const AABB &other) const;

        /**
         * Checks if the given AABB overlaps with this AABB.
         *
         * A touch is considered an overlap.
         *
         * @param other The AABB to check.
         *
         * @returns True if the given AABB overlaps with this AABB, false otherwise.
         */
        bool overlaps(const AABB &other) const;

        /**
         * Creates a transformed AABB from this AABB.
         *
         * @param t The transformation to apply.
         *
         * @returns The transformed AABB.
         */
        AABB transform(const Core::Transform &t) const;

    private:
        /**
         * The minimum point of the AABB.
         */
        glm::vec2 min = glm::vec2(0, 0);

        /**
         * The maximum point of the AABB.
         */
        glm::vec2 max = glm::vec2(0, 0);

        /**
         * The centre of the AABB.
         */
        glm::vec2 centre = glm::vec2(0, 0);

        /**
         * Updates the centre of the AABB.
         */
        void updateCentre();

        /**
         * The surface area of the AABB.
         */
        float surfaceArea = 0;

        /**
         * Updates the surface area of the AABB.
         */
        void updateSurfaceArea();
    };
}