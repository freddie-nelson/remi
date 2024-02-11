#pragma once

#include "../Rendering/Mesh/Mesh.h"
#include "../Core/Transform.h"

#include <box2d/b2_shape.h>
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
         */
        PolygonColliderShape2D(std::vector<glm::vec2> vertices);

        /**
         * Creates a new 2D polygon collider.
         *
         * @param mesh The mesh to create the collider from.
         */
        PolygonColliderShape2D(const Rendering::Mesh2D &mesh);

        /**
         * Creates a new 2D polygon collider.
         *
         * @param mesh The mesh to create the collider from.
         * @param transform The transform of the mesh.
         */
        PolygonColliderShape2D(const Rendering::Mesh2D &mesh, const Core::Transform &transform);

        /**
         * Creates a Box2D shape from the collider shape.
         *
         * @returns The Box2D shape.
         */
        b2Shape *createBox2DShape() const override;

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
         * Creates a Box2D shape from the collider shape.
         *
         * @returns The Box2D shape.
         */
        b2Shape *createBox2DShape() const override;

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
         * Creates a Box2D shape from the collider shape.
         *
         * @returns The Box2D shape.
         */
        b2Shape *createBox2DShape() const override;

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
         * Creates a Box2D shape from the collider shape.
         *
         * @returns The Box2D shape.
         */
        b2Shape *createBox2DShape() const override;

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
         *
         * The shape will be cloned so you can delete the original.
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
         * Whether or not the shape is dirty.
         *
         * This will be set to true if the shape is changed, and set to false once the physics world has informed box 2d of the shape change.
         *
         * @returns Whether or not the shape is dirty.
         */
        bool isShapeDirty() const;

        /**
         * Informs the collider that the shape has been updated by the physics world.
         *
         * This will set the shape dirty flag to false.
         *
         * This should only be called by the physics world, or if you really know what you're doing.
         */
        void informOfShapeUpdate();

        /**
         * The density of the collider.
         *
         * If the density is non zero then the mass of the body will be calculated from the area and the density.
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

    protected:
        /**
         * The shape of the collider.
         */
        ColliderShape2D *shape;

        /**
         * Wether or not the shape is dirty.
         *
         * This will be set to true if the shape is changed, and set to false once the physics world has informed box 2d of the shape change.
         */
        bool shapeDirty;
    };
}