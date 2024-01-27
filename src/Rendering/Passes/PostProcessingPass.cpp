#include "../../../include/Rendering/Passes/PostProcessingPass.h"
#include "../../../include/Rendering/RenderTarget.h"

#include <stdexcept>

const std::string Rendering::PostProcessingPass::vertexShader =
    "#version 300 es\n"
    "\n"
    "precision mediump float;\n"
    "\n"
    "in vec2 aPos;\n"
    "in vec2 aTexCoord;\n"
    "\n"
    "out vec2 vTexCoord;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos, 0.0f, 1.0f);\n"
    "   vTexCoord = aTexCoord;\n"
    "}\n";

const std::vector<glm::vec2> Rendering::PostProcessingPass::vertices = {
    glm::vec2(-1.0f, -1.0f),
    glm::vec2(1.0f, -1.0f),
    glm::vec2(1.0f, 1.0f),
    glm::vec2(-1.0f, 1.0f),
};

const std::vector<glm::vec2> Rendering::PostProcessingPass::texCoords = {
    glm::vec2(0.0f, 0.0f),
    glm::vec2(1.0f, 0.0f),
    glm::vec2(1.0f, 1.0f),
    glm::vec2(0.0f, 1.0f),
};

const std::vector<unsigned int> Rendering::PostProcessingPass::indices = {
    0,
    1,
    2,
    2,
    3,
    0,
};

Rendering::RenderPassInput *Rendering::PostProcessingPass::execute(RenderPassInput *input)
{
    // checkInput<RenderTarget>(input);
    input->renderTarget->bind(*input->textureManager);

    // create uniforms
    Uniform uRenderTexture("uRenderTexture", input->textureManager->getRenderTargetTextureUnit());

    // create attribs
    VertexAttrib aPos("aPos", vertices);
    VertexAttrib aTexCoord("aTexCoord", texCoords);

    // create indices
    VertexIndices indices(indices);

    // setup shader
    shader.use();

    // setup uniforms
    shader.uniform(&uRenderTexture);

    // user uniforms
    shader.uniform(uniforms);

    // attributes
    shader.attrib(&aPos);
    shader.attrib(&aTexCoord);

    // indices
    shader.indices(&indices);

    // draw
    shader.draw(GL_TRIANGLES, indices.size());

    // unbind
    shader.unbind();
    input->renderTarget->unbind();

    return input;
}

const Rendering::Shader &Rendering::PostProcessingPass::getShader() const
{
    return shader;
}

void Rendering::PostProcessingPass::uniform(UniformBase *u)
{
    uniforms[u->getName()] = u;
}

void Rendering::PostProcessingPass::createShader(std::string fragmentShader)
{
    shader = Shader();
    if (!shader.loadFromSource(vertexShader, fragmentShader))
    {
        throw std::runtime_error("Failed to load shader for post processor.");
    }
}