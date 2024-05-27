#include <cstdint>

namespace Physics
{
    /**
     * Represents the collision filter for a collider.
     */
    struct CollisionFilter
    {
        /**
         * The collision category bits.
         *
         * This state the category this shape belongs to.
         *
         * Normally you would just set one bit.
         */
        uint16_t category = 0x0001;

        /**
         * The collision mask bits.
         *
         * This states the categories that this shape would accept for collision.
         */
        uint16_t mask = 0xFFFF;

        /**
         * The collision group.
         *
         * Collision groups allow a certain group of objects to never collide (negative) or always collide (positive).
         * Zero means no collision group.
         *
         * Non-zero group filtering always wins against the mask bits.
         */
        int16_t group = 0;
    };
}