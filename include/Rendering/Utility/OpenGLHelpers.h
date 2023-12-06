#pragma once

#include "../../externals/glad/gl.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Rendering
{
    /**
     * Defines the OpenGL debug callback function.
     */
    void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity,
                                GLsizei length, const char *message, const void *userParam);

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
}
