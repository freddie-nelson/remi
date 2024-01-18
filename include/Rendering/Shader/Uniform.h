#pragma once

#include "../../gl.h"
#include "GLType.h"

namespace Rendering
{
    /**
     * Represents a shader uniform.
     */
    class UniformBase
    {
    public:
        /**
         * Gets the name of the uniform.
         *
         * @returns The name of the uniform.
         */
        virtual const std::string &getName() = 0;

        /**
         * Gets the value of the uniform.
         *
         * @returns A void pointer to the uniform's value.
         */
        virtual void *getValuePointer() = 0;

        /**
         * Gets the OpenGL type of the uniform.
         *
         * @returns The OpenGL type of the uniform.
         */
        virtual GLenum getGLType() = 0;

        /**
         * Gets the size of the uniform array.
         *
         * 1 if the uniform is not an array.
         *
         * @returns The size of the uniform array.
         */
        virtual size_t size() = 0;

        /**
         * Gets whether the uniform is an array.
         *
         * @returns True if the uniform is an array, false otherwise.
         */
        virtual bool isArray() = 0;
    };

    /**
     * Represents a shader uniform.
     */
    template <typename T>
    class Uniform : public UniformBase
    {
    public:
        /**
         * Creates a uniform.
         *
         * Array uniforms should be held in a std::vector.
         *
         * All vector and matrix uniforms should be held in glm::vec or glm::mat.
         *
         * @param name The name of the uniform
         * @param value The value of the uniform
         * @param isArray Whether the uniform is an array
         * @param count The number of items in the array, 1 if not an array
         * @param type The OpenGL type of the uniform, will try to infer the type if not provided (not accurate)
         */
        Uniform(std::string name, T value, bool isArray = false, size_t count = 1, GLenum type = GLType::type<T>) : name(name), value(value), isUniformArray(isArray), count(count), type(type)
        {
            if (count > 1 && !isArray)
            {
                throw std::runtime_error("Cannot create a non-array uniform with a count > 1.");
            }
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
         */
        void set(T value)
        {
            this->value = value;
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
         * Gets the value of the uniform.
         *
         * @returns A void pointer to the uniform's value.
         */
        void *getValuePointer()
        {
            return &value;
        }

        /**
         * Gets whether the uniform is an array.
         *
         * @returns True if the uniform is an array, false otherwise.
         */
        bool isArray()
        {
            return isUniformArray;
        }

        /**
         * Gets the size of the uniform array.
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
        bool isUniformArray;
        size_t count;
        GLenum type;
    };
}