#pragma once

#include <string>
#include <unordered_map>

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
        bool loadFromFile(std::string vertex, std::string fragment);

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
        bool loadFromSource(std::string vertex, std::string fragment);

        /**
         * Sets OpenGL to use this shader.
         */
        void use();

        /**
         * Unbinds the shader program if it is in use.
         *
         * Calls glUseProgram(0) to deselect this shader program.
         */
        void unbind();

        /**
         * Sets the value of the uniform with the given name to the given value.
         *
         * This function does not check that the value provided matches the uniform type. The user must make sure of this themselves.
         *
         * For vecs and matrices types `value` should be the glm equivalent.
         *
         * @param name The name of the uniform.
         * @param value The value to set the uniform to.
         */
        void setUniform(std::string name, void *value);

        /**
         * Sets the value of the uniform with the given name to the given value.
         *
         * This function does not check that the value provided matches the uniform type. The user must make sure of this themselves.
         *
         * The uniform must be an array.
         *
         * For vecs and matrices types `value` should be an array of the glm equivalent.
         *
         * @param name The name of the uniform.
         * @param value The value to set the uniform to.
         * @param length The length of the array.
         */
        void setUniformArray(std::string name, void *value, size_t length);

        /**
         * Sets the value of the attribute with the given name to the given value.
         *
         * @param name The name of the attribute.
         * @param value The value to set the attribute to.
         */
        void setAttrib(std::string name, void *value);

        /**
         * Gets wether or not this shader program is currently in use.
         *
         * @returns True if the program is in use, otherwise false.
         */
        bool inUse();

    private:
        bool loaded = false;

        unsigned int programId;

        std::unordered_map<std::string, void *> attribValues;
        std::unordered_map<std::string, void *> uniformValues;
        std::unordered_map<std::string, std::pair<void *, size_t>> uniformArrayValues;

        /**
         * Compiles the given shader source.
         *
         * @param source The shader source.
         * @param shaderType The type of shader, either `GL_VERTEX_SHADER` or `GL_FRAGMENT_SHADER`
         * @param shader The shader program id
         *
         * @returns True if the compilation was successful, otherwise false.
         */
        bool
        compileShaderSource(std::string source, unsigned int shaderType, unsigned int &shader);

        /**
         * Gets the location of the given uniform.
         *
         * @param name The name of the uniform.
         *
         * @returns The location of the uniform.
         */
        int getUniformLocation(std::string name);

        /**
         * Gets the location of the given attribute.
         *
         * @param name The name of the attribute.
         *
         * @returns The location of the attribute.
         */
        int getAttribLocation(std::string name);

        /**
         * Gets the type of the uniform with the given name.
         *
         * @param name The name of the uniform.
         *
         * @returns The type of the uniform.
         */
        unsigned int getUniformType(std::string name);

        /**
         * Gets the type of the attrib with the given name.
         *
         * @param name The name of the attrib.
         *
         * @returns The type of the attrib.
         */
        unsigned int getAttribType(std::string name);

        /**
         * Checks the uniform set rules for the given uniform.
         *
         * @param name The name of the uniform.
         */
        void checkUniformSetRules(std::string name);
    };
}