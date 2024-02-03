#include "../../../include/Rendering/Passes/PosterizePass.h"

Rendering::PosterizePass::PosterizePass(float steps)
{
    createShader(fragShader);
    setSteps(steps);
}

Rendering::PosterizePass::~PosterizePass()
{
    delete stepsUniform;
}

float Rendering::PosterizePass::getSteps()
{
    return steps;
}

void Rendering::PosterizePass::setSteps(float steps)
{
    if (steps < 1.0f)
    {
        throw std::invalid_argument("Steps must be greater than or equal to 1.");
    }

    this->steps = steps;

    if (stepsUniform != nullptr)
        delete stepsUniform;

    stepsUniform = new Uniform("uSteps", this->steps);
    uniform(stepsUniform);
}