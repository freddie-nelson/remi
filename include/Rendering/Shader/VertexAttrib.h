#pragma once

namespace Rendering
{

    /**
     * Represents a shader vertex attribute.
     */
    template <typename T>
    class VertexAttrib
    {
    public:
        /**
         * Creates a vertex attribute.
         *
         * The value will be copied into the vertex attribute, use std::move to move the value into the vertex attribute.
         *
         * @param name The name of the vertex attribute
         * @param value The value of the vertex attribute
         * @param type The OpenGL type of the vertex attribute
         */
        VertexAttrib(std::string name, std::vector<T> value, GLenum type = GLType::type<T>) : name(name), value(value), type(type)
        {
        }

        /**
         * Gets the name of the vertex attribute.
         *
         * @returns The name of the vertex attribute.
         */
        const std::string &getName()
        {
            return name;
        }

        /**
         * Sets the value of the vertex attribute.
         *
         * @param value The value of the vertex attribute
         * @param type The OpenGL type of the vertex attribute, defaults to the previous type of the vertex attribute
         */
        void set(std::vector<T> value, GLenum type = type)
        {
            this->value = value;
            this->type = type;
        }

        /**
         * Gets the value of the vertex attribute.
         *
         * @returns The value of the vertex attribute.
         */
        const std::vector<T> &get()
        {
            return value;
        }

        /**
         * Gets the size of the vertex attribute array.
         *
         * @returns The size of the vertex attribute array.
         */
        size_t size()
        {
            return value.size();
        }

    private:
        const std::string name;
        const std::vector<T> value;
        GLenum type;
    };
}