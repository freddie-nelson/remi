#pragma once

#include "../Color.h"

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
     * The mesh is transformed by multiplying each vertex by the transform matrix and then adding the translation vector.
     *
     * The transform matrix is the combined rotation and scale matrix. See [here](https://en.wikipedia.org/wiki/Transformation_matrix#Examples_in_2_dimensions) for an explanation.
     *
     * The default color given to the mesh is (1.0, 1.0, 1.0, 1.0) or fully opaque white.
     *
     * The default zIndex is 0. A higher zIndex means the mesh will be rendered above meshes with a lower zIndex.
     * A higher zIndex is 'closer' to the camera. 0 is the lowest zIndex and will be rendered below everying, whereas `Config::MAX_Z_INDEX` is the largest z index and will be rendered above everything.
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
         * @param sides The number of sides of the polygon.
         * @param radius The radius of the polygon.
         */
        Mesh2D(unsigned int sides, float radius);

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
         * @param sides The number of sides of the polygon.
         * @param radius The radius of the polygon.
         */
        void createRegularPolygon(unsigned int sides, float radius);

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
         * Moves the mesh forward by the given amount.
         *
         * This will increment the zIndex of the mesh by the given amount.
         *
         * @param amount The amount to move the mesh forward by.
         */
        void moveForward(unsigned int amount = 1);

        /**
         * Moves the mesh backward by the given amount.
         *
         * This will decrement the zIndex of the mesh by the given amount.
         *
         * @param amount The amount to move the mesh backward by.
         */
        void moveBackward(unsigned int amount = 1);

        /**
         * Sets the zIndex of the mesh.
         *
         * @param zIndex The zIndex of the mesh.
         */
        void setZIndex(unsigned int zIndex);

        /**
         * Gets the zIndex of the mesh.
         *
         * @returns the zIndex of the mesh.
         */
        unsigned int getZIndex() const;

        /**
         * Moves the mesh by the given amount.
         *
         * Adds the move vector to the translation of the mesh.
         *
         * @param move The amount to move the mesh by.
         */
        void move(glm::vec2 move);

        /**
         * Translates the mesh by the given amount.
         *
         * Adds the translation vector to the translation of the mesh.
         *
         * @param translation The amount to translate the mesh by.
         */
        void translate(glm::vec2 translation);

        /**
         * Sets the translation of the mesh.
         *
         * @param translation The translation of the mesh.
         */
        void setTranslation(glm::vec2 translation);

        /**
         * Gets the translation of the mesh.
         *
         * @returns the translation of the mesh.
         */
        glm::vec2 getTranslation() const;

        /**
         * Scales the mesh by the given amount.
         *
         * The scale of the mesh is multiplied by the scale vector.
         *
         * The x and y components of the scale vector represent the scale in the x and y directions respectively.
         *
         * e.g. scale(glm::vec2(2.0f, 1.0f)) will double the scale of the mesh in the x direction.
         *
         * @param scale The amount to scale the mesh by.
         */
        void scale(glm::vec2 scale);

        /**
         * Scales the mesh by the given amount.
         *
         * The scale of the mesh is multiplied by the scale scalar.
         *
         * e.g. scale(2.0f) will double the scale of the mesh.
         *
         * @param scale The amount to scale the mesh by.
         */
        void scale(float scale);

        /**
         * Sets the scale of the mesh.
         *
         * The x and y components of the scale vector represent the scale in the x and y directions respectively.
         *
         * @param scale The scale of the mesh.
         */
        void setScale(glm::vec2 scale);

        /**
         * Gets the scale of the mesh.
         *
         * @returns the scale of the mesh.
         */
        glm::vec2 getScale() const;

        /**
         * Sets the shear of the mesh.
         *
         * The x and y components of the shear vector represent the sheer in the x and y directions respectively.
         *
         * @param shear The shear of the mesh.
         */
        void setShear(glm::vec2 shear);

        /**
         * Gets the shear of the mesh.
         *
         * @returns the shear of the mesh.
         */
        glm::vec2 getShear() const;

        /**
         * Rotates the mesh by the given amount.
         *
         * The rotation is in radians.
         *
         * A positive angle will rotate the mesh counter-clockwise.
         *
         * @param angle The amount to rotate the mesh by.
         */
        void rotate(float angle);

        /**
         * Sets the rotation of the mesh.
         *
         * The rotation is in radians.
         *
         * @param angle The rotation angle of the mesh.
         */
        void setRotation(float angle);

        /**
         * Gets the rotation of the mesh.
         *
         * The rotation is in radians.
         *
         * @returns the rotation of the mesh.
         */
        float getRotation() const;

        /**
         * Sets the color of the mesh.
         *
         * @param color The color of the mesh.
         */
        void setColor(Color color);

        /**
         * Gets the color of the mesh.
         *
         * @returns the color of the mesh.
         */
        const Color &getColor() const;

        /**
         * Gets the transformation matrix of the mesh.
         *
         * The transformation matrix is the combined rotation, scale, sheer and translation matrix.
         *
         * The transformation matrix includes the zIndex translation of the mesh.
         *
         * This recomputes the transformation matrix every time it is called.
         *
         * @returns the transformation matrix of the mesh.
         */
        glm::mat4 getTransformationMatrix() const;

    private:
        std::vector<glm::vec2> vertices;
        std::vector<unsigned int> indices;

        unsigned int zIndex = 0;

        glm::vec2 translation = glm::vec2(0.0f);
        glm::vec2 scaleVec = glm::vec2(1.0f);
        glm::vec2 shear = glm::vec2(0.0f);
        float rotation = 0;

        Color color = Color(1.0f);
    };
}
