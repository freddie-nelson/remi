#include "../../../include/Rendering/Passes/OutputPass.h"

Rendering::OutputPass::OutputPass()
{
    createShader(fragShader);
}