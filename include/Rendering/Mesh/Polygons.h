#pragma once

#include "Mesh.h"

#include <vector>
#include <glm/vec2.hpp>

namespace Rendering
{
    /**
     * Creates a polygon with the given vertices.
     *
     * The vertices must be in counter-clockwise order.
     *
     * The vertices must represent a simple polygon.
     *
     * @param vertices The vertices of the polygon, in counter-clockwise order.
     *
     * @returns A Mesh2D representing the polygon.
     */
    Mesh2D createPolygon(const std::vector<glm::vec2> &vertices);

    /**
     * Creates a regular polygon with the centre at (0, 0), and the given radius and number of sides.
     *
     * A high number of sides can be used to approximate a circle.
     *
     * @param radius The radius of the polygon.
     * @param sides The number of sides of the polygon, must be at least 3.
     *
     * @returns A Mesh2D representing the regular polygon.
     */
    Mesh2D createRegularPolygon(float radius, int sides);

    /**
     * Creates a rectangle with the top left corner or centre at (0, 0), and the given width and height.
     *
     * @param w The width of the rectangle.
     * @param h The height of the rectangle.
     * @param centered Whether or not to centre the rectangle at (0, 0).
     *
     * @returns A Mesh2D representing the rectangle.
     */
    Mesh2D createRect(float w, float h, bool centered = true);
}