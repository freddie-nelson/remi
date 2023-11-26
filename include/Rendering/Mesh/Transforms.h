#pragma once

#include "Mesh.h"

namespace Rendering
{

    /**
     * Translates the given vector by the given translation.
     *
     * Modifies the given vector in place.
     *
     * @param v The vector to translate.
     * @param translation The translation to apply to the vector.
     *
     * @returns The given vector with the translation applied.
     */
    glm::vec2 translate(glm::vec2 &v, const glm::vec2 &translation);

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
    Mesh2D translate(Mesh2D &mesh, const glm::vec2 &translation);

    /**
     * Rotates the given matrix by the given angle.
     *
     * Modifies the given matrix in place.
     *
     * Postive angles rotate counter-clockwise.
     *
     * @param m The matrix to rotate.
     * @param angle The angle to rotate the matrix by, in radians.
     *
     * @returns The given matrix with the rotation applied.
     */
    glm::mat2 rotate(glm::mat2 &m, float angle);

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
    Mesh2D rotate(Mesh2D &mesh, float angle);

    /**
     * Scales the given matrix by the given scale.
     *
     * Modifies the given matrix in place.
     *
     * @param m The matrix to scale.
     * @param s The scale to apply to the matrix.
     *
     * @returns The given matrix with the scale applied.
     */
    glm::mat2 scale(glm::mat2 &m, const glm::vec2 &s);

    /**
     * Scales the vertices of the given mesh by the given scale.
     *
     * Modifies the transform of the given mesh in place.
     *
     * @param mesh The mesh to scale.
     * @param s The scale to apply to the mesh.
     *
     * @returns The given mesh with the transform applied.
     */
    Mesh2D scale(Mesh2D &mesh, const glm::vec2 &s);

    /**
     * Sheers the given matrix by the given sheer.
     *
     * Modifies the given matrix in place.
     *
     * @param m The matrix to sheer.
     * @param s The sheer to apply to the matrix.
     *
     * @returns The given matrix with the sheer applied.
     */
    glm::mat2 sheer(glm::mat2 &m, const glm::vec2 &s);

    /**
     * Sheers the vertices of the given mesh by the given sheer.
     *
     * Modifies the transform of the given mesh in place.
     *
     * @param mesh The mesh to sheer.
     * @param s The sheer to apply to the mesh.
     *
     * @returns The given mesh with the transform applied.
     */
    Mesh2D sheer(Mesh2D &mesh, const glm::vec2 &s);

    /**
     * Returns the transformation matrix of the given rotation, scale and sheer matrices.
     *
     * The transformation matrix is the combined scale, sheer and rotation matrix.
     *
     * `transformation = rotation * scale * sheer`
     *
     * @param rotation The rotation matrix.
     * @param scale The scale matrix.
     * @param sheer The sheer matrix.
     *
     * @returns The transformation matrix.
     */
    inline glm::mat2 getTransformationMatrix(const glm::mat2 &rotation, const glm::mat2 &scale, const glm::mat2 &sheer)
    {
        return rotation * scale * sheer;
    }

    /**
     * Returns the transformation matrix of the given mesh.
     *
     * The transformation matrix is the combined scale, sheer and rotation matrix.
     *
     * @returns The transformation matrix.
     */
    inline glm::mat2 getTransformationMatrix(const Mesh2D &mesh)
    {
        return getTransformationMatrix(mesh.rotation, mesh.scale, mesh.sheer);
    }

}