#pragma once

#include "../Rendering/Mesh/Mesh.h"
#include "box2d/b2_chain_shape.h"
#include "box2d/b2_circle_shape.h"
#include "box2d/b2_edge_shape.h"
#include "box2d/b2_polygon_shape.h"

#include <box2d/b2_shape.h>
#include <glm/vec2.hpp>
#include <vector>
#include <iostream>

namespace Physics
{

    enum ColliderShapeType
    {
        POLYGON = 0,
        CONCAVE_POLYGON,
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
        virtual ~ColliderShape2D() = default;

        /**
         * Gets the type of the collider shape.
         *
         * @returns The type of the collider shape.
         */
        ColliderShapeType getType() const;

        /**
         * Creates a Box2D shape from the collider shape.
         *
         * @returns The Box2D shape.
         */
        virtual b2Shape *createBox2DShape() const = 0;

        /**
         * Clones the collider shape.
         *
         * @returns The cloned collider shape.
         */
        virtual ColliderShape2D *clone() const = 0;

    protected:
        ColliderShapeType type;
    };

    /**
     * Represents a 2D polygon collider.
     *
     * This is a collider that is a convex polygon. [[info]](https://en.wikipedia.org/wiki/Convex_polygon)
     */
    class PolygonColliderShape2D : public ColliderShape2D
    {
    public:
        /**
         * Creates a new 2D polygon collider.
         *
         * The vertices should be in counter-clockwise order and represent a convex polygon.
         *
         * @param vertices The vertices of the polygon.
         */
        PolygonColliderShape2D(std::vector<glm::vec2> vertices);

        /**
         * Creates a new 2D polygon collider.
         *
         * The mesh should be a convex polygon.
         *
         * @param mesh The mesh to create the collider from.
         */
        PolygonColliderShape2D(const Rendering::Mesh2D &mesh);

        /**
         * Creates a new 2D polygon collider.
         *
         * The mesh should be a convex polygon.
         *
         * @param mesh The mesh to create the collider from.
         * @param transform The transform of the mesh.
         */
        PolygonColliderShape2D(const Rendering::Mesh2D &mesh, const Core::Transform &transform);

        /**
         * Creates a new 2D polygon collider.
         *
         * @param other The 2D polygon collider to copy.
         */
        PolygonColliderShape2D(const PolygonColliderShape2D &other);

        /**
         * Creates a Box2D shape from the collider shape.
         *
         * The shape is created on the heap and should be deleted when no longer needed.
         *
         * @returns The Box2D shape.
         */
        b2PolygonShape *createBox2DShape() const override;

        /**
         * Clones the collider shape.
         *
         * @returns The cloned collider shape.
         */
        ColliderShape2D *clone() const override;

        /**
         * The vertices of the polygon.
         *
         * This is in local space.
         */
        std::vector<glm::vec2> vertices;
    };

    /**
     * Represents a 2D concave polygon collider.
     *
     * This is a collider that is a concave polygon. [[info]](https://en.wikipedia.org/wiki/Concave_polygon)
     *
     * This class will split the concave polygon into triangles and add all of them as fixtures to the box2d body.
     */
    class ConcavePolygonColliderShape2D : public ColliderShape2D
    {
    public:
        /**
         * Creates a new 2D concave polygon collider.
         *
         * The vertices should be in counter-clockwise order and represent a concave polygon.
         *
         * @param vertices The vertices of the polygon.
         */
        ConcavePolygonColliderShape2D(std::vector<glm::vec2> vertices);

        /**
         * Creates a new 2D polygon collider.
         *
         * The mesh should be a concave polygon.
         *
         * @param mesh The mesh to create the collider from.
         */
        ConcavePolygonColliderShape2D(const Rendering::Mesh2D &mesh);

        /**
         * Creates a new 2D polygon collider.
         *
         * The mesh should be a concave polygon.
         *
         * @param mesh The mesh to create the collider from.
         * @param transform The transform of the mesh.
         */
        ConcavePolygonColliderShape2D(const Rendering::Mesh2D &mesh, const Core::Transform &transform);

        /**
         * Creates a new 2D concave polygon collider.
         *
         * @param other The 2D concave polygon collider to copy.
         */
        ConcavePolygonColliderShape2D(const ConcavePolygonColliderShape2D &other);

        /**
         * Creates a Box2D shape from the collider shape.
         *
         * This is actually a pointer to an array of shapes. The size is given by `getShapeCount`.
         *
         * The array is created on the heap and should be deleted when no longer needed.
         *
         * @returns The Box2D shape.
         */
        b2PolygonShape *createBox2DShape() const override;

        /**
         * Clones the collider shape.
         *
         * @returns The cloned collider shape.
         */
        ColliderShape2D *clone() const override;

        /**
         * Gets the number of shapes in the concave polygon.
         *
         * This is the number of triangles the concave polygon was split into.
         *
         * This is also the length of the array returned by createBox2DShape.
         *
         * @returns The number of triangles that make up the concave polygon.
         */
        size_t getShapeCount() const;

        /**
         * The vertices of the concave polygon.
         *
         * This is in local space.
         */
        std::vector<glm::vec2> vertices;

        /**
         * The indices to split the vertices into triangles.
         */
        std::vector<unsigned int> indices;
    };

    /**
     * Represents a 2D circle collider.
     *
     * This is a collider that is a geometric circle.
     */
    class CircleColliderShape2D : public ColliderShape2D
    {
    public:
        /**
         * Creates a new 2D circle collider.
         *
         * @param radius The radius of the circle.
         * @param centre The centre of the circle.
         */
        CircleColliderShape2D(float radius, glm::vec2 centre = glm::vec2(0.0f, 0.0f));

        /**
         * Creates a new 2D circle collider.
         *
         * @param other The 2D circle collider to copy.
         */
        CircleColliderShape2D(const CircleColliderShape2D &other);

        /**
         * Creates a Box2D shape from the collider shape.
         *
         * @returns The Box2D shape.
         */
        b2CircleShape *createBox2DShape() const override;

        /**
         * Clones the collider shape.
         *
         * @returns The cloned collider shape.
         */
        ColliderShape2D *clone() const override;

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
    class EdgeColliderShape2D : public ColliderShape2D
    {
    public:
        /**
         * Creates a new 2D edge collider.
         *
         * This collider will be two-sided.
         *
         * @param start The start position of the edge.
         * @param end The end position of the edge.
         */
        EdgeColliderShape2D(glm::vec2 start, glm::vec2 end);

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
        EdgeColliderShape2D(glm::vec2 adjacentStart, glm::vec2 start, glm::vec2 end, glm::vec2 adjacentEnd);

        /**
         * Creates a new 2D edge collider.
         *
         * @param other The 2D edge collider to copy.
         */
        EdgeColliderShape2D(const EdgeColliderShape2D &other);

        /**
         * Creates a Box2D shape from the collider shape.
         *
         * The shape is created on the heap and should be deleted when no longer needed.
         *
         * @returns The Box2D shape.
         */
        b2EdgeShape *createBox2DShape() const override;

        /**
         * Clones the collider shape.
         *
         * @returns The cloned collider shape.
         */
        ColliderShape2D *clone() const override;

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
        glm::vec2 adjacentStart = glm::vec2(0.0f, 0.0f);

        /**
         * The vertex adjacent to the end.
         *
         * This is in local space.
         *
         * This used to create smooth collision across junctions.
         */
        glm::vec2 adjacentEnd = glm::vec2(0.0f, 0.0f);

        /**
         * Whether or not the edge is one-sided.
         */
        bool oneSided = false;
    };

    /**
     * Represents a 2D chain collider.
     *
     * This is a collider that is a chain of line segments.
     *
     * Chains always provide one sided collision.
     *
     * The chain can either be a loop or a chain of edges with adjacent vertices to eliminate ghost collisions.
     *
     * If you don't need adjacent vertices you can specify the adjacent vertices to be the same as the start and end.
     */
    class ChainColliderShape2D : public ColliderShape2D
    {
    public:
        /**
         * Creates a new 2D chain collider.
         *
         * The chain will be a loop.
         *
         * @param vertices The vertices of the chain.
         */
        ChainColliderShape2D(std::vector<glm::vec2> vertices);

        /**
         * Creates a new 2D chain collider.
         *
         * The chain will not be a loop.
         *
         * @param adjacentStart The vertex adjacent to the start.
         * @param vertices The vertices of the chain.
         * @param adjacentEnd The vertex adjacent to the end.
         */
        ChainColliderShape2D(glm::vec2 adjacentStart, std::vector<glm::vec2> vertices, glm::vec2 adjacentEnd);

        /**
         * Creates a new 2D chain collider.
         *
         * @param other The 2D chain collider to copy.
         */
        ChainColliderShape2D(const ChainColliderShape2D &other);

        /**
         * Creates a Box2D shape from the collider shape.
         *
         * The shape is created on the heap and should be deleted when no longer needed.
         *
         * @returns The Box2D shape.
         */
        b2ChainShape *createBox2DShape() const override;

        /**
         * Clones the collider shape.
         *
         * @returns The cloned collider shape.
         */
        ColliderShape2D *clone() const override;

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
        bool isLoop = false;

        /**
         * The adjacent vertex to the start.
         *
         * This is in local space.
         *
         * This used to create smooth collision across junctions.
         */
        glm::vec2 adjacentStart = glm::vec2(0.0f, 0.0f);

        /**
         * The adjacent vertex to the end.
         *
         * This is in local space.
         *
         * This used to create smooth collision across junctions.
         */
        glm::vec2 adjacentEnd = glm::vec2(0.0f, 0.0f);

        /**
         * Whether or not the chain has adjacent vertices.
         */
        bool hasAdjacentVertices = false;
    };
}