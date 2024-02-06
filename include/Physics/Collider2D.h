#pragma once

#include <glm/vec2.hpp>
#include <vector>

namespace Physics
{
    enum ColliderShapeType
    {
        POLYGON,
        CIRCLE,
        EDGE,
        CHAIN,
    };

    /**
     * Represents a 2D collider shape.
     *
     * This is the shape of the collider.
     */
    class ColliderShape2D
    {
    public:
        /**
         * Gets the type of the collider shape.
         *
         * @returns The type of the collider shape.
         */
        ColliderShapeType getType() const;

    protected:
        ColliderShapeType type;
    };

    /**
     * Represents a 2D polygon collider.
     *
     * This is a collider that is a convex polygon. [[info]](https://en.wikipedia.org/wiki/Convex_polygon)
     */
    struct PolygonCollider2D : public ColliderShape2D
    {
        /**
         * Creates a new 2D polygon collider.
         */
        PolygonCollider2D(std::vector<glm::vec2> vertices);

        /**
         * The vertices of the polygon.
         *
         * This is in local space.
         */
        std::vector<glm::vec2> vertices;
    };

    /**
     * Represents a 2D circle collider.
     *
     * This is a collider that is a geometric circle.
     */
    struct CircleCollider2D : public ColliderShape2D
    {
        /**
         * Creates a new 2D circle collider.
         *
         * @param radius The radius of the circle.
         * @param centre The centre of the circle.
         */
        CircleCollider2D(float radius, glm::vec2 centre = glm::vec2(0.0f, 0.0f));

        /**
         * The radius of the circle.
         */
        float radius;

        /**
         * The centre of the circle.
         *
         * This is in local space.
         */
        glm::vec2 centre;
    };

    /**
     * Represents a 2D edge collider.
     *
     * This is a collider that is a line segment.
     *
     * You can specify adjacent vertices to the start and end to create smooth collision across junctions.
     * This is useful for when you have a chain of edges. You would specify the adjacent start to be the end of the previous edge and the adjacent end to be the start of the next edge.
     *
     * If you specify adjacent vertices the edge will be one-sided and the normal will point to the right looking from the start to the end.
     * If you do not specify that the edge is one-sided the adjacent vertices will be ignored and the edge will be two-sided.
     */
    struct EdgeCollider2D : public ColliderShape2D
    {
        /**
         * Creates a new 2D edge collider.
         *
         * This collider will be two-sided.
         *
         * @param start The start position of the edge.
         * @param end The end position of the edge.
         */
        EdgeCollider2D(glm::vec2 start, glm::vec2 end);

        /**
         * Creates a new 2D edge collider.
         *
         * This collider will be one-sided and the normal will point to the right looking from the start to the end.
         *
         * @param adjacentStart The vertex adjacent to the start.
         * @param start The start position of the edge.
         * @param end The end position of the edge.
         * @param adjacentEnd The vertex adjacent to the end.
         */
        EdgeCollider2D(glm::vec2 adjacentStart, glm::vec2 start, glm::vec2 end, glm::vec2 adjacentEnd);

        /**
         * The start position of the edge.
         *
         * This is in local space.
         */
        glm::vec2 start;

        /**
         * The end position of the edge.
         *
         * This is in local space.
         */
        glm::vec2 end;

        /**
         * The vertex adjacent to the start.
         *
         * This is in local space.
         *
         * This used to create smooth collision across junctions.
         */
        glm::vec2 adjacentStart;

        /**
         * The vertex adjacent to the end.
         *
         * This is in local space.
         *
         * This used to create smooth collision across junctions.
         */
        glm::vec2 adjacentEnd;

        /**
         * Whether or not the edge is one-sided.
         */
        bool oneSided;
    };

    /**
     * Represents a 2D chain collider.
     *
     * This is a collider that is a chain of line segments.
     *
     * Chains always provide one sided collision.
     *
     * You can specify adjacent vertices to the start and end to create smooth collision across junctions.
     * If you specify adjacent vertices you must also set the hasAdjacentVertices flag to true for them to be used.
     *
     * You can also specify whether or not the chain is a loop. If it is a loop the chain will be closed and the last vertex will be connected to the first.
     * If the vertices are provided in counter-clockwise order the loop normal will point outwards and if they are provided in clockwise order the loop normal will point inwards.
     */
    struct ChainCollider2D : public ColliderShape2D
    {
        /**
         * Creates a new 2D chain collider.
         *
         * @param vertices The vertices of the chain.
         * @param isLoop Whether or not the chain is a loop.
         */
        ChainCollider2D(std::vector<glm::vec2> vertices, bool isLoop = false);

        /**
         * Creates a new 2D chain collider.
         *
         * @param adjacentStart The vertex adjacent to the start.
         * @param vertices The vertices of the chain.
         * @param adjacentEnd The vertex adjacent to the end.
         */
        ChainCollider2D(glm::vec2 adjacentStart, std::vector<glm::vec2> vertices, glm::vec2 adjacentEnd);

        /**
         * The vertices of the chain.
         *
         * This is in local space.
         */
        std::vector<glm::vec2> vertices;

        /**
         * Whether or not the chain is a loop.
         *
         * If this is true the chain will be closed and the last vertex will be connected to the first.
         */
        bool isLoop;

        /**
         * The adjacent vertex to the start.
         *
         * This is in local space.
         *
         * This used to create smooth collision across junctions.
         */
        glm::vec2 adjacentStart;

        /**
         * The adjacent vertex to the end.
         *
         * This is in local space.
         *
         * This used to create smooth collision across junctions.
         */
        glm::vec2 adjacentEnd;

        /**
         * Whether or not the chain has adjacent vertices.
         */
        bool hasAdjacentVertices;
    };

    /**
     * Represents a 2D collider.
     */
    struct Collider2D
    {
        /**
         * Creates a new 2D collider.
         */
        Collider2D();

        /**
         * Destroys the 2D collider.
         */
        ~Collider2D();

        /**
         * The shape of the collider.
         */
        ColliderShape2D *shape;

        /**
         * The density of the collider.
         */
        float density;

        /**
         * The friction of the collider.
         */
        float friction;

        /**
         * The restitution (elasticity) of the collider.
         */
        float restitution;

        /**
         * The restitution velocity threshold. Any collisions above this speed have restitution applied (will bounce).
         */
        float restitutionThreshold;

        /**
         * Whether or not the collider is a sensor.
         *
         * A sensor will not cause collisions but will still report them.
         */
        bool isSensor;

        // TODO: add categories and masks
    };
}