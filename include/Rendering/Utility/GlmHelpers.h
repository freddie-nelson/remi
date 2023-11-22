#pragma once

#include <glm/glm.hpp>

namespace Rendering
{
    /**
     * Flatten an array of glm::vecs into a flat array.
     *
     * The resulting array will have a length of the vec degree * the num of vecs.
     *
     * @param v The array of vecs
     * @param length The number of vecs
     *
     * @returns The flattened vecs
     */
    template <glm::length_t L, class T, enum glm::qualifier Q>
    T *flattenGlmVecArray(const glm::vec<L, T, Q> *v, size_t length)
    {
        const size_t flattenedLen = L * length;
        T *flattened = new T[flattenedLen];

        for (size_t i = 0; i < length; i++)
        {
            glm::vec<L, T, Q> vec = v[i];

            for (size_t j = 0; j < L; j++)
            {
                flattened[i * L + j] = vec[j];
            }
        }

        return flattened;
    }

    /**
     * Flatten an array of glm::mats into a flat array.
     *
     * The resulting array will have a length of the mat degree squared * the num of mats.
     *
     * @param v The array of mats
     * @param length The number of mats
     *
     * @returns The flattened mats
     */
    template <glm::length_t C, glm::length_t R, class T, enum glm::qualifier Q>
    T *flattenGlmMatArray(const glm::mat<C, R, T, Q> *v, size_t length)
    {
        const size_t matLen = C * R;
        const size_t flattenedLen = matLen * length;
        T *flattened = new T[flattenedLen];

        for (size_t i = 0; i < length; i++)
        {
            glm::mat<C, R, T, Q> mat = v[i];

            for (size_t j = 0; j < C; j++)
            {
                for (size_t k = 0; k < R; k++)
                {
                    flattened[i * matLen + j] = mat[j][k];
                }
            }
        }

        return flattened;
    }
}