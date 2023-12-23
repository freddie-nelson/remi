#include "../../include/Rendering/Renderer.h"
#include "../../include/Rendering/Shader/MeshShader.h"
#include "../../include/Rendering/Shader/InstancedMeshShader.h"
#include "../../include/Rendering/Shader/BatchedMeshShader.h"
#include "../../include/Rendering/Utility/OpenGLHelpers.h"
#include "../../include/Config.h"

#include "../../include/externals/glad/gl.h"
#include <iostream>
#include <memory>
#include <math.h>
#include <glm/gtc/type_ptr.hpp>

Rendering::Renderer::Renderer(GLFWwindow *glfwWindow, int width, int height) : glfwWindow(glfwWindow), width(width), height(height), camera(Camera(glm::vec2(0, 0), width, height))
{
    setSize(width, height);
}

Rendering::Renderer::~Renderer()
{
    delete this;
}

void Rendering::Renderer::init()
{
    enableDepthTest(true);
    enableAlphaBlending(false);

    meshShader = Shader();
    if (!meshShader.loadFromSource(meshVertexShader, meshFragShader))
    {
        throw std::runtime_error("Failed to load mesh shader.");
    }

    instancedMeshShader = Shader();
    if (!instancedMeshShader.loadFromSource(instancedMeshVertexShader, instancedMeshFragShader))
    {
        throw std::runtime_error("Failed to load instanced mesh shader.");
    }

    batchedMeshShader = Shader();
    if (!batchedMeshShader.loadFromSource(batchedMeshVertexShader, batchedMeshFragShader))
    {
        throw std::runtime_error("Failed to load batched mesh shader.");
    }
}

void Rendering::Renderer::clear(bool clearColorBuffer, bool clearDepthBuffer, bool clearStencilBuffer) const
{
    glClearColor(clearColor.r(), clearColor.g(), clearColor.b(), clearColor.a());

    int clearBits = 0;
    if (clearColorBuffer)
    {
        clearBits |= GL_COLOR_BUFFER_BIT;
    }
    if (clearDepthBuffer)
    {
        clearBits |= GL_DEPTH_BUFFER_BIT;
    }
    if (clearStencilBuffer)
    {
        clearBits |= GL_STENCIL_BUFFER_BIT;
    }

    glClear(clearBits);
}

void Rendering::Renderer::present() const
{
    glfwSwapBuffers(glfwWindow);
}

void Rendering::Renderer::mesh(const Mesh2D &m, const Core::Transform &transform, const Material *material)
{
    const std::vector<glm::vec2> &vertices = m.getVertices();
    float *verticesPointer = (float *)glm::value_ptr(vertices[0]);

    const std::vector<unsigned int> &indices = m.getIndices();

    const std::vector<glm::vec2> &uvs = m.getUvs();
    float *uvsPointer = (float *)glm::value_ptr(uvs[0]);

    // for (int i = 0; i < mesh.vertices.size(); i++)
    // {
    //     std::cout << vertices[i * 2] << ", " << vertices[i * 2 + 1] << std::endl;
    //     std::cout << mesh.vertices[i].x << ", " << mesh.vertices[i].y << std::endl;
    // }

    meshShader.use();

    auto color = material->getColor();
    auto colorVec = color.getColor();
    meshShader.setUniform("uColor", &colorVec);

    auto texture = material->getTexture();
    auto boundTexture = textureManager.bind(texture);

    auto &textures = textureManager.getTexturesUniform();
    meshShader.setUniformArray("uTextures", const_cast<int *>(&textures[0]), textures.size());

    meshShader.setUniform("uTextureUnit", &boundTexture.textureUnit);
    meshShader.setUniform("uTextureSize", &boundTexture.textureSize);
    meshShader.setUniform("uTextureAtlasPos", &boundTexture.posInAtlas);
    meshShader.setUniform("uTextureAtlasSize", &boundTexture.atlasSize);

    meshShader.setUniform("uViewProjectionMatrix", &viewProjectionMatrix);

    glm::mat4 transformMatrix = transform.getTransformationMatrix();
    meshShader.setUniform("uMeshTransform", &transformMatrix);

    meshShader.setAttrib("aPos", verticesPointer, vertices.size() * 2, 2, GL_FLOAT, false, 0, GL_DYNAMIC_DRAW);
    meshShader.setIndices("aPos", &indices[0], indices.size(), GL_DYNAMIC_DRAW);

    meshShader.setAttrib("aTexCoord", uvsPointer, uvs.size() * 2, 2, GL_FLOAT, false, 0, GL_DYNAMIC_DRAW);

    meshShader.draw(GL_TRIANGLES, indices.size());
    meshShader.unbind();
};

void Rendering::Renderer::instancedMesh(const Mesh2D &m, const std::vector<Core::Transform> &transforms, const std::vector<Material *> &materials)
{
    if (transforms.size() != materials.size())
    {
        throw std::runtime_error("Renderer (instancedMesh): transforms and materials must have the same size.");
    }

    const unsigned int instanceCount = transforms.size();

    const std::vector<glm::vec2> &vertices = m.getVertices();
    float *verticesPointer = (float *)glm::value_ptr(vertices[0]);

    const std::vector<unsigned int> &indices = m.getIndices();

    // create transforms and colors arrays
    std::vector<glm::mat4> transformMatrices(instanceCount);
    std::vector<glm::vec4> colorsVec(instanceCount);

    for (int i = 0; i < instanceCount; i++)
    {
        transformMatrices[i] = transforms[i].getTransformationMatrix();
        colorsVec[i] = materials[i]->getColor().getColor();
    }

    instancedMeshShader.use();

    instancedMeshShader.setUniform("uViewProjectionMatrix", &viewProjectionMatrix);

    // instanced arrays
    float *transformsArr = (float *)glm::value_ptr(transformMatrices[0]);
    instancedMeshShader.setAttrib("aTransform", transformsArr, instanceCount * 16, 16, GL_FLOAT, false, 0, GL_DYNAMIC_DRAW, 1, 4);

    float *colorsArr = (float *)glm::value_ptr(colorsVec[0]);
    instancedMeshShader.setAttrib("aColor", colorsArr, instanceCount * 4, 4, GL_FLOAT, false, 0, GL_DYNAMIC_DRAW, 1);

    // vertices and indices
    instancedMeshShader.setAttrib("aPos", verticesPointer, vertices.size() * 2, 2, GL_FLOAT, false, 0, GL_DYNAMIC_DRAW);
    instancedMeshShader.setIndices("aPos", &indices[0], indices.size(), GL_DYNAMIC_DRAW);

    instancedMeshShader.drawInstanced(instanceCount, GL_TRIANGLES, indices.size());
    instancedMeshShader.unbind();
};

void Rendering::Renderer::batchedMesh(const std::vector<Mesh2D> &meshs, const std::vector<Core::Transform> &transforms, const std::vector<Material *> &materials)
{
    if (meshs.size() != transforms.size() || meshs.size() != materials.size())
    {
        throw std::runtime_error("Renderer (batchedMesh): meshs, transforms and materials must have the same size.");
    }

    if (meshs.size() == 0)
    {
        return;
    }

    int verticesCount = 0;
    int indicesCount = 0;

    for (auto &m : meshs)
    {
        verticesCount += m.getVertices().size();
        indicesCount += m.getIndices().size();
    }

    std::vector<glm::vec4> batchedVertices(verticesCount);
    std::vector<unsigned int> batchedIndices(indicesCount);

    std::vector<glm::vec4> batchedColors(verticesCount);

    int verticesOffset = 0;
    int indicesOffset = 0;

    for (int i = 0; i < meshs.size(); i++)
    {
        const auto &m = meshs[i];
        const auto &transform = transforms[i];
        const auto &color = materials[i]->getColor().getColor();

        const std::vector<glm::vec2> &vertices = m.getVertices();
        const std::vector<unsigned int> &indices = m.getIndices();

        for (int i = 0; i < vertices.size(); i++)
        {
            batchedVertices[verticesOffset + i] = transform.getTransformationMatrix() * glm::vec4(vertices[i], 0.0f, 1.0f);
        }

        for (int i = 0; i < indices.size(); i++)
        {
            batchedIndices[indicesOffset + i] = indices[i] + verticesOffset;
        }

        for (int i = 0; i < vertices.size(); i++)
        {
            batchedColors[verticesOffset + i] = color;
        }

        verticesOffset += vertices.size();
        indicesOffset += indices.size();
    }

    batchedMeshShader.use();

    batchedMeshShader.setUniform("uViewProjectionMatrix", &viewProjectionMatrix);

    batchedMeshShader.setAttrib("aColor", (float *)glm::value_ptr(batchedColors[0]), batchedColors.size() * 4, 4, GL_FLOAT, false, 0, GL_DYNAMIC_DRAW);

    batchedMeshShader.setAttrib("aPos", (float *)glm::value_ptr(batchedVertices[0]), batchedVertices.size() * 4, 4, GL_FLOAT, false, 0, GL_DYNAMIC_DRAW);
    batchedMeshShader.setIndices("aPos", &batchedIndices[0], batchedIndices.size(), GL_DYNAMIC_DRAW);

    batchedMeshShader.draw(GL_TRIANGLES, batchedIndices.size());
    batchedMeshShader.unbind();
}

void Rendering::Renderer::setClearColor(const Color &color)
{
    clearColor = color;
}

Rendering::Color Rendering::Renderer::getClearColor() const
{
    return clearColor;
}

void Rendering::Renderer::enableDepthTest(bool enable)
{
    if (enable)
    {
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }
}

bool Rendering::Renderer::isDepthTestEnabled() const
{
    return glIsEnabled(GL_DEPTH_TEST);
}

void Rendering::Renderer::enableAlphaBlending(bool enable)
{
    if (enable)
    {
        glEnable(GL_BLEND);
        glBlendFunc(alphaBlendingSFactor, alphaBlendingDFactor);
    }
    else
    {
        glDisable(GL_BLEND);
    }
}

void Rendering::Renderer::setAlphaBlendingFunction(GLenum sfactor, GLenum dfactor)
{
    if (!glIsValidAlphaBlendingFunction(sfactor) || !glIsValidAlphaBlendingFunction(dfactor))
    {
        throw std::invalid_argument("Renderer::setAlphaBlendingFunction: sfactor and dfactor must be valid values.");
    }

    alphaBlendingSFactor = sfactor;
    alphaBlendingDFactor = dfactor;
    glBlendFunc(sfactor, dfactor);
}

bool Rendering::Renderer::isAlphaBlendingEnabled() const
{
    return glIsEnabled(GL_BLEND);
}

void Rendering::Renderer::setSize(int w, int h)
{
    if (w <= 0 || h <= 0)
    {
        throw std::runtime_error("Renderer::resize: width and height must be greater than 0.");
    }

    width = w;
    height = h;

    glViewport(0, 0, width, height);

    if (syncCameraSizeWithRenderer)
    {
        camera.setSize(width, height);
    }
}

std::pair<int, int> Rendering::Renderer::getSize() const
{
    return std::make_pair(width, height);
}

std::pair<int, int> Rendering::Renderer::getWindowSize() const
{
    int width, height;
    glfwGetFramebufferSize(glfwWindow, &width, &height);

    return std::make_pair(width, height);
}

Rendering::Camera &Rendering::Renderer::getCamera()
{
    return camera;
}

void Rendering::Renderer::setCamera(Camera &camera)
{
    this->camera = camera;
}

void Rendering::Renderer::syncCameraSize(bool sync)
{
    syncCameraSizeWithRenderer = sync;
}

bool Rendering::Renderer::getSyncCameraSize() const
{
    return syncCameraSizeWithRenderer;
}

void Rendering::Renderer::updateViewProjectionMatrix()
{
    viewProjectionMatrix = camera.getViewProjectionMatrix();
}