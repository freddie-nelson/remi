#include "../../../include/Rendering/Passes/GaussianBlurPass.h"

Rendering::GaussianBlurPass::GaussianBlurPass(float radius, float quality, float directions)
{
    createShader(fragShader);

    setRadius(radius);
    setQuality(quality);
    setDirections(directions);
}

Rendering::GaussianBlurPass::~GaussianBlurPass()
{
    delete radiusUniform;
    delete qualityUniform;
    delete directionsUniform;
}

float Rendering::GaussianBlurPass::getRadius()
{
    return radius;
}

void Rendering::GaussianBlurPass::setRadius(float radius)
{
    this->radius = radius;

    if (radiusUniform != nullptr)
        delete radiusUniform;

    radiusUniform = new Uniform("uRadius", this->radius);
    uniform(radiusUniform);
}

float Rendering::GaussianBlurPass::getQuality()
{
    return quality;
}

void Rendering::GaussianBlurPass::setQuality(float quality)
{
    this->quality = quality;

    if (qualityUniform != nullptr)
        delete qualityUniform;

    qualityUniform = new Uniform("uQuality", this->quality);
    uniform(qualityUniform);
}

float Rendering::GaussianBlurPass::getDirections()
{
    return directions;
}

void Rendering::GaussianBlurPass::setDirections(float directions)
{
    this->directions = directions;

    if (directionsUniform != nullptr)
        delete directionsUniform;

    directionsUniform = new Uniform("uDirections", this->directions);
    uniform(directionsUniform);
}
