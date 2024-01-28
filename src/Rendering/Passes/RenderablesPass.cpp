#include "../../../include/Rendering/Passes/RenderablesPass.h"
#include "../../../include/Rendering/Mesh/Mesh.h"
#include "../../../include/Rendering/Renderable.h"
#include "../../../include/Rendering/Material/Material.h"
#include "../../../include/Rendering/Material/ShaderMaterial.h"
#include "../../../include/Core/Transform.h"

Rendering::RenderPassInput *Rendering::RenderablesPass::execute(RenderPassInput *input)
{
    checkInput<int>(input);

    auto *inputTyped = static_cast<RenderPassInputTyped<int> *>(input);
    auto &registry = *input->registry;

    // get entities
    auto &entities = registry.view<Mesh2D, Core::Transform, Renderable>();

    // check if entities have materials
    for (auto e : entities)
    {
        if (!registry.has<Material>(e) && !registry.has<ShaderMaterial>(e))
        {
            throw std::invalid_argument("Entity " + std::to_string(e) + " does not have a material or a shader material.");
        }
    }

    // create output
    RenderablesPassData *data = new std::vector<ECS::Entity>(entities.begin(), entities.end());
    auto *output = new RenderPassInputTyped(input, data);

    delete inputTyped;

    return output;
}