#pragma once

#include "Uniform.h"
#include "VertexAttrib.h"
#include "VertexIndices.h"

#include <string>
#include <unordered_map>
#include <unordered_set>

namespace Rendering
{
    class Shader
    {
    public:
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
         *
         * Will bind all uniforms and vertex attributes before drawing.
         *
         * @param drawMode The draw mode to use, i.e. GL_TRIANGLES, GL_POINTS, GL_LINES, etc.
         * @param drawCount The number of vertices to draw.
         * @param offset The offset in the arrays or indices to start drawing from.
         */
        void draw(GLenum drawMode, size_t drawCount, size_t offset = 0);

        /**
         * Draws the shader using instanced rendering.
         *
         * Will bind all uniforms and vertex attributes before drawing.
         *
         * @param instanceCount The number of instances to draw.
         * @param drawMode The draw mode to use, i.e. GL_TRIANGLES, GL_POINTS, GL_LINES, etc.
         * @param drawCount The number of vertices to draw.
         * @param offset The offset in the arrays or indices to start drawing from.
         */
        void drawInstanced(size_t instanceCount, GLenum drawMode, size_t drawCount, size_t offset = 0);

        /**
         * Sets or updates the uniform value.
         *
         * @param name The name of the uniform.
         *
         * @throws std::invalid_argument If the uniform is not found.
         */
        void uniform(UniformBase *uniform);

        /**
         * Sets or updates the uniform values.
         *
         * @param uniforms The uniforms to set or update.
         *
         * @throws std::invalid_argument If the uniform is not found.
         */
        void uniform(const std::vector<UniformBase *> &uniforms);

        /**
         * Sets or updates the uniform values.
         *
         * @param uniforms The uniforms to set or update.
         *
         * @throws std::invalid_argument If the uniform is not found.
         */
        void uniform(const std::unordered_map<std::string, UniformBase *> &uniforms);

        /**
         * Sets or updates the vertex attribute value.
         *
         * @param name The name of the vertex attribute.
         *
         * @throws std::invalid_argument If the vertex attribute is not found.
         */
        void attrib(VertexAttribBase *attrib);

        /**
         * Sets or updates the vertex attribute values.
         *
         * @param attribs The vertex attributes to set or update.
         *
         * @throws std::invalid_argument If the vertex attribute is not found.
         */
        void attrib(const std::vector<VertexAttribBase *> &attribs);

        /**
         * Sets or updates the vertex attribute values.
         *
         * @param attribs The vertex attributes to set or update.
         *
         * @throws std::invalid_argument If the vertex attribute is not found.
         */
        void attrib(const std::unordered_map<std::string, VertexAttribBase *> &attribs);

        /**
         * Sets the vertex indices.
         *
         * @param indices The vertex indices.
         */
        void indices(VertexIndices *indices);

        /**
         * Gets whether the shader is in use.
         *
         * Makes an OpenGL call.
         *
         * @returns True if the shader is in use, false otherwise.
         */
        bool inUse();

        /**
         * Gets whether the shader is loaded.
         *
         * @returns True if the shader is loaded, false otherwise.
         */
        bool isLoaded();

        /**
         * Gets whether the shader has the given uniform.
         *
         * Does not check if the uniform has been set.
         *
         * @param name The name of the uniform.
         *
         * @returns True if the shader has the uniform, false otherwise.
         */
        bool hasUniform(const std::string &name);

        /**
         * Gets whether the shader has the given vertex attribute.
         *
         * Does not check if the vertex attribute has been set.
         *
         * @param name The name of the vertex attribute.
         *
         * @returns True if the shader has the vertex attribute, false otherwise.
         */
        bool hasAttrib(const std::string &name);

    private:
        /**
         * Whether or not the shader program has been loaded.
         */
        bool loaded = false;

        /**
         * The OpenGL shader program ID.
         *
         * 0 if the shader program has not been loaded.
         */
        unsigned int program = 0;

        /**
         * The OpenGL vertex array object ID.
         */
        unsigned int VAO = 0;

        /**
         * Represents information about a uniform.
         */
        struct UniformInfo
        {
            /**
             * The name of the uniform.
             */
            std::string name;

            /**
             * The location of the uniform.
             */
            int location;

            /**
             * The type of the uniform.
             */
            GLenum type;

            /**
             * The size of the uniform variable.
             */
            int size;
        };

        /**
         * Info about all the uniforms in the shader.
         */
        std::unordered_map<std::string, UniformInfo> uniformInfo;

        /**
         * Represents information about a vertex attribute.
         */
        struct VertexAttribInfo
        {
            /**
             * The name of the vertex attribute.
             */
            std::string name;

            /**
             * The index of the vertex attribute.
             */
            int location;

            /**
             * The type of the vertex attribute.
             */
            GLenum type;

            /**
             * The size of the attribute variable.
             */
            int size;

            /**
             * The vbo the vertex attribute is bound to.
             *
             * 0 if the vertex attribute is not bound to a vbo.
             */
            unsigned int vbo = 0;
        };

        /**
         * Info about all the vertex attributes in the shader.
         */
        std::unordered_map<std::string, VertexAttribInfo> attribInfo;

        /**
         * The uniforms set.
         */
        std::unordered_map<std::string, UniformBase *> uniforms;

        /**
         * Whether or not the vertex attributes need to be updated.
         */
        bool attribsNeedUpdate = false;

        /**
         * The vertex attributes set.
         */
        std::unordered_map<std::string, VertexAttribBase *> vertexAttribs;

        /**
         * The vertex indices buffer object.
         */
        unsigned int EBO = 0;

        /**
         * Whether or not the vertex indices need to be updated.
         */
        bool indicesNeedUpdate = false;

        /**
         * The vertex indices set.
         */
        VertexIndices *vertexIndices = nullptr;

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
         * Replaces injectable variables in the shader source with the runtime values.
         *
         * Injectable variables are:
         * - `__MAX_TEXTURE_UNITS__` - The max number of texture units supported by the GPU.
         *
         * @param source The shader source.
         *
         * @returns The shader source with the injectable variables replaced.
         */
        std::string injectShaderVariables(const std::string &source);

        /**
         * Injects the shader functions into the shader source.
         *
         * Injects the following functions if specified:
         * - `__getTexture__` - Gets the texture color from the given texture unit, called with `getTexture(uint textureUnit, vec2 textureCoord)`.
         *
         * @param source The shader source.
         *
         * @returns The shader source with the shader functions injected.
         */
        std::string injectShaderFunctions(const std::string &source);

        /**
         * Binds all the currently set uniforms.
         */
        void bindUniforms();

        /**
         * Updates the VAO if needed and binds it.
         */
        void bindVertexAttribs();

        /**
         * Gets the names, location and type of all the uniforms in the shader.
         *
         * @returns The names, location and type of all the uniforms in the shader.
         */
        std::unordered_map<std::string, UniformInfo> getUniformInfo();

        /**
         * Gets the names, location and type of all the vertex attributes in the shader.
         *
         * @returns The names, location and type of all the vertex attributes in the shader.
         */
        std::unordered_map<std::string, VertexAttribInfo> getVertexAttribInfo();
    };
}