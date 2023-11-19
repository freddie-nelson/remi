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
    T *flattenGlmVecArray(glm::vec<L, T, Q> *v, size_t length);

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
    T *flattenGlmMatArray(glm::mat<C, R, T, Q> *v, size_t length);
}