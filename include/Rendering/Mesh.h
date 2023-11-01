#include <glm/vec2.hpp>
#include <vector>

namespace Rendering
{
    /**
     * A 2D mesh.
     *
     * This mesh must be ready to be rendered by OpenGL.
     *
     * A Mesh2D should be rendered using the GL_TRIANGLES primitive.
     *
     * A Mesh2D must represent a simple polygon.
     */
    struct Mesh2D
    {
        std::vector<glm::vec2> vertices;
        std::vector<unsigned int> indices;
    };

    /**
     * Triangulates the given vertices into a mesh.
     *
     * The vertices must be in counter-clockwise order.
     *
     * The vertices must represent a simple polygon.
     *
     * @param vertices The vertices to triangulate, must be in counter-clockwise order.
     */
    Mesh2D triangulate(const std::vector<glm::vec2> &vertices);

    /**
     * Creates a polygon with the given vertices.
     *
     * The vertices must be in counter-clockwise order.
     *
     * The vertices must represent a simple polygon.
     *
     * @param vertices The vertices of the polygon, in counter-clockwise order.
     */
    Mesh2D createPolygon(const std::vector<glm::vec2> &vertices);

    /**
     * Creates a regular polygon with the given centre, radius, and number of sides.
     *
     * A high number of sides can be used to approximate a circle.
     *
     * @param centre The centre of the polygon.
     * @param radius The radius of the polygon.
     * @param sides The number of sides of the polygon, must be at least 3.
     */
    Mesh2D createRegularPolygon(glm::vec2 centre, float radius, int sides);

    /**
     * Creates a rectangle with the given top-left corner, width, and height.
     *
     * @param topLeft The top-left corner of the rectangle.
     * @param w The width of the rectangle.
     * @param h The height of the rectangle.
     */
    Mesh2D createRect(glm::vec2 topLeft, float w, float h);
}
