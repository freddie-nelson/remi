#include "../../../include/Rendering/Utility/OpenGLHelpers.h"

#include <iostream>

// void APIENTRY Rendering::glDebugOutput(GLenum source,
//                                        GLenum type,
//                                        unsigned int id,
//                                        GLenum severity,
//                                        GLsizei length,
//                                        const char *message,
//                                        const void *userParam)
// {
//     // ignore non-significant error/warning codes
//     if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
//         return;

//     std::cout << "---------------" << std::endl;
//     std::cout << "Debug message (" << id << "): " << message << std::endl;

//     switch (source)
//     {
//     case GL_DEBUG_SOURCE_API:
//         std::cout << "Source: API";
//         break;
//     case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
//         std::cout << "Source: Window System";
//         break;
//     case GL_DEBUG_SOURCE_SHADER_COMPILER:
//         std::cout << "Source: Shader Compiler";
//         break;
//     case GL_DEBUG_SOURCE_THIRD_PARTY:
//         std::cout << "Source: Third Party";
//         break;
//     case GL_DEBUG_SOURCE_APPLICATION:
//         std::cout << "Source: Application";
//         break;
//     case GL_DEBUG_SOURCE_OTHER:
//         std::cout << "Source: Other";
//         break;
//     }
//     std::cout << std::endl;

//     switch (type)
//     {
//     case GL_DEBUG_TYPE_ERROR:
//         std::cout << "Type: Error";
//         break;
//     case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
//         std::cout << "Type: Deprecated Behaviour";
//         break;
//     case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
//         std::cout << "Type: Undefined Behaviour";
//         break;
//     case GL_DEBUG_TYPE_PORTABILITY:
//         std::cout << "Type: Portability";
//         break;
//     case GL_DEBUG_TYPE_PERFORMANCE:
//         std::cout << "Type: Performance";
//         break;
//     case GL_DEBUG_TYPE_MARKER:
//         std::cout << "Type: Marker";
//         break;
//     case GL_DEBUG_TYPE_PUSH_GROUP:
//         std::cout << "Type: Push Group";
//         break;
//     case GL_DEBUG_TYPE_POP_GROUP:
//         std::cout << "Type: Pop Group";
//         break;
//     case GL_DEBUG_TYPE_OTHER:
//         std::cout << "Type: Other";
//         break;
//     }
//     std::cout << std::endl;

//     switch (severity)
//     {
//     case GL_DEBUG_SEVERITY_HIGH:
//         std::cout << "Severity: high";
//         break;
//     case GL_DEBUG_SEVERITY_MEDIUM:
//         std::cout << "Severity: medium";
//         break;
//     case GL_DEBUG_SEVERITY_LOW:
//         std::cout << "Severity: low";
//         break;
//     case GL_DEBUG_SEVERITY_NOTIFICATION:
//         std::cout << "Severity: notification";
//         break;
//     }
//     std::cout << std::endl;
//     std::cout << std::endl;
// }

bool Rendering::glIsTypeInt(GLenum type)
{
    switch (type)
    {
    case GL_INT:
    case GL_UNSIGNED_INT:
    case GL_INT_VEC2:
    case GL_INT_VEC3:
    case GL_INT_VEC4:
    case GL_UNSIGNED_INT_VEC2:
    case GL_UNSIGNED_INT_VEC3:
    case GL_UNSIGNED_INT_VEC4:
    case GL_BOOL:
    case GL_BOOL_VEC2:
    case GL_BOOL_VEC3:
    case GL_BOOL_VEC4:
    case GL_INT_SAMPLER_1D:
    case GL_INT_SAMPLER_2D:
    case GL_INT_SAMPLER_3D:
    case GL_INT_SAMPLER_CUBE:
    case GL_INT_SAMPLER_1D_ARRAY:
    case GL_INT_SAMPLER_2D_ARRAY:
    case GL_INT_SAMPLER_2D_MULTISAMPLE:
    case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
    case GL_INT_SAMPLER_BUFFER:
    case GL_INT_SAMPLER_2D_RECT:
    case GL_UNSIGNED_INT_SAMPLER_1D:
    case GL_UNSIGNED_INT_SAMPLER_2D:
    case GL_UNSIGNED_INT_SAMPLER_3D:
    case GL_UNSIGNED_INT_SAMPLER_CUBE:
    case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
    case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_BUFFER:
    case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
        return true;
    default:
        return false;
    }
}

bool Rendering::glIsValidAlphaBlendingFunction(GLenum function)
{
    switch (function)
    {
    case GL_ZERO:
    case GL_ONE:
    case GL_SRC_COLOR:
    case GL_ONE_MINUS_SRC_COLOR:
    case GL_DST_COLOR:
    case GL_ONE_MINUS_DST_COLOR:
    case GL_SRC_ALPHA:
    case GL_ONE_MINUS_SRC_ALPHA:
    case GL_DST_ALPHA:
    case GL_ONE_MINUS_DST_ALPHA:
    case GL_CONSTANT_COLOR:
    case GL_ONE_MINUS_CONSTANT_COLOR:
    case GL_CONSTANT_ALPHA:
    case GL_ONE_MINUS_CONSTANT_ALPHA:
    case GL_SRC_ALPHA_SATURATE:
    case GL_SRC1_COLOR:
    case GL_ONE_MINUS_SRC1_COLOR:
    case GL_SRC1_ALPHA:
    case GL_ONE_MINUS_SRC1_ALPHA:
        return true;
    default:
        return false;
    }
}

bool maxTextureUnitsChecked = false;
unsigned int maxTextureUnits = 0;

unsigned int Rendering::glGetMaxTextureUnits()
{
    if (maxTextureUnitsChecked)
        return maxTextureUnits;

    int temp;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &temp);

    return maxTextureUnits = static_cast<unsigned int>(temp);
}