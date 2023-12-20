#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace Core
{
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
         * @param points The points to create the AABB from.
         */
        AABB(std::vector<glm::vec2> points);

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
         * @param points The points to create the AABB from.
         */
        void setFromPoints(std::vector<glm::vec2> points);

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

    private:
        /**
         * The minimum point of the AABB.
         */
        glm::vec2 min;

        /**
         * The maximum point of the AABB.
         */
        glm::vec2 max;

        /**
         * The centre of the AABB.
         */
        glm::vec2 centre;

        /**
         * Updates the centre of the AABB.
         */
        void updateCentre();
    };
}