#include "../../../include/Rendering/Passes/CullingPass.h"
#include "../../../include/Rendering/Camera/Camera.h"
#include "../../../include/Core/BoundingCircle.h"
#include "../../../include/Rendering/Renderable.h"
#include "../../../include/Rendering/Mesh/Mesh.h"
#include "../../../include/Rendering/Passes/RenderablesPass.h"

#include <stack>

Rendering::RenderPassInput *Rendering::CullingPass::execute(RenderPassInput *input)
{
    checkInput<RenderablesPassData>(input);

    auto inputTyped = static_cast<RenderPassInputTyped<RenderablesPassData> *>(input);

    auto &world = *inputTyped->world;
    auto &registry = world.getRegistry();

    auto camera = inputTyped->camera;
    auto &entities = *inputTyped->data;

    // get camera aabb
    auto viewAabb = getCullingAABB(world, *inputTyped->spaceTransformer, camera);

    // get renderables
    std::vector<ECS::Entity> *renderables = new std::vector<ECS::Entity>;
    renderables->reserve(entities.size());

    std::vector<ECS::Entity> staticEntities;
    staticEntities.reserve(entities.size());

    std::vector<ECS::Entity> dynamicEntities;
    dynamicEntities.reserve(entities.size());

    for (auto &e : entities)
    {
        auto &renderable = registry.get<Renderable>(e);

        if (renderable.isStatic)
        {
            staticEntities.push_back(e);
        }
        else
        {
            dynamicEntities.push_back(e);
        }
    }

    // get static renderables
    getRenderables(world, staticEntities, viewAabb, true, *renderables);

    // get dynamic renderables
    getRenderables(world, dynamicEntities, viewAabb, false, *renderables);

    // create output
    RenderPassInputTyped<CullingPassData> *output = new RenderPassInputTyped<CullingPassData>(input, renderables);

    delete inputTyped;

    // drawAABBTree(staticRenderablesTree, const_cast<ECS::Registry &>(registry), *inputTyped->renderer, *inputTyped->renderTarget, *inputTyped->textureManager);
    // drawAABBTree(dynamicRenderablesTree, const_cast<ECS::Registry &>(registry), *inputTyped->renderer, *inputTyped->renderTarget, *inputTyped->textureManager);

    return output;
}

// ! TODO: implement proper coordinate space transformation/conversion class to handle this
// ! and get rid of the magic number 100.0f

Core::AABB Rendering::CullingPass::getCullingAABB(World::World &world, const Core::SpaceTransformer &spaceTransformer, const ECS::Entity camera) const
{
    auto &registry = world.getRegistry();
    auto &sceneGraph = world.getSceneGraph();

    auto &cameraComponent = registry.get<Camera>(camera);
    auto cameraTransform = Core::Transform(sceneGraph.getModelMatrix(camera));

    // if camera is rotated use bounding circle aabb
    if (cameraTransform.getRotation() != 0)
    {
        auto cameraAabb = cameraComponent.getAABB();

        // convert camera aabb to meters
        cameraAabb.scale(1.0f / spaceTransformer.getPixelsPerMeter());

        auto cameraBoundingCircle = Core::BoundingCircle(cameraAabb, cameraTransform);
        return Core::AABB(cameraBoundingCircle.getCentre(), cameraBoundingCircle.getRadius());
    }
    else
    {
        // otherwise scaled and translated aabb is sufficient
        return cameraComponent.getScaledAndTranslatedAabb(cameraTransform, spaceTransformer.getPixelsPerMeter());
    }
}

void Rendering::CullingPass::pruneTrees(const ECS::Registry &registry)
{
    auto &staticTreeEntities = staticRenderablesTree.getIds();
    std::vector<ECS::Entity> toRemove;

    for (auto &e : staticTreeEntities)
    {
        if (!registry.has<Renderable>(e))
        {
            toRemove.push_back(e);
        }
    }

    for (auto &e : toRemove)
    {
        staticRenderablesTree.remove(e);
        staticRenderables.erase(e);
    }

    auto &dynamicTreeEntities = dynamicRenderablesTree.getIds();
    toRemove.clear();

    for (auto &e : dynamicTreeEntities)
    {
        if (!registry.has<Renderable>(e))
        {
            toRemove.push_back(e);
        }
    }

    for (auto &e : toRemove)
    {
        dynamicRenderablesTree.remove(e);
        dynamicRenderables.erase(e);
    }
}

void Rendering::CullingPass::getRenderables(World::World &world, const std::vector<ECS::Entity> &entities, const Core::AABB &viewAabb, bool isStatic, std::vector<ECS::Entity> &renderables)
{
    auto &registry = world.getRegistry();
    auto &sceneGraph = world.getSceneGraph();

    // prune trees
    callsSinceLastTreePrune++;
    if (callsSinceLastTreePrune == treePruneFrequency)
    {
        // std::cout << "pruning trees\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
        //           << std::endl;

        pruneTrees(registry);
        callsSinceLastTreePrune = 0;
    }

    size_t otherCount = 0;

    std::unordered_set<ECS::Entity> includeInQuery;

    for (auto &e : entities)
    {
        auto &renderable = registry.get<Renderable>(e);
        if (!renderable.isVisible)
        {
            continue;
        }

        if (renderable.noCulling)
        {
            if ((isStatic && renderable.isStatic) || (!isStatic && !renderable.isStatic))
            {
                renderables.push_back(e);
            }

            continue;
        }

        includeInQuery.emplace(e);

        if (isStatic && dynamicRenderables.contains(e))
        {
            dynamicRenderablesTree.remove(e);
            dynamicRenderables.erase(e);
        }
        if (!isStatic && staticRenderables.contains(e))
        {
            staticRenderablesTree.remove(e);
            staticRenderables.erase(e);
        }

        // skip if already in tree and is static
        if (isStatic && staticRenderables.contains(e))
        {
            continue;
        }

        auto &transform = sceneGraph.getModelMatrix(e);
        auto &mesh = registry.get<Mesh2D>(e);

        auto boundingCircle = Core::BoundingCircle(mesh.getAABB(), transform);
        auto aabb = Core::AABB(boundingCircle.getCentre(), boundingCircle.getRadius());

        if (dynamicRenderablesTree.has(e))
        {
            dynamicRenderables.insert_or_assign(e, std::move(aabb));
            bool updated = dynamicRenderablesTree.update(e, dynamicRenderables[e]);

            continue;
        }

        if (isStatic)
        {
            staticRenderables.insert_or_assign(e, std::move(aabb));
            staticRenderablesTree.insert(e, staticRenderables[e]);
        }
        else
        {
            dynamicRenderables.insert_or_assign(e, std::move(aabb));
            dynamicRenderablesTree.insert(e, dynamicRenderables[e]);
        }
    }

    // get entities in view
    if (isStatic)
    {
        staticRenderablesTree.query(viewAabb, renderables, includeInQuery);
    }
    else
    {
        dynamicRenderablesTree.query(viewAabb, renderables, includeInQuery);
    }
}

void Rendering::CullingPass::drawAABBTree(const Core::AABBTree<ECS::Entity> &tree, World::World &world, const Rendering::Renderer &renderer, const Rendering::RenderTarget &renderTarget, Rendering::TextureManager &textureManager) const
{
    auto &registry = world.getRegistry();

    auto entity = registry.create();
    auto &transform = registry.add(entity, Core::Transform());
    auto &material = registry.add(entity, Rendering::Material());
    auto &mesh = registry.add(entity, Rendering::Mesh2D(1.0f, 1.0f));

    transform.setZIndex(Config::MAX_Z_INDEX);

    auto root = tree.getRoot();

    if (root == nullptr)
    {
        return;
    }

    struct AABBData
    {
        const Core::AABB *aabb;
        bool isLeaf;
        bool isFat;
    };

    float opacity = 1.0f;

    std::vector<AABBData> aabbs;

    std::stack<Core::AABBTreeNode<ECS::Entity> *> nodes;
    nodes.push(root);

    while (!nodes.empty())
    {
        auto node = nodes.top();
        nodes.pop();

        if (node->left != nullptr)
        {
            nodes.push(node->left);
        }
        if (node->right != nullptr)
        {
            nodes.push(node->right);
        }

        if (node->isLeaf())
        {
            aabbs.push_back({node->aabb, true, false});
            aabbs.push_back({&node->fatAabb, true, true});
        }
        else
        {
            aabbs.push_back({&node->fatAabb, false, true});
        }
    }

    renderTarget.bind(textureManager);

    for (auto &[aabb, isLeaf, isFat] : aabbs)
    {
        auto &centre = aabb->getCentre();

        transform.setTranslation(centre);
        transform.setScale(glm::vec2(aabb->getWidth(), aabb->getHeight()));

        // std::cout << "min: " << aabb->getMin().x << ", " << aabb->getMin().y << std::endl;
        // std::cout << "max: " << aabb->getMax().x << ", " << aabb->getMax().y << std::endl;

        if (isLeaf)
        {
            if (isFat)
            {
                material.setColor(Rendering::Color(1.0f, 0.0f, 0.0f, opacity));
                continue;
            }
            else
            {
                material.setColor(Rendering::Color(0.0f, 1.0f, 0.0f, opacity));
            }
        }
        else
        {
            material.setColor(Rendering::Color(0.0f, 0.0f, 1.0f, opacity));
            continue;
        }

        renderer.entity(world, renderer.getActiveCamera(registry), entity);
    }

    renderTarget.unbind(textureManager);
    registry.destroy(entity);
}