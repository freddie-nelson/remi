#include "../../../include/Rendering/Passes/OutputPass.h"

Rendering::OutputPass::OutputPass()
{
    outputToScreen = true;
    createShader(fragShader);
}