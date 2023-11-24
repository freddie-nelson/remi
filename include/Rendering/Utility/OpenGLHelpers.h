#pragma once

#include "../../externals/glad/gl.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Rendering
{
    void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity,
                                GLsizei length, const char *message, const void *userParam);
}
