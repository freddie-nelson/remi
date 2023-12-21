#pragma once

#include "../../../include/externals/glad/gl.h"
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <iostream>

namespace Rendering
{
    /**
     * Represents a shader.
     *
     * A shader is a program that runs on the GPU and is used by the Renderer.
     */
    class Shader
    {
    public:
        /**
         * Gets the max vertex attribs (input variables) supported by the GPU.
         *
         * @returns The max vertex attribs.
         */
        static int getMaxVertexAttribs();

        /**
         * Creates a shader instance.
         */
        Shader();

        /**
         * Destroys the shader.
         */
        ~Shader();

        /**
         * Loads the vertex and fragment shader from the given files.
         *
         * @param vertex The path to the vertex shader file.
         * @param fragment The path to the fragment shader file.
         *
         * @returns True if the shader was loaded successfully, false otherwise.
         */
        bool loadFromFile(const std::string &vertex, const std::string &fragment);

        /**
         * Loads the vertex and fragment shader from the given source.
         *
         * The source should be a string containing the shader source.
         *
         * @param vertex The vertex shader source.
         * @param fragment The fragment shader source.
         *
         * @returns True if the shader was loaded successfully, false otherwise.
         */
        bool loadFromSource(const std::string &vertex, const std::string &fragment);

        /**
         * Sets OpenGL to use this shader.
         *
         * Will bind the current VAO.
         */
        void use();

        /**
         * Unbinds the shader program if it is in use.
         *
         * This will also clear all set uniforms.
         *
         * Calls glUseProgram(0) to deselect this shader program.
         */
        void unbind();

        /**
         * Draws the shader.
         *
         * Will bind the current VAO and all uniforms before drawing.
         *
         * @param drawMode The draw mode to use, i.e. GL_TRIANGLES, GL_POINTS, GL_LINES, etc.
         * @param drawCount The number of vertices to draw.
         * @param offset The offset in the arrays or indices to start drawing from.
         */
        void draw(unsigned int drawMode, unsigned int drawCount, unsigned int offset = 0);

        /**
         * Draws the shader.
         *
         * Will bind the current VAO before drawing.
         *
         * @param instanceCount The number of instances to draw.
         * @param drawMode The draw mode to use, i.e. GL_TRIANGLES, GL_POINTS, GL_LINES, etc.
         * @param drawCount The number of vertices to draw.
         * @param offset The offset in the arrays or indices to start drawing from.
         */
        void drawInstanced(unsigned int instanceCount, unsigned int drawMode, unsigned int drawCount, unsigned int offset = 0);

        /**
         * Sets the value of the uniform with the given name to the given value.
         *
         * This function does not check that the value provided matches the uniform type. The user must make sure of this themselves.
         *
         * For vecs and matrices types `value` should be the glm equivalent.
         *
         * It is a good idea to make the passed value pointer a heap allocated value so that it does not get deleted while still in use by the shader..
         *
         * @param name The name of the uniform.
         * @param value The value to set the uniform to.
         */
        void setUniform(const std::string &name, void *value);

        /**
         * Sets the value of the uniform with the given name to the given value.
         *
         * This function does not check that the value provided matches the uniform type. The user must make sure of this themselves.
         *
         * The uniform must be an array.
         *
         * For vecs and matrices types `value` should be an array of the glm equivalent.
         *
         * It is a good idea to make the passed value pointer a heap allocated value so that it does not get deleted while still in use by the shader..
         *
         * @param name The name of the uniform.
         * @param value The value to set the uniform to.
         * @param length The length of the array.
         */
        void setUniformArray(const std::string &name, void *value, size_t length);

        /**
         * Sets the value of the attribute with the given name to the given value.
         *
         * This will bind an ARRAY_BUFFER to the attribute.
         *
         * If the attribute had indices attached then this will reset them to `nullptr`.
         *
         * For matrices stride and componentSize will be calculated within the shader using `matrixSize`.
         *
         * @param name The name of the attribute.
         * @param value The value to set the attribute to.
         * @param length The length of the given array.
         * @param componentSize The number of components in the attribute i.e. 3 for vec3, 4 for mat2 (2 vec2s), etc.
         * @param glType The OpenGL type of the attribute i.e. GL_FLOAT, GL_INT_VEC2, etc.
         * @param normalize Whether or not to normalize the attribute.
         * @param offset The offset of the attribute in the array in bytes.
         * @param drawType The type of draw to use i.e. GL_STATIC_DRAW, GL_DYNAMIC_DRAW, etc.
         * @param divisor The divisor to use for instanced rendering.
         * @param matrixSize The size of the matrix if the attribute is a matrix.
         */
        template <typename T>
        void setAttrib(const std::string &name, T *value, size_t length, size_t componentSize, unsigned int glType, bool normalize, unsigned int offset, unsigned int drawType, int divisor = -1, int matrixSize = -1)
        {
            if (!loaded)
            {
                throw std::runtime_error("Shader must be loaded before attributes can be set.");
            }

            if (drawType != GL_STATIC_DRAW && drawType != GL_DYNAMIC_DRAW && drawType != GL_STREAM_DRAW)
            {
                throw std::invalid_argument("drawType must be either GL_STATIC_DRAW, GL_DYNAMIC_DRAW or GL_STREAM_DRAW.");
            }

            // delete previous indices if they exist
            if (attribValues.contains(name) && attribValues[name].indices != nullptr)
            {
                delete attribValues[name].indices;
                attribValues[name].indices = nullptr;
            }

            int location = getAttribLocation(name);
            unsigned int VBO = attribValues.contains(name) ? attribValues[name].VBO : 0;
            unsigned int EBO = attribValues.contains(name) ? attribValues[name].EBO : 0;

            size_t size = length * sizeof(T);
            size_t stride = componentSize * sizeof(T);

            attribValues[name] = {true, sizeof(T), value, size, componentSize, glType, normalize, stride, offset, drawType, divisor, matrixSize, nullptr, VBO, EBO};

            updateAttributes = true;
        }

        /**
         * Sets the indices of the given attribute's VBO.
         *
         * @param name The name of the attribute.
         * @param indices The indices to set.
         * @param length The length of the indices array.
         * @param drawType The type of draw to use i.e. GL_STATIC_DRAW, GL_DYNAMIC_DRAW, etc.
         */
        void setIndices(const std::string &name, const unsigned int *indices, size_t length, unsigned int drawType);

        /**
         * Gets wether or not this shader program is currently in use.
         *
         * @returns True if the program is in use, otherwise false.
         */
        bool inUse();

    private:
        bool loaded = false;
        bool updateUniforms = true;
        bool updateUniformArrays = true;
        bool updateAttributes = true;
        bool hasIndices = false;

        unsigned int programId = 0;
        unsigned int VAO = 0;

        struct AttribIndices
        {
            const unsigned int *indices;
            size_t length;
            unsigned int drawType;
        };

        struct VertexAttrib
        {
            bool stale;

            size_t typeSize;
            void *value;
            size_t size;
            size_t componentSize;
            unsigned int glType;
            bool normalize;
            size_t stride;
            size_t offset;
            unsigned int drawType;
            int divisor;
            int matrixSize;

            AttribIndices *indices;
            unsigned int VBO;
            unsigned int EBO;
        };

        struct Uniform
        {
            bool stale;

            void *value;
        };

        struct UniformArray
        {
            bool stale;

            void *value;
            size_t length;
        };

        std::unordered_map<std::string, VertexAttrib> attribValues;
        std::unordered_map<std::string, Uniform> uniformValues;
        std::unordered_map<std::string, UniformArray> uniformArrayValues;

        /**
         * Compiles the given shader source.
         *
         * @param source The shader source.
         * @param shaderType The type of shader, either `GL_VERTEX_SHADER` or `GL_FRAGMENT_SHADER`
         * @param shader The shader program id
         *
         * @returns True if the compilation was successful, otherwise false.
         */
        bool compileShaderSource(const std::string &source, unsigned int shaderType, unsigned int &shader);

        /**
         * Binds the set attributes and uniforms to the shader.
         */
        void updateAttributesAndUniforms();

        /**
         * Binds the uniform with the given name from the uniformValues map.
         *
         * @param name The name of the uniform.
         */
        void useUniform(const std::string &name);

        /**
         * Binds the uniform array with the given name from the uniformArrayValues map.
         *
         * @param name The name of the uniform.
         */
        void useUniformArray(const std::string &name);

        /**
         * Binds all the attributes in the attribValues map and creates a VAO for them.
         */
        void createVAO();

        /**
         * Gets the location of the given uniform.
         *
         * @param name The name of the uniform.
         *
         * @returns The location of the uniform.
         */
        int getUniformLocation(const std::string &name);

        /**
         * Gets the location of the given attribute.
         *
         * @param name The name of the attribute.
         *
         * @returns The location of the attribute.
         */
        int getAttribLocation(const std::string &name);

        /**
         * Gets the type of the uniform with the given name.
         *
         * @param name The name of the uniform.
         *
         * @returns The type of the uniform.
         */
        unsigned int getUniformType(const std::string &name);

        /**
         * Gets the name of the uniform stored at the given location.
         *
         * @param location The location of the uniform.
         *
         * @returns The name of the uniform stored at the specified location.
         */
        std::string getUniformName(unsigned int location);

        /**
         * Gets the name of every uniform location to find the correct location for the given name.
         *
         * If `isUniformArray` is true then the name to find will be suffixed with `[0]`.
         *
         * @param name The name of the uniform to find.
         * @param isUniformArray Whether or not the uniform is an array.
         *
         * @returns The location of the uniform.
         */
        int findUniformLocation(const std::string &name, bool isUniformArray = false);

        /**
         * Gets the type of the attrib with the given name.
         *
         * @param name The name of the attrib.
         *
         * @returns The type of the attrib.
         */
        unsigned int getAttribType(const std::string &name);

        /**
         * Checks the uniform set rules for the given uniform.
         *
         * @param name The name of the uniform.
         */
        void checkUniformSetRules(const std::string &name);

        /**
         * Checks if the given draw mode is valid.
         *
         * @param drawMode The draw mode to check.
         *
         * @throws std::invalid_argument If the draw mode is not valid.
         */
        void checkDrawModeValid(unsigned int drawMode);
    };
}