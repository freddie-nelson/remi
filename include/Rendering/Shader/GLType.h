#pragma once

#include "../../gl.h"
#include "../../Utility/TypeHelpers.h"

#include <glm/glm.hpp>
#include <type_traits>

namespace Rendering
{

    /**
     * Represents the OpenGL type of a c++ type.
     */
    class GLType
    {
    public:
        /**
         * Trys to get the GL type of a c++ type.
         *
         * @tparam T The c++ type
         *
         * @returns The GL type.
         */
        template <typename T>
        static GLenum getGLType()
        {
            auto type = blz::TypeIdGenerator::id<T>;

            switch (type)
            {
            case blz::TypeIdGenerator::id<int>:
                return GL_INT;

            case blz::TypeIdGenerator::id<unsigned int>:
                return GL_UNSIGNED_INT;

            case blz::TypeIdGenerator::id<float>:
                return GL_FLOAT;

            case blz::TypeIdGenerator::id<double>:
                return GL_DOUBLE;

            case blz::TypeIdGenerator::id<char>:
                return GL_BYTE;

            case blz::TypeIdGenerator::id<unsigned char>:
                return GL_UNSIGNED_BYTE;

            case blz::TypeIdGenerator::id<short>:
                return GL_SHORT;

            case blz::TypeIdGenerator::id<unsigned short>:
                return GL_UNSIGNED_SHORT;

            case blz::TypeIdGenerator::id<bool>:
                return GL_BOOL;

            case blz::TypeIdGenerator::id<glm::vec2>:
                return GL_FLOAT_VEC2;

            case blz::TypeIdGenerator::id<glm::vec3>:
                return GL_FLOAT_VEC3;

            case blz::TypeIdGenerator::id<glm::vec4>:
                return GL_FLOAT_VEC4;

            case blz::TypeIdGenerator::id<glm::ivec2>:
                return GL_INT_VEC2;

            case blz::TypeIdGenerator::id<glm::ivec3>:
                return GL_INT_VEC3;

            case blz::TypeIdGenerator::id<glm::ivec4>:
                return GL_INT_VEC4;

            case blz::TypeIdGenerator::id<glm::uvec2>:
                return GL_UNSIGNED_INT_VEC2;

            case blz::TypeIdGenerator::id<glm::uvec3>:
                return GL_UNSIGNED_INT_VEC3;

            case blz::TypeIdGenerator::id<glm::uvec4>:
                return GL_UNSIGNED_INT_VEC4;

            case blz::TypeIdGenerator::id<glm::mat2>:
                return GL_FLOAT_MAT2;

            case blz::TypeIdGenerator::id<glm::mat3>:
                return GL_FLOAT_MAT3;

            case blz::TypeIdGenerator::id<glm::mat4>:
                return GL_FLOAT_MAT4;

            default:
                break;
            }

            throw std::runtime_error("Could not determine OpenGL type of type.");
        }

        /**
         * The GL type of a c++ type.
         */
        template <typename T>
        inline static const GLenum type = getGLType<T>();
    };
}
