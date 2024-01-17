#pragma once

#include "../../gl.h"
#include "GLType.h"

namespace Rendering
{

    /**
     * Represents a shader uniform.
     */
    template <typename T>
    class Uniform
    {
    public:
        /**
         * Creates a uniform.
         *
         * @param name The name of the uniform
         * @param value The value of the uniform
         * @param count The number of items in the array, 1 if not an array
         * @param type The OpenGL type of the uniform
         */
        Uniform(std::string name, T value, size_t count = 1, GLenum type = GLType::type<T>) : name(name), value(value), count(count), type(type)
        {
        }

        /**
         * Gets the name of the uniform.
         *
         * @returns The name of the uniform.
         */
        const std::string &getName()
        {
            return name;
        }

        /**
         * Sets the value of the uniform.
         *
         * @param value The value of the uniform
         * @param count The number of items in the array, 1 if not an array
         * @param type The OpenGL type of the uniform, defaults to the previous type of the uniform
         */
        void set(T value, int count = 1, GLenum type = type)
        {
            this->value = value;
            this->count = count;
            this->type = type;
        }

        /**
         * Gets the value of the uniform.
         *
         * @returns The value of the uniform.
         */
        T get()
        {
            return value;
        }

        /**
         * Gets the size of the uniform array.
         *
         * 1 if the uniform is not an array.
         *
         * @returns The size of the uniform array.
         */
        size_t size()
        {
            return count;
        }

        /**
         * Gets the OpenGL type of the uniform.
         *
         * @returns The OpenGL type of the uniform.
         */
        GLenum getGLType()
        {
            return type;
        }

    private:
        const std::string name;
        T value;
        size_t count;
        GLenum type;
    };
}