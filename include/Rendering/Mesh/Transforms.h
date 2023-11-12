#pragma once

#include "Mesh.h"

namespace Rendering
{
    /**
     * Translates the vertices of the given mesh by the given translation.
     *
     * Modifies the translation of the given mesh in place.
     *
     * @param mesh The mesh to translate.
     * @param translation The translation to apply to the mesh.
     *
     * @returns The given mesh with the translation applied.
     */
    Mesh2D translateMesh(Mesh2D &mesh, const glm::vec2 &translation);

    /**
     * Rotates the vertices of the given mesh by the given angle.
     *
     * Modifies the transform of the given mesh in place.
     *
     * Postive angles rotate counter-clockwise.
     *
     * @param mesh The mesh to rotate.
     * @param angle The angle to rotate the mesh by, in radians.
     *
     * @returns The given mesh with the transform applied.
     */
    Mesh2D rotateMesh(Mesh2D &mesh, float angle);

    /**
     * Scales the vertices of the given mesh by the given scale.
     *
     * Modifies the transform of the given mesh in place.
     *
     * @param mesh The mesh to scale.
     * @param scale The scale to apply to the mesh.
     *
     * @returns The given mesh with the transform applied.
     */
    Mesh2D scaleMesh(Mesh2D &mesh, const glm::vec2 &scale);
}