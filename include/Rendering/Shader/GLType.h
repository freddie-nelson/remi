#pragma once

#include "../gl.h"

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
            auto type = typeid(T).hash_code();

            switch (type)
            {
            case typeid(int).hash_code():
            case typeid(int *).hash_code():
                return GL_INT;
                break;

            case typeid(unsigned int).hash_code():
            case typeid(unsigned int *).hash_code():
                return GL_UNSIGNED_INT;
                break;

            case typeid(float).hash_code():
            case typeid(float *).hash_code():
                return GL_FLOAT;
                break;

            case typeid(double).hash_code():
            case typeid(double *).hash_code():
                return GL_DOUBLE;
                break;

            case typeid(char).hash_code():
            case typeid(char *).hash_code():
                return GL_BYTE;
                break;

            case typeid(unsigned char).hash_code():
            case typeid(unsigned char *).hash_code():
                return GL_UNSIGNED_BYTE;
                break;

            case typeid(short).hash_code():
            case typeid(short *).hash_code():
                return GL_SHORT;
                break;

            case typeid(unsigned short).hash_code():
            case typeid(unsigned short *).hash_code():
                return GL_UNSIGNED_SHORT;
                break;

            default:
                throw std::runtime_error("Could not determine OpenGL type of type.");
                break;
            }
        }

        /**
         * The GL type of a c++ type.
         */
        template <typename T>
        inline static const GLenum type = getGLType<T>();
    };
}
