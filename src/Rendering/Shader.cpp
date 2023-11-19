#include "../../include/Rendering/Shader.h"
#include "../../include/Rendering/Utility/FileHandling.h"
#include "../../include/Rendering/Utility/GlmHelpers.h"

#include "../../include/externals/glad/glad.h"
#include <stdexcept>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

Rendering::Shader::Shader()
{
}

Rendering::Shader::~Shader()
{
    unbind();

    if (loaded)
    {
        glDeleteProgram(programId);
    }
}

bool Rendering::Shader::loadFromFile(std::string vertex, std::string fragment)
{
    try
    {
        std::string vertexSource = loadFile(vertex);
        std::string fragmentSource = loadFile(fragment);

        return loadFromSource(vertexSource, fragmentSource);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return false;
    }
}

bool Rendering::Shader::loadFromSource(std::string vertex, std::string fragment)
{
    if (loaded)
    {
        throw std::runtime_error("Shader has already been loaded. Can only load shaders once.");
    }

    // vertex shader
    unsigned int vertexShader;
    if (!compileShaderSource(vertex, GL_VERTEX_SHADER, vertexShader))
    {
        return false;
    }

    // fragment shader
    unsigned int fragmentShader;
    if (!compileShaderSource(vertex, GL_FRAGMENT_SHADER, fragmentShader))
    {
        return false;
    }

    // shader program
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // check for linking errors
    int success;
    char infoLog[512];

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "Failed to link shader program. Info: " << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    shaderProgram = programId;
    loaded = true;

    return true;
}

void Rendering::Shader::use()
{
    if (!loaded)
    {
        throw std::runtime_error("Shader must be loaded before it can be used.");
    }

    glUseProgram(programId);
}

void Rendering::Shader::unbind()
{
    if (inUse())
    {
        glUseProgram(0);
    }
}

void Rendering::Shader::setUniform(std::string name, void *value)
{
    checkUniformSetRules(name);

    int location = getUniformLocation(name);
    unsigned int type = getUniformType(name);

    switch (type)
    {
    // INTS
    case GL_FLOAT:
    {
        auto v = *static_cast<float *>(value);
        glUniform1f(location, v);
        break;
    }
    case GL_FLOAT_VEC2:
    {
        auto v = *static_cast<glm::vec2 *>(value);
        glUniform2f(location, v[0], v[1]);
        break;
    }
    case GL_FLOAT_VEC3:
    {
        auto v = *static_cast<glm::vec3 *>(value);
        glUniform3f(location, v[0], v[1], v[2]);
        break;
    }
    case GL_FLOAT_VEC4:
    {
        auto v = *static_cast<glm::vec4 *>(value);
        glUniform4i(location, v[0], v[1], v[2], v[3]);
        break;
    }

    // INTS
    case GL_INT:
    {
        auto v = *static_cast<int *>(value);
        glUniform1i(location, v);
        break;
    }
    case GL_INT_VEC2:
    {
        auto v = *static_cast<glm::ivec2 *>(value);
        glUniform2i(location, v[0], v[1]);
        break;
    }
    case GL_INT_VEC3:
    {
        auto v = *static_cast<glm::ivec3 *>(value);
        glUniform3i(location, v[0], v[1], v[2]);
        break;
    }
    case GL_INT_VEC4:
    {
        auto v = *static_cast<glm::ivec4 *>(value);
        glUniform4i(location, v[0], v[1], v[2], v[3]);
        break;
    }

    // UNSIGNED INTS
    case GL_UNSIGNED_INT:
    {
        auto v = *static_cast<unsigned int *>(value);
        glUniform1ui(location, v);
        break;
    }
    case GL_UNSIGNED_INT_VEC2:
    {
        auto v = *static_cast<glm::uvec2 *>(value);
        glUniform2ui(location, v[0], v[1]);
        break;
    }
    case GL_UNSIGNED_INT_VEC3:
    {
        auto v = *static_cast<glm::uvec3 *>(value);
        glUniform3ui(location, v[0], v[1], v[2]);
        break;
    }
    case GL_UNSIGNED_INT_VEC4:
    {
        auto v = *static_cast<glm::uvec4 *>(value);
        glUniform4ui(location, v[0], v[1], v[2], v[3]);
        break;
    }

    // SQUARE MATRICES
    case GL_FLOAT_MAT2:
    {
        auto v = *static_cast<glm::mat2 *>(value);
        glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(v));
        break;
    }
    case GL_FLOAT_MAT3:
    {
        auto v = *static_cast<glm::mat3 *>(value);
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(v));
        break;
    }
    case GL_FLOAT_MAT4:
    {
        auto v = *static_cast<glm::mat4 *>(value);
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(v));
        break;
    }

    // UNSUPPORTED
    default:
        throw std::runtime_error("Uniform " + name + " has an unsupported type.");
        break;
    }
}

void Rendering::Shader::setUniformArray(std::string name, void *value, size_t length)
{
    checkUniformSetRules(name);

    int location = getUniformLocation(name);
    unsigned int type = getUniformType(name);

    switch (type)
    {
    // INTS
    case GL_FLOAT:
    {
        auto v = static_cast<float *>(value);
        glUniform1fv(location, length, v);
        break;
    }
    case GL_FLOAT_VEC2:
    {
        auto v = static_cast<glm::vec2 *>(value);
        auto flattened = flattenGlmVecArray(v, length);
        glUniform2fv(location, length, flattened);
        break;
    }
    case GL_FLOAT_VEC3:
    {
        auto v = static_cast<glm::vec3 *>(value);
        auto flattened = flattenGlmVecArray(v, length);
        glUniform3fv(location, length, flattened);
        break;
    }
    case GL_FLOAT_VEC4:
    {
        auto v = static_cast<glm::vec4 *>(value);
        auto flattened = flattenGlmVecArray(v, length);
        glUniform4fv(location, length, flattened);
        break;
    }

    // INTS
    case GL_INT:
    {
        auto v = static_cast<int *>(value);
        glUniform1iv(location, length, v);
        break;
    }
    case GL_INT_VEC2:
    {
        auto v = static_cast<glm::ivec2 *>(value);
        auto flattened = flattenGlmVecArray(v, length);
        glUniform2iv(location, length, flattened);
        break;
    }
    case GL_INT_VEC3:
    {
        auto v = static_cast<glm::ivec3 *>(value);
        auto flattened = flattenGlmVecArray(v, length);
        glUniform3iv(location, length, flattened);
        break;
    }
    case GL_INT_VEC4:
    {
        auto v = static_cast<glm::ivec4 *>(value);
        auto flattened = flattenGlmVecArray(v, length);
        glUniform4iv(location, length, flattened);
        break;
    }

    // UNSIGNED INTS
    case GL_UNSIGNED_INT:
    {
        auto v = static_cast<unsigned int *>(value);
        glUniform1uiv(location, length, v);
        break;
    }
    case GL_UNSIGNED_INT_VEC2:
    {
        auto v = static_cast<glm::uvec2 *>(value);
        auto flattened = flattenGlmVecArray(v, length);
        glUniform2uiv(location, length, flattened);
        break;
    }
    case GL_UNSIGNED_INT_VEC3:
    {
        auto v = static_cast<glm::uvec3 *>(value);
        auto flattened = flattenGlmVecArray(v, length);
        glUniform3uiv(location, length, flattened);
        break;
    }
    case GL_UNSIGNED_INT_VEC4:
    {
        auto v = static_cast<glm::uvec4 *>(value);
        auto flattened = flattenGlmVecArray(v, length);
        glUniform4uiv(location, length, flattened);
        break;
    }

    // SQUARE MATRICES
    case GL_FLOAT_MAT2:
    {
        auto v = static_cast<glm::mat2 *>(value);
        auto flattened = flattenGlmMatArray(v, length);
        glUniformMatrix2fv(location, 1, GL_FALSE, flattened);
        break;
    }
    case GL_FLOAT_MAT3:
    {
        auto v = static_cast<glm::mat3 *>(value);
        auto flattened = flattenGlmMatArray(v, length);
        glUniformMatrix3fv(location, 1, GL_FALSE, flattened);
        break;
    }
    case GL_FLOAT_MAT4:
    {
        auto v = static_cast<glm::mat4 *>(value);
        auto flattened = flattenGlmMatArray(v, length);
        glUniformMatrix4fv(location, 1, GL_FALSE, flattened);
        break;
    }

    // UNSUPPORTED
    default:
        throw std::runtime_error("Uniform " + name + " has an unsupported type.");
        break;
    }
}

void Rendering::Shader::setAttrib(std::string name, void *value)
{
    if (!loaded)
    {
        throw std::runtime_error("Shader must be loaded before attributes can be set.");
    }

    int location = getAttribLocation(name);
}

bool Rendering::Shader::inUse()
{
    if (!loaded)
        return false;

    int currentProgramId;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgramId);

    return currentProgramId == programId;
}

bool Rendering::Shader::compileShaderSource(std::string source, unsigned int shaderType, unsigned int &shader)
{
    if (shaderType != GL_VERTEX_SHADER && shaderType != GL_FRAGMENT_SHADER)
    {
        throw std::invalid_argument("shaderType must be either GL_VERTEX_SHADER or GL_FRAGMENT_SHADER.");
    }

    const char *sourceStr = source.c_str();

    unsigned int shaderId = glCreateShader(shaderType);
    glShaderSource(shaderId, 1, &sourceStr, NULL);
    glCompileShader(shaderId);

    // handle compile errors
    int success;
    char infoLog[512];

    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
        std::cerr << "Failed to compile vertex shader. Info: " << infoLog << std::endl;
        return false;
    }

    shader = shaderId;
    return true;
}

int Rendering::Shader::getUniformLocation(std::string name)
{
    if (!loaded)
    {
        throw std::runtime_error("Shader has not been loaded.");
    }

    int location = glGetUniformLocation(programId, name.c_str());
    if (location == -1)
    {
        throw std::runtime_error("Uniform " + name + " does not exist.");
    }

    return location;
}

int Rendering::Shader::getAttribLocation(std::string name)
{
    if (!loaded)
    {
        throw std::runtime_error("Shader has not been loaded.");
    }

    int location = glGetAttribLocation(programId, name.c_str());
    if (location == -1)
    {
        throw std::runtime_error("Attribute " + name + " does not exist.");
    }

    return location;
}

unsigned int Rendering::Shader::getUniformType(std::string name)
{
    if (!loaded)
    {
        throw std::runtime_error("Shader has not been loaded.");
    }

    int location = getUniformLocation(name);

    unsigned int type;
    glGetActiveUniform(programId, location, NULL, NULL, NULL, &type, NULL);

    return type;
}

unsigned int Rendering::Shader::getAttribType(std::string name)
{
    if (!loaded)
    {
        throw std::runtime_error("Shader has not been loaded.");
    }

    int location = getAttribLocation(name);

    unsigned int type;
    glGetActiveAttrib(programId, location, NULL, NULL, NULL, &type, NULL);

    return type;
}

void Rendering::Shader::checkUniformSetRules(std::string name)
{
    if (!loaded)
    {
        throw std::runtime_error("Shader must be loaded before uniforms can be set.");
    }

    getUniformLocation(name);
}
