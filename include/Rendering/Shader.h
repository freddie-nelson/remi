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
         */
        void use();

        /**
         * Unbinds the shader program if it is in use.
         *
         * Calls glUseProgram(0) to deselect this shader program.
         */
        void unbind();

        /**
         * Draws the shader.
         */
        void draw();

        /**
         * Sets the draw mode of the shader.
         *
         * @param drawMode The draw mode.
         */
        void setDrawMode(unsigned int drawMode);

        /**
         * Gets the draw mode of the shader.
         *
         * @returns The draw mode.
         */
        unsigned int getDrawMode();

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
         * @param name The name of the attribute.
         * @param value The value to set the attribute to.
         * @param componentSize The number of components in the attribute i.e. 3 for vec3.
         * @param glType The OpenGL type of the attribute i.e. GL_FLOAT, GL_INT_VEC2, etc.
         * @param normalize Whether or not to normalize the attribute.
         * @param offset The offset of the attribute in the array.
         * @param drawType The type of draw to use i.e. GL_STATIC_DRAW, GL_DYNAMIC_DRAW, etc.
         */
        template <typename T>
        void setAttrib(const std::string &name, T *value, size_t componentSize, unsigned int glType, bool normalize, unsigned int offset, unsigned int drawType);

        /**
         * Sets the indices of the given attribute's VBO.
         *
         * @param name The name of the attribute.
         * @param indices The indices to set.
         * @param length The length of the indices array.
         * @param drawType The type of draw to use i.e. GL_STATIC_DRAW, GL_DYNAMIC_DRAW, etc.
         */
        void setIndices(const std::string &name, unsigned int *indices, size_t length, unsigned int drawType);

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

        unsigned int programId;
        unsigned int VAO;
        unsigned int drawMode = GL_TRIANGLES;

        struct AttribIndices
        {
            unsigned int *indices;
            size_t length;
            unsigned int drawType;
        };

        struct VertexAttrib
        {
            void *value;
            size_t componentSize;
            unsigned int glType;
            bool normalize;
            unsigned int stride;
            unsigned int offset;
            unsigned int drawType;

            AttribIndices *indices;
            unsigned int VBO;
            unsigned int EBO;
        };

        std::unordered_map<std::string, VertexAttrib> attribValues;
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
         *
         * @returns The id of the VAO that was bound.
         */
        unsigned int createVAO();

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
    };
}