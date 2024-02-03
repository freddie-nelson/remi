#include "../../../include/Rendering/Passes/BrightnessPass.h"

#include <stdexcept>

Rendering::BrightnessPass::BrightnessPass(float brightness)
{
    createShader(fragShader);
    setBrightness(brightness);
}

Rendering::BrightnessPass::~BrightnessPass()
{
    delete brightnessUniform;
}

float Rendering::BrightnessPass::getBrightness()
{
    return brightness;
}

void Rendering::BrightnessPass::setBrightness(float brightness)
{
    if (brightness < 0.0f || brightness > 1.0f)
    {
        throw std::invalid_argument("Brightness must be between 0 and 1 (inclusive).");
    }

    this->brightness = brightness;

    if (brightnessUniform != nullptr)
        delete brightnessUniform;

    brightnessUniform = new Uniform("uBrightness", this->brightness);
    uniform(brightnessUniform);
}