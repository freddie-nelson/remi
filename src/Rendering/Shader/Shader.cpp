#include "../../../include/Rendering/Shader/Shader.h"
#include "../../../include/Rendering/Utility/FileHandling.h"
#include "../../../include/Rendering/Utility/GlmHelpers.h"
#include "../../../include/Rendering/Utility/OpenGLHelpers.h"
#include "../../../include/gl.h"

#include <stdexcept>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <glm/gtx/string_cast.hpp>

Rendering::Shader::Shader()
{
}

Rendering::Shader::~Shader()
{
    unbind();

    for (auto &[name, attrib] : attribValues)
    {
        if (attrib.indices != nullptr)
        {
            delete attrib.indices;
        }
    }

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

    std::string vertexSource = injectShaderVariables(injectShaderFunctions(vertex));
    std::string fragmentSource = injectShaderVariables(injectShaderFunctions(fragment));

    // std::cout << "vertexSource: " << std::endl;
    // std::cout << vertexSource << std::endl;

    // std::cout << "fragmentSource: " << std::endl;
    // std::cout << fragmentSource << std::endl;

    // vertex shader
    unsigned int vertexShader;
    if (!compileShaderSource(vertexSource, GL_VERTEX_SHADER, vertexShader))
    {
        return false;
    }

    // fragment shader
    unsigned int fragmentShader;
    if (!compileShaderSource(fragmentSource, GL_FRAGMENT_SHADER, fragmentShader))
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

    // std::cout << "using shader: " << programId << std::endl;

    glUseProgram(programId);

    // updateUniforms = true;
    // updateUniformArrays = true;
    // updateAttributesAndUniforms();

    // glBindVertexArray(VAO);
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
    checkDrawModeValid(drawMode);

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

void Rendering::Shader::drawInstanced(unsigned int instanceCount, unsigned int drawMode, unsigned int drawCount, unsigned int offset)
{
    checkDrawModeValid(drawMode);

    if (!inUse())
    {
        throw std::runtime_error("Shader must be in use before it can be drawn.");
    }

    updateAttributesAndUniforms();
    glBindVertexArray(VAO);

    if (hasIndices)
    {
        glDrawElementsInstanced(drawMode, drawCount, GL_UNSIGNED_INT, (void *)offset, instanceCount);
    }
    else
    {
        glDrawArraysInstanced(drawMode, offset, drawCount, instanceCount);
    }
}

void Rendering::Shader::setUniform(const std::string &name, void *value)
{
    checkUniformSetRules(name);

    int location;
    unsigned int type;

    if (uniformValues.contains(name))
    {
        location = uniformValues[name].location;
        type = uniformValues[name].type;
    }
    else
    {
        // std::cout << "fetching uniform location" << std::endl;
        location = getUniformLocation(name);
        type = getUniformType(name);
    }

    uniformValues[name] = {true, location, type, value};

    updateUniforms = true;
}

void Rendering::Shader::setUniformArray(const std::string &name, void *value, size_t length)
{
    checkUniformSetRules(name, true);

    int location;
    unsigned int type;

    if (uniformArrayValues.contains(name))
    {
        location = uniformArrayValues[name].location;
        type = uniformArrayValues[name].type;
    }
    else
    {
        location = getUniformLocation(name, true);
        type = getUniformType(name, true);
    }

    uniformArrayValues[name] = UniformArray{true, location, type, value, length};

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

    if (attribValues[name].indices != nullptr)
    {
        delete attribValues[name].indices;
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

    return static_cast<unsigned int>(currentProgramId) == programId;
}

bool Rendering::Shader::compileShaderSource(const std::string &source, unsigned int shaderType, unsigned int &shader)
{
    if (shaderType != GL_VERTEX_SHADER && shaderType != GL_FRAGMENT_SHADER)
    {
        throw std::invalid_argument("shaderType must be either GL_VERTEX_SHADER or GL_FRAGMENT_SHADER.");
    }

    const auto shaderName = shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment";
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
        std::cerr << "Failed to compile " << shaderName << " shader. Info: " << infoLog << std::endl;
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

    if (!uniformValues.contains(name))
    {
        throw std::runtime_error("Uniform " + name + " has not been set.");
    }

    int location = uniformValues[name].location;
    unsigned int type = uniformValues[name].type;
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
        std::cout << "uniform name: " << name << std::endl;
        std::cout << "v: " << v[0] << ", " << v[1] << std::endl;
        std::cout << "location: " << location << std::endl;
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
        // std::cout << "unsigned int" << std::endl;
        // std::cout << v << std::endl;
        // std::cout << "using uniform: " << name << std::endl;
        // std::cout << "location: " << getUniformLocation(name) << std::endl;
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
        std::cout << "using uniform: " << name << std::endl;
        std::cout << "v: " << glm::to_string(v) << std::endl;
        std::cout << "location: " << location << std::endl;
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(v));
        break;
    }

    case GL_SAMPLER_2D:
    {
        auto v = *static_cast<int *>(value);
        std::cout << "using uniform: " << name << std::endl;
        std::cout << "v: " << v << std::endl;
        if (v < 0 || v > static_cast<int>(glGetMaxTextureUnits()))
        {
            throw std::invalid_argument("Sampler2D uniform value must be between 0 and " + std::to_string(glGetMaxTextureUnits()) + ".");
        }

        glUniform1i(location, v);
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
    checkUniformSetRules(name, true);

    if (!uniformArrayValues.contains(name))
    {
        throw std::runtime_error("Uniform " + name + " has not been set.");
    }

    auto u = uniformArrayValues[name];

    int location = u.location;
    unsigned int type = u.type;

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
    case GL_SAMPLER_2D:
    case GL_INT:
    {
        auto v = static_cast<int *>(value);
        glUniform1iv(location, length, v);

        std::cout << "using uniform: " << name << ", location: " << location << std::endl;
        std::cout << "v: ";
        for (size_t i = 0; i < glGetMaxTextureUnits(); i++)
        {
            std::cout << v[i] << ", ";
        }

        std::cout << std::endl;

        break;
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
        const auto &[stale, typeSize, value, size, componentSize, glType, normalize, stride, offset, drawType, divisor, matrixSize, indices, VBO, EBO] = attrib;

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

        // if (name == "aZIndex")
        // {
        //     unsigned int *intVal = static_cast<unsigned int *>(value);
        //     for (int i = 0; i < size / sizeof(unsigned int); i++)
        //     {
        //         std::cout << i << ": ";
        //         std::cout << intVal[i] << std::endl;
        //     }
        // }

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

        if (matrixSize == -1)
        {
            if (glIsTypeInt(glType))
            {
                glVertexAttribIPointer(location, componentSize, glType, stride, (void *)offset);
            }
            else
            {
                glVertexAttribPointer(location, componentSize, glType, normalize ? GL_TRUE : GL_FALSE, stride, (void *)offset);
            }

            if (divisor != -1)
            {
                glVertexAttribDivisor(location, divisor);
            }

            glEnableVertexAttribArray(location);
        }
        else
        {
            // we must set the attribute pointer for each column of the matrix
            // component size for mat2 is 4, mat3 is 9, mat4 is 16
            for (int col = 0; col < matrixSize; col++)
            {
                glVertexAttribPointer(location + col, matrixSize, glType, normalize ? GL_TRUE : GL_FALSE, typeSize * matrixSize * matrixSize, (void *)(typeSize * matrixSize * col));

                if (divisor != -1)
                {
                    glVertexAttribDivisor(location + col, divisor);
                }

                glEnableVertexAttribArray(location + col);
            }
        }

        attrib.stale = false;
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    updateAttributes = false;
}

int Rendering::Shader::getUniformLocation(const std::string &name, bool isUniformArray)
{
    if (!loaded)
    {
        throw std::runtime_error("Shader has not been loaded.");
    }

    std::string realName = isUniformArray && !name.ends_with("]") ? name + "[0]" : name;

    int location = glGetUniformLocation(programId, realName.c_str());
    // std::cout << "name: " << name << std::endl;
    // std::cout << "location: " << location << std::endl;

    std::string otherName = isUniformArray ? name + "[0]" : name;

    if (location == -1 && isUniformArray)
    {
        location = glGetUniformLocation(programId, otherName.c_str());
        // std::cout << "arrayName: " << otherName << std::endl;
        // std::cout << "location: " << location << std::endl;
    }

    if (location == -1)
    {
        throw std::runtime_error("Uniform " + name + " does not exist.");
    }

    //? workaround weird opengl bug? TODO: fix - was driver related?
    //? keeping anyway just in case, robustness is good
    // std::cout << "name: " << realName << std::endl;
    // std::cout << "arrayName: " << otherName << std::endl;

    std::string uniformAtLocation = getUniformName(location, true);

    while (uniformAtLocation == "")
    {
        uniformAtLocation = getUniformName(location, true);
    }

    // std::cout << "uniformAtLocation: " << uniformAtLocation << std::endl;

    if (uniformAtLocation != realName && uniformAtLocation != otherName && uniformAtLocation != name)
    {
        location = findUniformLocation(realName, isUniformArray);
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

unsigned int Rendering::Shader::getUniformType(const std::string &name, bool isUniformArray)
{
    if (!loaded)
    {
        throw std::runtime_error("Shader has not been loaded.");
    }

    int location = getUniformLocation(name, isUniformArray);

    unsigned int type;
    glGetActiveUniform(programId, location, 0, NULL, NULL, &type, NULL);

    return type;
}

std::string Rendering::Shader::getUniformName(unsigned int location, bool safe)
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
    if (nameStr.length() == 0 && !safe)
    {
        throw std::runtime_error("Failed to get uniform name at location " + std::to_string(location) + ".");
    }

    return nameStr;
}

int Rendering::Shader::findUniformLocation(const std::string &name, bool isUniformArray)
{
    GLint numOfUniforms;
    glGetProgramiv(programId, GL_ACTIVE_UNIFORMS, &numOfUniforms);

    const auto uniformName = isUniformArray && !name.ends_with("]") ? name + "[0]" : name;

    // we can just loop through as uniform locations are assigned in order
    for (GLint location = 0; location < numOfUniforms; location++)
    {
        auto nameAtLocation = getUniformName(location);
        // std::cout << "name: " << name << ", nameAtLocation: " << nameAtLocation << std::endl;
        if (nameAtLocation == uniformName || nameAtLocation == name)
        {
            // std::cout << "found uniform at location: " << location << std::endl;
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

void Rendering::Shader::checkUniformSetRules(const std::string &name, bool isUniformArray)
{
    if (!loaded)
    {
        throw std::runtime_error("Shader must be loaded before uniforms can be set.");
    }

    if (!inUse())
    {
        throw std::runtime_error("Shader must be in use before uniforms can be set.");
    }
}

void Rendering::Shader::checkDrawModeValid(unsigned int drawMode)
{
    if (drawMode != GL_POINTS && drawMode != GL_LINE_STRIP && drawMode != GL_LINE_LOOP && drawMode != GL_LINES && drawMode != GL_TRIANGLE_STRIP && drawMode != GL_TRIANGLE_FAN && drawMode != GL_TRIANGLES)
    {
        throw std::invalid_argument("drawMode must be one of the following: GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES, GL_LINE_STRIP_ADJACENCY, GL_LINES_ADJACENCY, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_TRIANGLES, GL_TRIANGLE_STRIP_ADJACENCY, GL_TRIANGLES_ADJACENCY, GL_PATCHES");
    }
}

std::string Rendering::Shader::injectShaderVariables(const std::string &source)
{
    std::string result = source;

    // __MAX_TEXTURE_UNITS__
    boost::replace_all(result, std::string("__MAX_TEXTURE_UNITS__"), std::to_string(glGetMaxTextureUnits()));

    return result;
}

std::string getTextureFunctionStr = "";
std::string getTextureFunction()
{
    if (getTextureFunctionStr != "")
        return getTextureFunctionStr;

    getTextureFunctionStr = "vec4 getTexture(uint textureUnit, vec2 textureCoord)\n";

    std::string block = "{\n"
                        "   switch (textureUnit)\n"
                        "   {\n";

    for (unsigned int i = 0; i < Rendering::glGetMaxTextureUnits(); i++)
    {
        block += "   case " + std::to_string(i) + "u:\n"
                                                  "       return texture(uTextures[" +
                 std::to_string(i) + "], textureCoord);\n";
    }

    block += "}\n"
             "   return vec4(1.0f, 0.0f, 1.0f, 1.0f);\n"
             "}\n";

    getTextureFunctionStr += block;

    return getTextureFunctionStr;
}

std::string Rendering::Shader::injectShaderFunctions(const std::string &source)
{
    std::string result = source;

    // __getTexture__
    boost::replace_all(result, std::string("__getTexture__"), getTextureFunction());

    return result;
}