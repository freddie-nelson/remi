#include "../../externals/glad/glad.h"

namespace Rendering
{
    void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity,
                                GLsizei length, const char *message, const void *userParam);
}
