#include "../../include/Rendering/Renderer.h"
#include "../../include/Rendering/Shader/MeshShader.h"
#include "../../include/Rendering/Shader/InstancedMeshShader.h"
#include "../../include/Rendering/Utility/OpenGLHelpers.h"
#include "../../include/Rendering/Mesh/Transforms.h"
#include "../../include/Rendering/Config.h"

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
}

void Rendering::Renderer::clear(bool clearColorBuffer, bool clearDepthBuffer, bool clearStencilBuffer)
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

void Rendering::Renderer::present()
{
    glfwSwapBuffers(glfwWindow);
}

void Rendering::Renderer::mesh(const Mesh2D &m)
{
    mesh(m, m.color);
}

void Rendering::Renderer::mesh(const Mesh2D &m, const Color &color)
{
    float *vertices = (float *)glm::value_ptr(m.vertices[0]);
    const std::vector<unsigned int> &indices = m.indices;

    // for (int i = 0; i < mesh.vertices.size(); i++)
    // {
    //     std::cout << vertices[i * 2] << ", " << vertices[i * 2 + 1] << std::endl;
    //     std::cout << mesh.vertices[i].x << ", " << mesh.vertices[i].y << std::endl;
    // }

    meshShader.use();

    auto colorVec = color.getColor();
    meshShader.setUniform("uColor", &colorVec);

    glm::vec2 resolution(width, height);
    // meshShader.setUniform("uResolution", &resolution);
    meshShader.setUniform("uViewProjectionMatrix", &viewProjectionMatrix);
    meshShader.setUniform("uMaxZIndex", &Config::MAX_Z_INDEX);

    unsigned int zIndex = m.zIndex;
    glm::vec2 translation = m.translation;
    glm::mat2 transform = getTransformationMatrix(m);

    meshShader.setUniform("uMeshZIndex", &zIndex);
    meshShader.setUniform("uMeshTranslation", &translation);
    meshShader.setUniform("uMeshTransform", &transform);

    meshShader.setAttrib("aPos", vertices, m.vertices.size() * 2, 2, GL_FLOAT, false, 0, GL_DYNAMIC_DRAW);
    meshShader.setIndices("aPos", &m.indices[0], m.indices.size(), GL_DYNAMIC_DRAW);

    meshShader.draw(GL_TRIANGLES, m.indices.size());
    meshShader.unbind();
};

void Rendering::Renderer::instancedMesh(const Mesh2D &m, std::vector<unsigned int> &zIndices, std::vector<glm::vec2> &translations, std::vector<glm::mat2> &transforms, std::vector<glm::vec4> &colors)
{
    if (translations.size() != transforms.size() || translations.size() != colors.size() || translations.size() != zIndices.size())
    {
        throw std::runtime_error("instancedMesh: zIndices, translations, transforms and colors must have the same size.");
    }

    const unsigned int instanceCount = translations.size();

    float *vertices = (float *)glm::value_ptr(m.vertices[0]);
    const std::vector<unsigned int> &indices = m.indices;

    instancedMeshShader.use();

    glm::vec2 resolution(width, height);
    // instancedMeshShader.setUniform("uResolution", &resolution);
    instancedMeshShader.setUniform("uViewProjectionMatrix", &viewProjectionMatrix);
    instancedMeshShader.setUniform("uMaxZIndex", &Config::MAX_Z_INDEX);

    // instanced arrays
    unsigned int *zIndicesArr = (unsigned int *)&zIndices[0];
    float *translationsArr = (float *)glm::value_ptr(translations[0]);
    float *transformsArr = (float *)glm::value_ptr(transforms[0]);

    instancedMeshShader.setAttrib("aZIndex", zIndicesArr, instanceCount, 1, GL_UNSIGNED_INT, false, 0, GL_DYNAMIC_DRAW, 1);
    instancedMeshShader.setAttrib("aTranslation", translationsArr, instanceCount * 2, 2, GL_FLOAT, false, 0, GL_DYNAMIC_DRAW, 1);
    instancedMeshShader.setAttrib("aTransform", transformsArr, instanceCount * 4, 4, GL_FLOAT, false, 0, GL_DYNAMIC_DRAW, 1, 2);

    float *colorsArr = (float *)glm::value_ptr(colors[0]);
    instancedMeshShader.setAttrib("aColor", colorsArr, instanceCount * 4, 4, GL_FLOAT, false, 0, GL_DYNAMIC_DRAW, 1);

    // vertices and indices
    instancedMeshShader.setAttrib("aPos", vertices, m.vertices.size() * 2, 2, GL_FLOAT, false, 0, GL_DYNAMIC_DRAW);
    instancedMeshShader.setIndices("aPos", &m.indices[0], m.indices.size(), GL_DYNAMIC_DRAW);

    instancedMeshShader.drawInstanced(instanceCount, GL_TRIANGLES, m.indices.size());
    instancedMeshShader.unbind();
};

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

void Rendering::Renderer::setCamera(const Camera &camera)
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