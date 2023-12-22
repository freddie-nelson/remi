#pragma once

#include "../Material/Color.h"
#include "../../Core/AABB.h"

#include <glm/vec2.hpp>
#include <glm/mat2x2.hpp>
#include <vector>

namespace Rendering
{
    /**
     * ! For adding uvs to mesh:
     * ! First need to implement function to get aabb of mesh's vertices, only recalculate when raw vertices change, don't even need to worry about indices since they won't affect the aabb
     * ! Need to either pass aabb to shader to have uvs calculated in shader, or calculate uvs in c++ and pass to shader
     * ! Need to add ability for user to set custom uvs for the mesh, but use calculated uvs by default, number of uvs should be equal to number of vertices
     * ! uvs are in range [0, 1] and represent the percentage of the texture to use, so (0, 0) is the bottom left of the texture and (1, 1) is the top right of the texture
     * ! maybe add ability to scale aabb used for calculating uvs, so that the user can scale the uvs i.e. the texture on the mesh
     */

    /**
     * A 2D mesh.
     *
     * This mesh must be ready to be rendered by OpenGL.
     *
     * A Mesh2D should be rendered using the GL_TRIANGLES primitive.
     *
     * A Mesh2D must represent a simple polygon.
     *
     * A Mesh2D's vertices must be in counter-clockwise order.
     *
     * By default the mesh's uvs will be set using the mesh's AABB. Custom uvs can be set using `setUvs`.
     *
     * Until custom uvs are set, the uvs will always be recalculated using the mesh's AABB whenever the AABB changes.
     */
    class Mesh2D
    {
    public:
        /**
         * Creates a mesh instance.
         *
         * The mesh is not ready to be rendered until the vertices and indices have been set.
         *
         * This will create a mesh with no vertices or indices.
         */
        Mesh2D();

        /**
         * Creates a mesh instance.
         *
         * The mesh created will be a polygon with the given vertices.
         *
         * The vertices must be in counter-clockwise order and represent a simple polygon.
         *
         * @param vertices The vertices of the polygon.
         */
        Mesh2D(std::vector<glm::vec2> vertices);

        /**
         * Creates a mesh instance.
         *
         * The mesh created will be a regular polygon with the given number of sides and radius.
         *
         * A high number of sides can be used to approximate a circle.
         *
         * @param radius The radius of the polygon.
         * @param sides The number of sides of the polygon.
         */
        Mesh2D(float radius, unsigned int sides);

        /**
         * Creates a mesh instance.
         *
         * The mesh created will be a rectangle with the given width and height.
         *
         * @param width The width of the rectangle.
         * @param height The height of the rectangle.
         */
        Mesh2D(float width, float height);

        /**
         * Creates the vertices and indices of the mesh to form a polygon.
         *
         * This will triangulate the polygon represented by the given vertices, to create the mesh.
         *
         * The vertices must be in counter-clockwise order and represent a simple polygon.
         *
         * @param vertices The vertices of the polygon.
         */
        void createPolygon(std::vector<glm::vec2> vertices);

        /**
         * Creates the vertices and indices of the mesh to form a regular polygon.
         *
         * The polygon is centred at (0, 0) and has the given radius.
         *
         * A high number of sides can be used to approximate a circle.
         *
         * @param radius The radius of the polygon.
         * @param sides The number of sides of the polygon.
         */
        void createRegularPolygon(float radius, unsigned int sides);

        /**
         * Creates the vertices and indices of the mesh to form a rectangle.
         *
         * The rectangle is centred at (0, 0) and has the given width and height.
         *
         * @param width The width of the rectangle.
         * @param height The height of the rectangle.
         */
        void createRect(float width, float height);

        /**
         * Sets the vertices of the mesh.
         *
         * @param vertices The vertices of the mesh.
         */
        void setVertices(std::vector<glm::vec2> vertices);

        /**
         * Gets the vertices of the mesh.
         *
         * @returns the vertices of the mesh.
         */
        const std::vector<glm::vec2> &getVertices() const;

        /**
         * Sets the indices of the mesh.
         *
         * @param indices The indices of the mesh.
         */
        void setIndices(std::vector<unsigned int> indices);

        /**
         * Gets the indices of the mesh.
         *
         * @returns the indices of the mesh.
         */
        const std::vector<unsigned int> &getIndices() const;

        /**
         * Sets the uvs of the mesh.
         *
         * @param uvs The uvs of the mesh.
         */
        void setUvs(std::vector<glm::vec2> uvs);

        /**
         * Sets the uvs of the mesh based on the meshs AABB.
         *
         * This will overwrite any uvs that have been set.
         *
         * This is called on creation of the mesh and will be the default uvs.
         */
        void setUvsFromAABB();

        /**
         * Gets the uvs of the mesh.
         *
         * @returns the uvs of the mesh.
         */
        const std::vector<glm::vec2> &getUvs() const;

        /**
         * Gets the AABB of the mesh.
         *
         * @returns The AABB of the mesh.
         */
        const Core::AABB &getAABB() const;

    private:
        std::vector<glm::vec2> vertices;
        std::vector<unsigned int> indices;

        bool hasCustomUvs;
        std::vector<glm::vec2> uvs;

        Core::AABB aabb;
    };
}
