#include "../../../include/Rendering/Shader/Shader.h"
#include "../../../include/Utility/FileHandling.h"
#include "../../../include/Rendering/Utility/OpenGLHelpers.h"

#include <iostream>
#include <boost/algorithm/string/replace.hpp>

Rendering::Shader::Shader()
{
}

Rendering::Shader::~Shader()
{
    glDeleteProgram(program);
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

    program = shaderProgram;
    loaded = true;

    uniformInfo = getUniformInfo();
    attribInfo = getVertexAttribInfo();

    return true;
}

void Rendering::Shader::use()
{
    if (!loaded)
    {
        throw std::runtime_error("Shader must be loaded before it can be used.");
    }

    // std::cout << "using shader: " << programId << std::endl;

    glUseProgram(program);

    // updateUniforms = true;
    // updateUniformArrays = true;
    // updateAttributesAndUniforms();

    // glBindVertexArray(VAO);
}

void Rendering::Shader::unbind()
{
    if (inUse())
    {
        glBindVertexArray(0);
        glUseProgram(0);
    }
}

void Rendering::Shader::draw(GLenum drawMode, size_t drawCount, size_t offset)
{
    glIsValidDrawMode(drawMode, false);

    if (!inUse())
    {
        throw std::runtime_error("Shader must be in use before it can be drawn.");
    }

    bindUniforms();
    bindVertexAttribs();

    if (vertexIndices != nullptr)
    {
        glDrawElements(drawMode, drawCount, GL_UNSIGNED_INT, (void *)offset);
    }
    else
    {
        glDrawArrays(drawMode, offset, drawCount);
    }

    glBindVertexArray(0);
}

void Rendering::Shader::drawInstanced(size_t instanceCount, GLenum drawMode, size_t drawCount, size_t offset)
{
    glIsValidDrawMode(drawMode, false);

    if (!inUse())
    {
        throw std::runtime_error("Shader must be in use before it can be drawn.");
    }

    bindUniforms();
    bindVertexAttribs();

    if (vertexIndices != nullptr)
    {
        glDrawElementsInstanced(drawMode, drawCount, GL_UNSIGNED_INT, (void *)offset, instanceCount);
    }
    else
    {
        glDrawArraysInstanced(drawMode, offset, drawCount, instanceCount);
    }

    glBindVertexArray(0);
}

void Rendering::Shader::uniform(UniformBase *uniform)
{
    if (!inUse())
    {
        throw std::runtime_error("Shader must be in use before uniforms can be set.");
    }

    if (!hasUniform(uniform->getName()))
    {
        throw std::invalid_argument("Uniform " + uniform->getName() + " not found.");
    }

    if (uniform->getGLType() != uniformInfo[uniform->getName()].type)
    {
        throw std::invalid_argument("Uniform " + uniform->getName() + " has incorrect type. Expected " + glTypeToString(uniformInfo[uniform->getName()].type) + ", got " + glTypeToString(uniform->getGLType()) + ".");
    }

    uniforms[uniform->getName()] = uniform;
}

void Rendering::Shader::uniform(const std::vector<UniformBase *> &uniforms)
{
    for (auto u : uniforms)
    {
        uniform(u);
    }
}

void Rendering::Shader::uniform(const std::unordered_map<std::string, UniformBase *> &uniforms)
{
    for (auto u : uniforms)
    {
        uniform(u.second);
    }
}

void Rendering::Shader::attrib(VertexAttribBase *attrib)
{
    if (!isLoaded())
    {
        throw std::runtime_error("Shader must be loaded before vertex attributes can be set.");
    }

    if (!hasAttrib(attrib->getName()))
    {
        throw std::invalid_argument("Vertex attribute " + attrib->getName() + " not found.");
    }

    if (attrib->getGLType() != attribInfo[attrib->getName()].type)
    {
        throw std::invalid_argument("Vertex attribute " + attrib->getName() + " has incorrect type. Expected " + glTypeToString(attribInfo[attrib->getName()].type) + ", got " + glTypeToString(attrib->getGLType()) + ".");
    }

    attribsNeedUpdate = true;

    vertexAttribs[attrib->getName()] = attrib;
}

void Rendering::Shader::attrib(const std::vector<VertexAttribBase *> &attribs)
{
    for (auto a : attribs)
    {
        attrib(a);
    }
}

void Rendering::Shader::attrib(const std::unordered_map<std::string, VertexAttribBase *> &attribs)
{
    for (auto a : attribs)
    {
        attrib(a.second);
    }
}

void Rendering::Shader::indices(VertexIndices *indices)
{
    if (!isLoaded())
    {
        throw std::runtime_error("Shader must be loaded before vertex indices can be set.");
    }

    indicesNeedUpdate = true;

    vertexIndices = indices;
}

bool Rendering::Shader::inUse()
{
    int currentProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);

    return currentProgram == program;
}

bool Rendering::Shader::isLoaded()
{
    return loaded;
}

bool Rendering::Shader::hasUniform(const std::string &name)
{
    if (!isLoaded())
    {
        throw std::runtime_error("Shader must be loaded before uniforms can be checked.");
    }

    return uniformInfo.contains(name);
}

bool Rendering::Shader::hasAttrib(const std::string &name)
{
    if (!isLoaded())
    {
        throw std::runtime_error("Shader must be loaded before vertex attributes can be checked.");
    }

    return attribInfo.contains(name);
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

void Rendering::Shader::bindUniforms()
{
    if (!inUse())
    {
        throw std::runtime_error("Shader must be in use before uniforms can be bound.");
    }

    for (auto u : uniforms)
    {
        // std::cout << "binding uniform: " << u.first << std::endl;
        // std::cout << "location: " << uniformInfo[u.first].location << std::endl;
        // std::cout << "type: " << glTypeToString(u.second->getGLType()) << std::endl;

        auto info = uniformInfo[u.first];
        glUniform(info.location, u.second);
    }
}

void Rendering::Shader::bindVertexAttribs()
{
    if (!isLoaded())
    {
        throw std::runtime_error("Shader must be loaded before vertex attributes can be bound.");
    }

    if (!attribsNeedUpdate)
    {
        glBindVertexArray(VAO);
        return;
    }

    // create VAO
    if (VAO != 0)
    {
        glDeleteVertexArrays(1, &VAO);
    }

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // bind vertex attributes
    for (auto &[name, a] : vertexAttribs)
    {
        std::cout << "binding vertex attrib: " << name << std::endl;
        std::cout << "location: " << attribInfo[name].location << std::endl;
        std::cout << "type: " << glTypeToString(a->getGLType()) << std::endl;
        std::cout << "size: " << a->size() << std::endl;
        std::cout << "tSize: " << a->getTSize() << std::endl;

        auto &info = attribInfo[name];
        unsigned int vbo = info.vbo;

        if (vbo == 0)
        {
            glGenBuffers(1, &vbo);
            info.vbo = vbo;
        }

        size_t bufferSize = a->size() * a->getTSize();

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, bufferSize, a->getValuePointer(), VERTEX_ATTRIB_DRAW_TYPE);

        // matrix
        if (a->getMatrixSize() != -1)
        {
            // we need to set up the attribs for each column of the matrix
            for (int i = 0; i < a->getMatrixSize(); i++)
            {
                glVertexAttribPointer(info.location + i, a->getMatrixSize(), a->getGLType(), a->getNormalize() ? GL_TRUE : GL_FALSE, a->getMatrixSize() * a->getMatrixSize() * a->getTSize(), (void *)(i * a->getTSize() * a->getMatrixSize()));
                glVertexAttribDivisor(info.location + i, a->getDivisor());
                glEnableVertexAttribArray(info.location + i);
            }
        }
        else
        {
            // not a matrix
            if (glIsTypeInt(a->getGLType()))
            {
                glVertexAttribIPointer(info.location, a->getNumComponents(), a->getGLType(), a->getTSize(), (void *)a->getOffset());
            }
            else
            {
                glVertexAttribPointer(info.location, a->getNumComponents(), a->getGLType(), a->getNormalize() ? GL_TRUE : GL_FALSE, a->getTSize(), (void *)a->getOffset());
            }

            glVertexAttribDivisor(info.location, a->getDivisor());
            glEnableVertexAttribArray(info.location);
        }
    }

    // bind vertex indices
    if (vertexIndices != nullptr)
    {
        if (EBO == 0)
        {
            glGenBuffers(1, &EBO);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

        if (indicesNeedUpdate)
        {
            auto &indices = vertexIndices->get();
            size_t bufferSize = indices.size() * sizeof(indices[0]);

            glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferSize, &indices[0], VERTEX_ATTRIB_DRAW_TYPE);
        }
    }

    attribsNeedUpdate = false;
    indicesNeedUpdate = false;

    // unbind buffers
    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // rebind vao
    glBindVertexArray(VAO);
}

std::unordered_map<std::string, Rendering::Shader::UniformInfo> Rendering::Shader::getUniformInfo()
{
    if (!loaded)
    {
        throw std::runtime_error("Shader must be loaded before uniforms can be retrieved.");
    }

    std::unordered_map<std::string, UniformInfo> result;

    int uniformCount;
    glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &uniformCount);

    int maxLength;
    glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);

    for (int i = 0; i < uniformCount; i++)
    {
        char name[maxLength];
        GLenum type;
        int size;

        glGetActiveUniform(program, i, maxLength, NULL, &size, &type, name);

        std::cout << "uniform: " << name << std::endl;

        result.emplace(std::string(name), UniformInfo{name, i, type, size});
    }

    return result;
}

std::unordered_map<std::string, Rendering::Shader::VertexAttribInfo> Rendering::Shader::getVertexAttribInfo()
{
    if (!loaded)
    {
        throw std::runtime_error("Shader must be loaded before vertex attributes can be retrieved.");
    }

    std::unordered_map<std::string, VertexAttribInfo> result;

    int attribCount;
    glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &attribCount);

    int maxLength;
    glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength);

    for (int i = 0; i < attribCount; i++)
    {
        char name[maxLength];
        int length;
        int size;
        GLenum type;

        glGetActiveAttrib(program, i, maxLength, &length, &size, &type, name);

        std::cout << "attrib: " << name << std::endl;

        result.emplace(std::string(name), VertexAttribInfo{name, i, type, size});
    }

    return result;
}