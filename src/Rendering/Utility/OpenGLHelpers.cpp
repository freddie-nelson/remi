#include "../../../include/Rendering/Utility/OpenGLHelpers.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
#include <stdexcept>

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
    case GL_INT_SAMPLER_2D:
    case GL_INT_SAMPLER_3D:
    case GL_INT_SAMPLER_CUBE:
    case GL_INT_SAMPLER_2D_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_2D:
    case GL_UNSIGNED_INT_SAMPLER_3D:
    case GL_UNSIGNED_INT_SAMPLER_CUBE:
    case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
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
        return true;
    default:
        return false;
    }
}

bool Rendering::glIsValidDrawMode(GLenum mode, bool safe)
{
    switch (mode)
    {
    case GL_POINTS:
    case GL_LINE_STRIP:
    case GL_LINE_LOOP:
    case GL_LINES:
    case GL_TRIANGLE_STRIP:
    case GL_TRIANGLE_FAN:
    case GL_TRIANGLES:
        return true;
    default:
        if (safe)
            return false;
        else
            throw std::runtime_error("Invalid draw mode.");
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

unsigned int Rendering::glGetNumComponents(GLenum type)
{
    switch (type)
    {
    case GL_FLOAT:
    case GL_INT:
    case GL_UNSIGNED_INT:
    case GL_BOOL:
    case GL_SAMPLER_2D:
    case GL_SAMPLER_3D:
    case GL_SAMPLER_CUBE:
    case GL_SAMPLER_2D_ARRAY:
        return 1;
    case GL_FLOAT_VEC2:
    case GL_INT_VEC2:
    case GL_UNSIGNED_INT_VEC2:
    case GL_BOOL_VEC2:
        return 2;
    case GL_FLOAT_VEC3:
    case GL_INT_VEC3:
    case GL_UNSIGNED_INT_VEC3:
    case GL_BOOL_VEC3:
        return 3;
    case GL_FLOAT_VEC4:
    case GL_INT_VEC4:
    case GL_UNSIGNED_INT_VEC4:
    case GL_BOOL_VEC4:
        return 4;
    case GL_FLOAT_MAT2:
        return 4;
    case GL_FLOAT_MAT3:
        return 9;
    case GL_FLOAT_MAT4:
        return 16;
    default:
        throw std::runtime_error("Could not determine number of components in OpenGL type.");
    }
}

int Rendering::glGetMatrixSize(GLenum type)
{
    switch (type)
    {
    case GL_FLOAT_MAT2:
        return 2;
    case GL_FLOAT_MAT3:
        return 3;
    case GL_FLOAT_MAT4:
        return 4;
    default:
        return -1;
    }
}

void Rendering::glUniform(int location, UniformBase *uniform)
{
    auto type = uniform->getGLType();
    auto size = uniform->size();
    auto value = uniform->getValuePointer();
    auto isArray = uniform->isArray();

    switch (type)
    {
    case GL_FLOAT:
        if (isArray)
            glUniform1fv(location, size, &(*reinterpret_cast<std::vector<float> *>(value))[0]);
        else
            glUniform1fv(location, size, reinterpret_cast<float *>(value));
        break;

    case GL_INT:
        if (isArray)
            glUniform1iv(location, size, &(*reinterpret_cast<std::vector<int> *>(value))[0]);
        else
            glUniform1iv(location, size, reinterpret_cast<int *>(value));
        break;

    case GL_UNSIGNED_INT:
        if (isArray)
            glUniform1uiv(location, size, &(*reinterpret_cast<std::vector<unsigned int> *>(value))[0]);
        else
            glUniform1uiv(location, size, reinterpret_cast<unsigned int *>(value));
        break;

    case GL_BOOL:
        if (isArray)
            glUniform1iv(location, size, &(*reinterpret_cast<std::vector<int> *>(value))[0]);
        else
            glUniform1iv(location, size, reinterpret_cast<int *>(value));
        break;

    case GL_SAMPLER_2D:
    case GL_SAMPLER_3D:
    case GL_SAMPLER_CUBE:
    case GL_SAMPLER_2D_ARRAY:
        if (isArray)
            glUniform1iv(location, size, &(*reinterpret_cast<std::vector<int> *>(value))[0]);
        else
            glUniform1iv(location, size, reinterpret_cast<int *>(value));
        break;

    case GL_FLOAT_VEC2:
        if (isArray)
            glUniform2fv(location, size, glm::value_ptr((*reinterpret_cast<std::vector<glm::vec2> *>(value))[0]));
        else
            glUniform2fv(location, size, glm::value_ptr(*reinterpret_cast<glm::vec2 *>(value)));
        break;

    case GL_INT_VEC2:
        if (isArray)
            glUniform2iv(location, size, glm::value_ptr((*reinterpret_cast<std::vector<glm::ivec2> *>(value))[0]));
        else
            glUniform2iv(location, size, glm::value_ptr(*reinterpret_cast<glm::ivec2 *>(value)));
        break;

    case GL_UNSIGNED_INT_VEC2:
        if (isArray)
            glUniform2uiv(location, size, glm::value_ptr((*reinterpret_cast<std::vector<glm::uvec2> *>(value))[0]));
        else
            glUniform2uiv(location, size, glm::value_ptr(*reinterpret_cast<glm::uvec2 *>(value)));
        break;

    case GL_BOOL_VEC2:
        if (isArray)
            glUniform2iv(location, size, glm::value_ptr((*reinterpret_cast<std::vector<glm::ivec2> *>(value))[0]));
        else
            glUniform2iv(location, size, glm::value_ptr(*reinterpret_cast<glm::ivec2 *>(value)));
        break;

    case GL_FLOAT_VEC3:
        if (isArray)
            glUniform3fv(location, size, glm::value_ptr((*reinterpret_cast<std::vector<glm::vec3> *>(value))[0]));
        else
            glUniform3fv(location, size, glm::value_ptr(*reinterpret_cast<glm::vec3 *>(value)));
        break;

    case GL_INT_VEC3:
        if (isArray)
            glUniform3iv(location, size, glm::value_ptr((*reinterpret_cast<std::vector<glm::ivec3> *>(value))[0]));
        else
            glUniform3iv(location, size, glm::value_ptr(*reinterpret_cast<glm::ivec3 *>(value)));
        break;

    case GL_UNSIGNED_INT_VEC3:
        if (isArray)
            glUniform3uiv(location, size, glm::value_ptr((*reinterpret_cast<std::vector<glm::uvec3> *>(value))[0]));
        else
            glUniform3uiv(location, size, glm::value_ptr(*reinterpret_cast<glm::uvec3 *>(value)));
        break;

    case GL_BOOL_VEC3:
        if (isArray)
            glUniform3iv(location, size, glm::value_ptr((*reinterpret_cast<std::vector<glm::ivec3> *>(value))[0]));
        else
            glUniform3iv(location, size, glm::value_ptr(*reinterpret_cast<glm::ivec3 *>(value)));
        break;

    case GL_FLOAT_VEC4:
        if (isArray)
            glUniform4fv(location, size, glm::value_ptr((*reinterpret_cast<std::vector<glm::vec4> *>(value))[0]));
        else
            glUniform4fv(location, size, glm::value_ptr(*reinterpret_cast<glm::vec4 *>(value)));
        break;

    case GL_INT_VEC4:
        if (isArray)
            glUniform4iv(location, size, glm::value_ptr((*reinterpret_cast<std::vector<glm::ivec4> *>(value))[0]));
        else
            glUniform4iv(location, size, glm::value_ptr(*reinterpret_cast<glm::ivec4 *>(value)));
        break;

    case GL_UNSIGNED_INT_VEC4:
        if (isArray)
            glUniform4uiv(location, size, glm::value_ptr((*reinterpret_cast<std::vector<glm::uvec4> *>(value))[0]));
        else
            glUniform4uiv(location, size, glm::value_ptr(*reinterpret_cast<glm::uvec4 *>(value)));
        break;

    case GL_BOOL_VEC4:
        if (isArray)
            glUniform4iv(location, size, glm::value_ptr((*reinterpret_cast<std::vector<glm::ivec4> *>(value))[0]));
        else
            glUniform4iv(location, size, glm::value_ptr(*reinterpret_cast<glm::ivec4 *>(value)));
        break;

    case GL_FLOAT_MAT2:
        if (isArray)
            glUniformMatrix2fv(location, size, GL_FALSE, glm::value_ptr((*reinterpret_cast<std::vector<glm::mat2> *>(value))[0]));
        else
            glUniformMatrix2fv(location, size, GL_FALSE, glm::value_ptr(*reinterpret_cast<glm::mat2 *>(value)));
        break;

    case GL_FLOAT_MAT3:
        if (isArray)
            glUniformMatrix3fv(location, size, GL_FALSE, glm::value_ptr((*reinterpret_cast<std::vector<glm::mat3> *>(value))[0]));
        else
            glUniformMatrix3fv(location, size, GL_FALSE, glm::value_ptr(*reinterpret_cast<glm::mat3 *>(value)));
        break;

    case GL_FLOAT_MAT4:
        if (isArray)
            glUniformMatrix4fv(location, size, GL_FALSE, glm::value_ptr((*reinterpret_cast<std::vector<glm::mat4> *>(value))[0]));
        else
            glUniformMatrix4fv(location, size, GL_FALSE, glm::value_ptr(*reinterpret_cast<glm::mat4 *>(value)));
        break;
    }
}

std::string Rendering::glTypeToString(GLenum type)
{
    switch (type)
    {
    case GL_FLOAT:
        return "GL_FLOAT";
    case GL_INT:
        return "GL_INT";
    case GL_UNSIGNED_INT:
        return "GL_UNSIGNED_INT";
    case GL_BOOL:
        return "GL_BOOL";
    case GL_SAMPLER_2D:
        return "GL_SAMPLER_2D";
    case GL_SAMPLER_3D:
        return "GL_SAMPLER_3D";
    case GL_SAMPLER_CUBE:
        return "GL_SAMPLER_CUBE";
    case GL_SAMPLER_2D_ARRAY:
        return "GL_SAMPLER_2D_ARRAY";
    case GL_FLOAT_VEC2:
        return "GL_FLOAT_VEC2";
    case GL_INT_VEC2:
        return "GL_INT_VEC2";
    case GL_UNSIGNED_INT_VEC2:
        return "GL_UNSIGNED_INT_VEC2";
    case GL_BOOL_VEC2:
        return "GL_BOOL_VEC2";
    case GL_FLOAT_VEC3:
        return "GL_FLOAT_VEC3";
    case GL_INT_VEC3:
        return "GL_INT_VEC3";
    case GL_UNSIGNED_INT_VEC3:
        return "GL_UNSIGNED_INT_VEC3";
    case GL_BOOL_VEC3:
        return "GL_BOOL_VEC3";
    case GL_FLOAT_VEC4:
        return "GL_FLOAT_VEC4";
    case GL_INT_VEC4:
        return "GL_INT_VEC4";
    case GL_UNSIGNED_INT_VEC4:
        return "GL_UNSIGNED_INT_VEC4";
    case GL_BOOL_VEC4:
        return "GL_BOOL_VEC4";
    case GL_FLOAT_MAT2:
        return "GL_FLOAT_MAT2";
    case GL_FLOAT_MAT3:
        return "GL_FLOAT_MAT3";
    case GL_FLOAT_MAT4:
        return "GL_FLOAT_MAT4";
    }

    throw std::runtime_error("Could not determine string representation of OpenGL type.");
}

GLenum Rendering::glGetComponentType(GLenum type)
{
    switch (type)
    {
    case GL_FLOAT_VEC2:
    case GL_FLOAT_VEC3:
    case GL_FLOAT_VEC4:
    case GL_FLOAT_MAT2:
    case GL_FLOAT_MAT3:
    case GL_FLOAT_MAT4:
        return GL_FLOAT;
    case GL_INT_VEC2:
    case GL_INT_VEC3:
    case GL_INT_VEC4:
        return GL_INT;
    case GL_UNSIGNED_INT_VEC2:
    case GL_UNSIGNED_INT_VEC3:
    case GL_UNSIGNED_INT_VEC4:
        return GL_UNSIGNED_INT;
    case GL_BOOL_VEC2:
    case GL_BOOL_VEC3:
    case GL_BOOL_VEC4:
        return GL_BOOL;
    default:
        return type;
    }
}