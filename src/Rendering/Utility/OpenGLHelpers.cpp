#include "../../../include/Rendering/Utility/OpenGLHelpers.h"

#include <iostream>

void GLAPIENTRY Rendering::openGlMessageCallback(GLenum source,
                                                 GLenum type,
                                                 GLuint id,
                                                 GLenum severity,
                                                 GLsizei length,
                                                 const GLchar *message,
                                                 const void *userParam)
{
    std::cerr << "GL CALLBACK: " << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "")
              << " type = 0x" << std::hex << type << ", severity = 0x" << severity
              << ", message = " << message << std::endl;
}
