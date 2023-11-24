#include "../../../include/Rendering/Shader/Shader.h"
#include "../../../include/Rendering/Utility/FileHandling.h"
#include "../../../include/Rendering/Utility/GlmHelpers.h"

#include "../../../include/externals/glad/gl.h"
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
    if (!compileShaderSource(fragment, GL_FRAGMENT_SHADER, fragmentShader))
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

    programId = shaderProgram;
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

    updateUniforms = true;
    updateUniformArrays = true;
    updateAttributesAndUniforms();

    glBindVertexArray(VAO);
}

void Rendering::Shader::unbind()
{
    for (auto &[name, u] : uniformValues)
    {
        u.stale = true;
    }

    for (auto &[name, u] : uniformArrayValues)
    {
        u.stale = true;
    }

    if (inUse())
    {
        glBindVertexArray(0);
        glUseProgram(0);
    }
}

void Rendering::Shader::draw(unsigned int drawMode, unsigned int drawCount, unsigned int offset)
{
    if (drawMode != GL_POINTS && drawMode != GL_LINE_STRIP && drawMode != GL_LINE_LOOP && drawMode != GL_LINES && drawMode != GL_LINE_STRIP_ADJACENCY && drawMode != GL_LINES_ADJACENCY && drawMode != GL_TRIANGLE_STRIP && drawMode != GL_TRIANGLE_FAN && drawMode != GL_TRIANGLES && drawMode != GL_TRIANGLE_STRIP_ADJACENCY && drawMode != GL_TRIANGLES_ADJACENCY && drawMode != GL_PATCHES)
    {
        throw std::invalid_argument("drawMode must be one of the following: GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES, GL_LINE_STRIP_ADJACENCY, GL_LINES_ADJACENCY, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_TRIANGLES, GL_TRIANGLE_STRIP_ADJACENCY, GL_TRIANGLES_ADJACENCY, GL_PATCHES");
    }

    if (!inUse())
    {
        throw std::runtime_error("Shader must be in use before it can be drawn.");
    }

    updateAttributesAndUniforms();
    glBindVertexArray(VAO);

    if (hasIndices)
    {
        glDrawElements(drawMode, drawCount, GL_UNSIGNED_INT, (void *)offset);
    }
    else
    {
        glDrawArrays(drawMode, offset, drawCount);
    }
}

void Rendering::Shader::setUniform(const std::string &name, void *value)
{
    checkUniformSetRules(name);

    uniformValues[name] = {true, value};

    updateUniforms = true;
}

void Rendering::Shader::setUniformArray(const std::string &name, void *value, size_t length)
{
    checkUniformSetRules(name);

    uniformArrayValues[name] = UniformArray{true, value, length};

    updateUniformArrays = true;
}

void Rendering::Shader::setIndices(const std::string &name, const unsigned int *indices, size_t length, unsigned int drawType)
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
    if (updateUniforms)
    {
        for (auto &[name, u] : uniformValues)
        {
            if (!u.stale)
                continue;

            useUniform(name);
            u.stale = false;
        }
    }

    if (updateUniformArrays)
    {
        for (auto &[name, u] : uniformArrayValues)
        {
            if (!u.stale)
                continue;

            useUniformArray(name);
            u.stale = false;
        }
    }

    if (updateAttributes)
    {
        createVAO();
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
    void *value = uniformValues[name].value;

    //! TODO cleanup switch statement into functions
    switch (type)
    {
    // FLOATS
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
        glUniform4f(location, v[0], v[1], v[2], v[3]);
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

    auto u = uniformArrayValues[name];
    void *value = u.value;
    size_t length = u.length;

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
        auto v = *static_cast<glm::vec2 *>(value);
        glUniform2fv(location, length, glm::value_ptr(v));
        break;
    }
    case GL_FLOAT_VEC3:
    {
        auto v = *static_cast<glm::vec3 *>(value);
        glUniform3fv(location, length, glm::value_ptr(v));
        break;
    }
    case GL_FLOAT_VEC4:
    {
        auto v = *static_cast<glm::vec4 *>(value);
        glUniform4fv(location, length, glm::value_ptr(v));
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
        auto v = *static_cast<glm::ivec2 *>(value);
        glUniform2iv(location, length, glm::value_ptr(v));
        break;
    }
    case GL_INT_VEC3:
    {
        auto v = *static_cast<glm::ivec3 *>(value);
        glUniform3iv(location, length, glm::value_ptr(v));
        break;
    }
    case GL_INT_VEC4:
    {
        auto v = *static_cast<glm::ivec4 *>(value);
        glUniform4iv(location, length, glm::value_ptr(v));
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
        auto v = *static_cast<glm::uvec2 *>(value);
        glUniform2uiv(location, length, glm::value_ptr(v));
        break;
    }
    case GL_UNSIGNED_INT_VEC3:
    {
        auto v = *static_cast<glm::uvec3 *>(value);
        glUniform3uiv(location, length, glm::value_ptr(v));
        break;
    }
    case GL_UNSIGNED_INT_VEC4:
    {
        auto v = *static_cast<glm::uvec4 *>(value);
        glUniform4uiv(location, length, glm::value_ptr(v));
        break;
    }

    // SQUARE MATRICES
    case GL_FLOAT_MAT2:
    {
        auto v = *static_cast<glm::mat2 *>(value);
        glUniformMatrix2fv(location, length, GL_FALSE, glm::value_ptr(v));
        break;
    }
    case GL_FLOAT_MAT3:
    {
        auto v = *static_cast<glm::mat3 *>(value);
        glUniformMatrix3fv(location, length, GL_FALSE, glm::value_ptr(v));
        break;
    }
    case GL_FLOAT_MAT4:
    {
        auto v = *static_cast<glm::mat4 *>(value);
        glUniformMatrix4fv(location, length, GL_FALSE, glm::value_ptr(v));
        break;
    }

    // UNSUPPORTED
    default:
        throw std::runtime_error("Uniform " + name + " has an unsupported type.");
        break;
    }
}

void Rendering::Shader::createVAO()
{
    if (!loaded)
    {
        throw std::runtime_error("Shader must be loaded before attributes can be used.");
    }

    if (VAO != 0)
    {
        glDeleteVertexArrays(1, &VAO);
    }

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    for (auto &[name, attrib] : attribValues)
    {
        const auto &[stale, value, size, componentSize, glType, normalize, stride, offset, drawType, indices, VBO, EBO] = attrib;

        // if attribute does not need updated, skip
        if (!stale)
        {
            continue;
        }

        int location = getAttribLocation(name);

        if (VBO == 0)
        {
            glGenBuffers(1, &attrib.VBO);
        }

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, size, value, drawType);

        // float *floatVal = static_cast<float *>(value);
        // for (int i = 0; i < size / sizeof(float); i++)
        // {
        //     std::cout << i << ": ";
        //     std::cout << floatVal[i] << std::endl;
        // }

        if (indices != nullptr)
        {
            if (EBO == 0)
            {
                glGenBuffers(1, &attrib.EBO);
            }

            // for (int i = 0; i < indices->length; i++)
            // {
            //     std::cout << indices->indices[i] << std::endl;
            // }

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices->length, indices->indices, drawType);
        }

        // std::cout << "location: " << location << std::endl;
        // std::cout << "componentSize: " << componentSize << std::endl;
        // std::cout << "glType: " << glType << std::endl;
        // std::cout << "normalize: " << normalize << std::endl;
        // std::cout << "stride: " << stride << std::endl;
        // std::cout << "offset: " << offset << std::endl;

        glVertexAttribPointer(location, componentSize, glType, normalize ? GL_TRUE : GL_FALSE, stride, (void *)offset);
        glEnableVertexAttribArray(location);

        attrib.stale = false;
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

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

    //? workaround weird opengl bug? TODO: fix - was driver related?
    //? keeping anyway just in case, robustness is good
    std::string uniformAtLocation = getUniformName(location);
    if (uniformAtLocation != name)
    {
        location = findUniformLocation(name);
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
        throw std::runtime_error("getAttribLocation: Attribute " + name + " does not exist.");
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
    glGetActiveUniform(programId, location, 0, NULL, NULL, &type, NULL);

    return type;
}

std::string Rendering::Shader::getUniformName(unsigned int location)
{

    GLint bufsize;
    glGetProgramiv(programId, GL_ACTIVE_UNIFORM_MAX_LENGTH, &bufsize);

    char name[bufsize];
    GLsizei nameLength;

    //! error on this line? program crashing only on nvidia gpu?
    //! investigated and definitely a driver bug, but not sure how to fix
    //! code works fine on intel integrated graphics, not tested on amd
    //! only tested on my (freddie) 1060 so not sure if it's just this card or what
    glGetActiveUniform(programId, location, bufsize, &nameLength, NULL, NULL, name);

    const auto nameStr = std::string(name);
    if (nameStr.length() == 0)
    {
        throw std::runtime_error("Failed to get uniform name at location " + std::to_string(location) + ".");
    }

    return nameStr;
}

int Rendering::Shader::findUniformLocation(const std::string &name)
{
    GLint numOfUniforms;
    glGetProgramiv(programId, GL_ACTIVE_UNIFORMS, &numOfUniforms);

    // we can just loop through as uniform locations are assigned in order
    for (GLint location = 0; location < numOfUniforms; location++)
    {
        auto nameAtLocation = getUniformName(location);
        if (nameAtLocation == name)
        {
            return location;
        }
    }

    throw std::runtime_error("Could not find uniform location, '" + name + "'.");
}

unsigned int Rendering::Shader::getAttribType(const std::string &name)
{
    if (!loaded)
    {
        throw std::runtime_error("Shader has not been loaded.");
    }

    int location = getAttribLocation(name);

    unsigned int type;
    glGetActiveAttrib(programId, location, sizeof(unsigned int), NULL, NULL, &type, NULL);

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
