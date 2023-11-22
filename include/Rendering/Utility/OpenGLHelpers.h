#include "../../externals/glad/glad.h"

namespace Rendering
{
    void GLAPIENTRY openGlMessageCallback(GLenum source,
                                          GLenum type,
                                          GLuint id,
                                          GLenum severity,
                                          GLsizei length,
                                          const GLchar *message,
                                          const void *userParam);
}