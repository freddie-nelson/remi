#pragma once

#include "../../gl.h"
#include "../../Utility/TypeHelpers.h"

#include <glm/glm.hpp>
#include <type_traits>
#include <stdexcept>
#include <iostream>

namespace Rendering
{
    /**
     * Trys to get the GL type of a c++ type.
     *
     * This is an expensive function and should only be used when necessary.
     *
     * Use GLType::type<T> instead (uses getGLType internally but will only be called once before main).
     *
     * @tparam T The c++ type
     *
     * @returns The GL type.
     */
    template <typename T>
    static GLenum getGLType()
    {
        auto type = remi::generateTypeId(typeid(T).name());

        // std::cout << "Getting GL type of " << type << std::endl;

        if (type == remi::generateTypeId(typeid(int).name()))
            return GL_INT;
        if (type == remi::generateTypeId(typeid(unsigned int).name()))
            return GL_UNSIGNED_INT;
        if (type == remi::generateTypeId(typeid(float).name()))
            return GL_FLOAT;
        if (type == remi::generateTypeId(typeid(char).name()))
            return GL_BYTE;
        if (type == remi::generateTypeId(typeid(unsigned char).name()))
            return GL_UNSIGNED_BYTE;
        if (type == remi::generateTypeId(typeid(short).name()))
            return GL_SHORT;
        if (type == remi::generateTypeId(typeid(unsigned short).name()))
            return GL_UNSIGNED_SHORT;
        if (type == remi::generateTypeId(typeid(bool).name()))
            return GL_BOOL;
        if (type == remi::generateTypeId(typeid(glm::vec2).name()))
            return GL_FLOAT_VEC2;
        if (type == remi::generateTypeId(typeid(glm::vec3).name()))
            return GL_FLOAT_VEC3;
        if (type == remi::generateTypeId(typeid(glm::vec4).name()))
            return GL_FLOAT_VEC4;
        if (type == remi::generateTypeId(typeid(glm::ivec2).name()))
            return GL_INT_VEC2;
        if (type == remi::generateTypeId(typeid(glm::ivec3).name()))
            return GL_INT_VEC3;
        if (type == remi::generateTypeId(typeid(glm::ivec4).name()))
            return GL_INT_VEC4;
        if (type == remi::generateTypeId(typeid(glm::uvec2).name()))
            return GL_UNSIGNED_INT_VEC2;
        if (type == remi::generateTypeId(typeid(glm::uvec3).name()))
            return GL_UNSIGNED_INT_VEC3;
        if (type == remi::generateTypeId(typeid(glm::uvec4).name()))
            return GL_UNSIGNED_INT_VEC4;
        if (type == remi::generateTypeId(typeid(glm::mat2).name()))
            return GL_FLOAT_MAT2;
        if (type == remi::generateTypeId(typeid(glm::mat3).name()))
            return GL_FLOAT_MAT3;
        if (type == remi::generateTypeId(typeid(glm::mat4).name()))
            return GL_FLOAT_MAT4;

        throw std::runtime_error("Could not determine OpenGL type of type.");
    }

    /**
     * Represents the OpenGL type of a c++ type.
     */
    class GLType
    {
    public:
        /**
         * The GL type of a c++ type.
         */
        template <typename T>
        inline static const GLenum type = getGLType<T>();
    };
}
