#pragma once

#include "../../gl.h"
#include "../Shader/Uniform.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <string>

namespace Rendering
{
    /**
     * Defines the OpenGL debug callback function.
     */
    // void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity,
    //                             GLsizei length, const char *message, const void *userParam);

    /**
     * Checks if the given OpenGL type is an int type.
     *
     * This also includes unsigned int types and vectors.
     *
     * @param type The OpenGL type to check.
     *
     * @returns True if the type is an int type, false otherwise.
     */
    bool glIsTypeInt(GLenum type);

    /**
     * Checks if the given function is a valid OpenGL alpha blending function.
     *
     * @param function The OpenGL alpha blending function to check.
     *
     * @returns True if the function is a valid OpenGL alpha blending function, false otherwise.
     */
    bool glIsValidAlphaBlendingFunction(GLenum function);

    /**
     * Checks if the given mode is a valid OpenGL draw mode.
     *
     * @param mode The OpenGL draw mode to check.
     * @param safe If false, then the function will throw an exception if the mode is not valid.
     *
     * @returns True if the mode is a valid OpenGL draw mode, false otherwise.
     *
     * @throws std::runtime_error If the mode is not valid and safe is false.
     */
    bool glIsValidDrawMode(GLenum mode, bool safe = true);

    /**
     * Gets the maximum number of texture units supported by the GPU.
     *
     * @returns The maximum number of texture units supported by the GPU.
     */
    unsigned int glGetMaxTextureUnits();

    /**
     * Gets the number of components in the given OpenGL type.
     *
     * i.e. GL_FLOAT_VEC3 would return 3, GL_INT would return 1.
     *
     * @param type The OpenGL type.
     *
     * @returns The number of components in the given OpenGL type.
     *
     * @throws std::runtime_error If the number of components could not be determined.
     */
    unsigned int glGetNumComponents(GLenum type);

    /**
     * Gets the size of the given OpenGL type.
     *
     * If the type is not a matrix, then this will return -1.
     *
     * @param type The OpenGL type.
     *
     * @returns The matrix size of the given OpenGL type.
     */
    int glGetMatrixSize(GLenum type);

    /**
     * Calls the correct glUniform function for the given uniform.
     *
     * The shader program to bind the uniform to must be in use before calling this function.
     *
     * @param location The location of the uniform.
     * @param uniform The uniform to set.
     *
     * @throws std::runtime_error If the uniform type is not supported.
     */
    void glUniform(int location, UniformBase *uniform);

    /**
     * Gets the string representation of the given OpenGL type.
     *
     * i.e. GL_FLOAT would return "GL_FLOAT".
     *
     * @param type The OpenGL type.
     *
     * @returns The string representation of the given OpenGL type.
     */
    std::string glTypeToString(GLenum type);

    /**
     * Gets the OpenGL component type of the given gl type.
     *
     * i.e. GL_FLOAT_VEC3 would return GL_FLOAT.
     *
     * @param type The gl type.
     *
     * @returns The OpenGL component type of the given gl type.
     */
    GLenum glGetComponentType(GLenum type);

    /**
     * Gets the string representation of the given framebuffer status.
     *
     * @param status The framebuffer status.
     *
     * @returns The string representation of the given framebuffer status.
     */
    std::string glFramebufferStatusToString(GLenum status);
}
