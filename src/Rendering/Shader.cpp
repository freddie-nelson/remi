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

bool Rendering::Shader::loadFromFile(const std::string &vertex, const std::string &fragment)
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

bool Rendering::Shader::loadFromSource(const std::string &vertex, const std::string &fragment)
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

    updateUniforms = true;
    updateUniformArrays = true;
    updateAttributes = true;
    updateAttributesAndUniforms();

    glUseProgram(programId);
    glBindVertexArray(VAO);
}

void Rendering::Shader::unbind()
{
    if (inUse())
    {
        glBindVertexArray(0);
        glUseProgram(0);
    }
}

void Rendering::Shader::draw()
{
    if (!inUse())
    {
        throw std::runtime_error("Shader must be in use before it can be drawn.");
    }

    if (hasIndices)
    {
        glDrawElements(drawMode, attribValues["indices"].indices->length, GL_UNSIGNED_INT, 0);
    }
    else
    {
        glDrawArrays(drawMode, 0, attribValues["vertices"].componentSize);
    }
}

void Rendering::Shader::setDrawMode(unsigned int drawMode)
{
    if (drawMode != GL_POINTS && drawMode != GL_LINE_STRIP && drawMode != GL_LINE_LOOP && drawMode != GL_LINES && drawMode != GL_LINE_STRIP_ADJACENCY && drawMode != GL_LINES_ADJACENCY && drawMode != GL_TRIANGLE_STRIP && drawMode != GL_TRIANGLE_FAN && drawMode != GL_TRIANGLES && drawMode != GL_TRIANGLE_STRIP_ADJACENCY && drawMode != GL_TRIANGLES_ADJACENCY && drawMode != GL_PATCHES)
    {
        throw std::invalid_argument("drawMode must be one of the following: GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES, GL_LINE_STRIP_ADJACENCY, GL_LINES_ADJACENCY, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_TRIANGLES, GL_TRIANGLE_STRIP_ADJACENCY, GL_TRIANGLES_ADJACENCY, GL_PATCHES");
    }

    this->drawMode = drawMode;
}

unsigned int Rendering::Shader::getDrawMode()
{
    return drawMode;
}

void Rendering::Shader::setUniform(const std::string &name, void *value)
{
    checkUniformSetRules(name);

    uniformValues[name] = value;

    updateUniforms = true;
}

void Rendering::Shader::setUniformArray(const std::string &name, void *value, size_t length)
{
    checkUniformSetRules(name);

    uniformArrayValues[name] = std::make_pair(value, length);

    updateUniformArrays = true;
}

template <typename T>
void Rendering::Shader::setAttrib(const std::string &name, T *value, size_t componentSize, unsigned int glType, bool normalize, unsigned int offset, unsigned int drawType)
{
    if (!loaded)
    {
        throw std::runtime_error("Shader must be loaded before attributes can be set.");
    }

    if (type != GL_ARRAY_BUFFER && type != GL_ELEMENT_ARRAY_BUFFER)
    {
        throw std::invalid_argument("type must be either GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER.");
    }

    if (drawType != GL_STATIC_DRAW && drawType != GL_DYNAMIC_DRAW && drawType != GL_STREAM_DRAW)
    {
        throw std::invalid_argument("drawType must be either GL_STATIC_DRAW, GL_DYNAMIC_DRAW or GL_STREAM_DRAW.");
    }

    int location = getAttribLocation(name);
    attribValues[name] = {value, componentSize, glType, normalize, componentSize * sizeof(T), offset, drawType, nullptr, 0, 0};

    updateAttributes = true;
}

void Rendering::Shader::setIndices(const std::string &name, unsigned int *indices, size_t length, unsigned int drawType)
{
    if (!loaded)
    {
        throw std::runtime_error("Shader must be loaded before indices can be set.");
    }

    if (!attribValues.contains(name))
    {
        throw std::runtime_error("Attribute " + name + " has not been set.");
    }

    if (drawType != GL_STATIC_DRAW && drawType != GL_DYNAMIC_DRAW && drawType != GL_STREAM_DRAW)
    {
        throw std::invalid_argument("drawType must be either GL_STATIC_DRAW, GL_DYNAMIC_DRAW or GL_STREAM_DRAW.");
    }

    attribValues[name].indices = new AttribIndices{indices, length, drawType};
    hasIndices = true;
}

bool Rendering::Shader::inUse()
{
    if (!loaded)
        return false;

    int currentProgramId;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgramId);

    return currentProgramId == programId;
}

bool Rendering::Shader::compileShaderSource(const std::string &source, unsigned int shaderType, unsigned int &shader)
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

void Rendering::Shader::updateAttributesAndUniforms()
{
    if (updateAttributes)
    {
        createVAO();
    }

    if (updateUniforms)
    {
        for (auto &[name, value] : uniformValues)
        {
            useUniform(name);
        }
    }

    if (updateUniformArrays)
    {
        for (auto &[name, value] : uniformArrayValues)
        {
            useUniformArray(name);
        }
    }
}

void Rendering::Shader::useUniform(const std::string &name)
{
    checkUniformSetRules(name);

    if (!inUse())
    {
        throw std::runtime_error("Shader must be in use before uniforms can be used.");
    }

    if (!uniformValues.contains(name))
    {
        throw std::runtime_error("Uniform " + name + " has not been set.");
    }

    int location = getUniformLocation(name);
    unsigned int type = getUniformType(name);
    void *value = uniformValues[name];

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

void Rendering::Shader::useUniformArray(const std::string &name)
{
    checkUniformSetRules(name);

    if (!inUse())
    {
        throw std::runtime_error("Shader must be in use before uniforms can be used.");
    }

    if (!uniformValues.contains(name))
    {
        throw std::runtime_error("Uniform " + name + " has not been set.");
    }

    int location = getUniformLocation(name);
    unsigned int type = getUniformType(name);

    auto pair = uniformArrayValues[name];
    void *value = pair.first;
    size_t length = pair.second;

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

unsigned int Rendering::Shader::createVAO()
{
    if (!loaded)
    {
        throw std::runtime_error("Shader must be loaded before attributes can be used.");
    }

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    for (auto &[name, attrib] : attribValues)
    {
        const auto &[value, componentSize, glType, normalize, stride, offset, drawType, indices, VBO, EBO] = attrib;

        int location = getAttribLocation(name);

        if (VBO == 0)
        {
            glGenBuffers(1, &attrib.VBO);
        }

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(value), value, drawType);

        if (indices != nullptr)
        {
            if (EBO == 0)
            {
                glGenBuffers(1, &attrib.EBO);
            }

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, drawType);
        }

        glVertexAttribPointer(location, componentSize, glType, normalize ? GL_TRUE : GL_FALSE, stride, (void *)offset);
        glEnableVertexAttribArray(location);
    }

    glBindVertexArray(0);

    updateAttributes = false;
}

int Rendering::Shader::getUniformLocation(const std::string &name)
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

int Rendering::Shader::getAttribLocation(const std::string &name)
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

unsigned int Rendering::Shader::getUniformType(const std::string &name)
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

unsigned int Rendering::Shader::getAttribType(const std::string &name)
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

void Rendering::Shader::checkUniformSetRules(const std::string &name)
{
    if (!loaded)
    {
        throw std::runtime_error("Shader must be loaded before uniforms can be set.");
    }

    getUniformLocation(name);
}
