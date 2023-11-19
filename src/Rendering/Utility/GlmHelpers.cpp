#include "../../../include/Rendering/Utility/GlmHelpers.h"

template <glm::length_t L, class T, enum glm::qualifier Q>
T *Rendering::flattenGlmVecArray(glm::vec<L, T, Q> *v, size_t length)
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

template <glm::length_t C, glm::length_t R, class T, enum glm::qualifier Q>
T *flattenGlmMatArray(glm::mat<C, R, T, Q> *v, size_t length)
{
    const size_t matLen = C * R;
    const size_t flattenedLen = matLen * length;
    T *flattened = new T[flattenedLen];

    for (size_t i = 0; i < length; i++)
    {
        glm::mat<C, R, T, Q> mat = v[i];

        for (size_t j = 0; j < matLen; j++)
        {
            flattened[i * matLen + j] = mat[j];
        }
    }

    return flattened;
}
