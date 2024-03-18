#include "../../../include/Rendering/Passes/CullingPass.h"
#include "../../../include/Rendering/Camera/Camera.h"
#include "../../../include/Core/BoundingCircle.h"
#include "../../../include/Rendering/Renderable.h"
#include "../../../include/Rendering/Mesh/Mesh.h"
#include "../../../include/Rendering/Passes/RenderablesPass.h"

#include <stack>
#include <iostream>
#include <fstream>

Rendering::RenderPassInput *Rendering::CullingPass::execute(RenderPassInput *input)
{
    checkInput<RenderablesPassData>(input);

    auto inputTyped = static_cast<RenderPassInputTyped<RenderablesPassData> *>(input);

    auto &world = *inputTyped->world;
    auto &registry = world.getRegistry();

    auto camera = inputTyped->camera;
    auto &data = *inputTyped->data;

    // get camera aabb
    auto viewAabb = getCullingAABB(world, *inputTyped->spaceTransformer, camera);

    // get renderables
    std::vector<ECS::Entity> *renderables = new std::vector<ECS::Entity>;
    renderables->reserve(data.staticRenderables.size() + data.dynamicRenderables.size());

    // get static renderables
    getRenderables(world, data.newStaticRenderables, viewAabb, true, *renderables);

    // get dynamic renderables
    getRenderables(world, data.dynamicRenderables, viewAabb, false, *renderables);

    // create output
    RenderPassInputTyped<CullingPassData> *output = new RenderPassInputTyped<CullingPassData>(input, renderables);

    delete inputTyped;

    // check for duplicates in renderables
    // std::unordered_set<ECS::Entity> uniqueRenderables;

    // for (auto &e : *renderables)
    // {
    //     if (uniqueRenderables.contains(e))
    //     {
    //         throw std::invalid_argument("Duplicate entity in renderables: " + std::to_string(e));
    //     }

    //     uniqueRenderables.insert(e);
    // }

    // std::cout << "static height: " << staticRenderablesTree.height() << std::endl;
    // std::cout << "node count: " << staticRenderablesTree.size() << std::endl;
    // std::cout << "leaf count: " << staticRenderablesTree.leafCount() << std::endl;
    // std::cout << "left count: " << staticRenderablesTree.leftNodeCount() << std::endl;
    // std::cout << "right count: " << staticRenderablesTree.rightNodeCount() << std::endl;

    // std::ofstream graphFile("graph.gv");
    // graphFile << staticRenderablesTree.toDebugString();
    // graphFile.close();

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

    auto &tree = isStatic ? staticRenderablesTree : dynamicRenderablesTree;
    auto &map = isStatic ? staticRenderables : dynamicRenderables;

    // prune trees
    callsSinceLastTreePrune++;
    if (callsSinceLastTreePrune == treePruneFrequency)
    {
        // std::cout << "pruning trees\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
        //           << std::endl;

        pruneTrees(registry);
        callsSinceLastTreePrune = 0;
    }

    size_t insertionCount = 0;

    // auto start = Core::timeSinceEpochMicrosec();

    for (auto &e : entities)
    {
        auto &renderable = registry.get<Renderable>(e);

        // skip if already in tree and is static
        if (isStatic && map.contains(e))
        {
            continue;
        }

        auto &transform = sceneGraph.getModelMatrix(e);
        auto &mesh = registry.get<Mesh2D>(e);

        auto boundingCircle = Core::BoundingCircle(mesh.getAABB(), transform);
        auto aabb = Core::AABB(boundingCircle.getCentre(), boundingCircle.getRadius());

        // update dyanmic renderables aabb
        if (!isStatic && tree.has(e))
        {
            map.insert_or_assign(e, std::move(aabb));
            bool updated = tree.update(e, map[e]);

            if (updated)
            {
                insertionCount++;
            }

            continue;
        }

        map.insert_or_assign(e, std::move(aabb));
        tree.insert(e, map[e]);
        insertionCount++;
    }

    // auto end = Core::timeSinceEpochMicrosec();
    // std::cout << "insert time: " << (end - start) << std::endl;

    // get entities in view
    // start = Core::timeSinceEpochMicrosec();
    auto visited = tree.query(viewAabb, renderables, true,
                              [&](const ECS::Entity &e)
                              { return registry.has<Renderable>(e); });
    // std::cout << "visited: " << visited << std::endl;

    auto aabbCount = tree.aabbCount();
    if (isStatic && aabbCount > treeMergeThreshold && insertionCount > treeMergeModifiedThreshold * aabbCount)
    {
        tree.mergeLeavesTillMin();
    }

    // end = Core::timeSinceEpochMicrosec();
    // std::cout << "query time: " << (end - start) << std::endl;
}

void Rendering::CullingPass::drawAABBTree(bool isStatic, World::World &world, const Rendering::Renderer &renderer, const Rendering::RenderTarget &renderTarget, Rendering::TextureManager &textureManager) const
{
    auto &tree = isStatic ? staticRenderablesTree : dynamicRenderablesTree;

    auto &registry = world.getRegistry();

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
        bool isLeftChild;
    };

    float opacity = 0.4f;

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
            aabbs.push_back({&node->fatAabb, true, true, node->isLeftChild()});

            for (auto &[entity, aabb] : node->aabbs)
            {
                // aabbs.push_back({aabb, true, false});
            }
        }
        else
        {
            aabbs.push_back({&node->fatAabb, false, true, node->isLeftChild()});
        }
    }

    renderTarget.bind(textureManager);

    auto entity = registry.create();
    auto &transform = registry.add(entity, Core::Transform());
    auto &material = registry.add(entity, Rendering::Material());
    auto &mesh = registry.add(entity, Rendering::Mesh2D(1.0f, 1.0f));

    for (auto &[aabb, isLeaf, isFat, isLeftChild] : aabbs)
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
                material.setColor(Rendering::Color(1.0f, isLeftChild ? 1.0f : 0.0f, 0.0f, opacity));
                transform.setZIndex(Config::MAX_Z_INDEX - 1);
            }
            else
            {
                material.setColor(Rendering::Color(0.0f, 1.0f, 0.0f, opacity));
                transform.setZIndex(Config::MAX_Z_INDEX);
            }
        }
        else
        {
            material.setColor(Rendering::Color(0.0f, isLeftChild ? 1.0f : 0.0f, 1.0f, opacity));
            transform.setZIndex(Config::MAX_Z_INDEX - 2);
        }

        renderer.entity(world, renderer.getActiveCamera(registry), entity);
    }

    // renderer.batch(world, renderer.getActiveCamera(registry), entities);
    renderTarget.unbind(textureManager);

    registry.destroy(entity);
}