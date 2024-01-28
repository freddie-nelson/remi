#include "../../../include/Rendering/Passes/ColorBlendPass.h"

Rendering::ColorBlendPass::ColorBlendPass(Color color)
{
    createShader(fragShader);
    setColor(color);
}

Rendering::ColorBlendPass::~ColorBlendPass()
{
    delete colorUniform;
}

const Rendering::Color &Rendering::ColorBlendPass::getColor()
{
    return color;
}

void Rendering::ColorBlendPass::setColor(Color color)
{
    this->color = color;
    colorVec = color.getColor();

    if (colorUniform != nullptr)
        delete colorUniform;

    colorUniform = new Uniform("uColor", colorVec);
    uniform(colorUniform);
}