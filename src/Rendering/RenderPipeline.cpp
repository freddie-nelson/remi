#include "../../include/Rendering/RenderPipeline.h"
#include "../../include/Rendering/Passes/RenderablesPass.h"

#include <string>

Rendering::RenderPipeline::RenderPipeline()
{
}

Rendering::RenderPipeline::~RenderPipeline()
{
}

void Rendering::RenderPipeline::execute(RenderPassInput *input)
{
    RenderPassInput *output = input;

    for (auto pass : passes)
    {
        // std::cout << "executing " << pass.second->getName() << std::endl;
        output = pass.second->execute(output);
        // std::cout << "executed " << pass.second->getName() << std::endl;
    }

    auto *outputTyped = static_cast<RenderPassInputTyped<int> *>(output);
    delete outputTyped;
}

void Rendering::RenderPipeline::add(RenderPass *pass, unsigned int order)
{
    passes.emplace(order, pass);
}

void Rendering::RenderPipeline::remove(RenderPass *pass)
{
    for (auto it = passes.begin(); it != passes.end(); ++it)
    {
        if (it->second == pass)
        {
            passes.erase(it);
            break;
        }
    }
}

Rendering::RenderPass *Rendering::RenderPipeline::get(unsigned int order) const
{
    if (!has(order))
    {
        throw std::invalid_argument("No pass with order " + std::to_string(order) + " exists.");
    }

    return passes.at(order);
}

bool Rendering::RenderPipeline::has(unsigned int order) const
{
    return passes.contains(order);
}

std::string Rendering::RenderPipeline::toString() const
{
    std::string str = "[RenderPipeline]\n";

    for (auto &pass : passes)
    {
        str += "(" + std::to_string(pass.first) + "): " + pass.second->getName() + ",\n";
    }

    return str;
}