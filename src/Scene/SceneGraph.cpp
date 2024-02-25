#include "../../include/Scene/SceneGraph.h"

#include <string>
#include <stdexcept>

Scene::SceneGraph::SceneGraph(const ECS::Registry *registry) : registry(registry)
{
}

Scene::SceneGraph::~SceneGraph()
{
}

void Scene::SceneGraph::relate(ECS::Entity parent, ECS::Entity child)
{
    if (!registry->has<Core::Transform>(parent))
    {
        throw std::invalid_argument("SceneGraph (relate): Parent entity does not have a transform component.");
    }

    if (!registry->has<Core::Transform>(child))
    {
        throw std::invalid_argument("SceneGraph (relate): Child entity does not have a transform component.");
    }

    if (hasParent(child))
    {
        unrelate(child);
    }

    childrenMap[parent].emplace(child);
    parents[child] = parent;

    updateModelMatrix(child, false, true);
}

void Scene::SceneGraph::relate(ECS::Entity parent, const std::vector<ECS::Entity> &children)
{
    for (auto child : children)
    {
        relate(parent, child);
    }
}

void Scene::SceneGraph::unrelate(ECS::Entity entity)
{
    if (!hasParent(entity))
    {
        return;
    }

    auto parent = parents[entity];
    parents.erase(entity);

    childrenMap[parent].erase(entity);

    updateModelMatrix(entity, false, true);
}

ECS::Entity Scene::SceneGraph::parent(ECS::Entity entity) const
{
    if (!hasParent(entity))
    {
        throw std::invalid_argument("SceneGraph (parent): Entity '" + std::to_string(entity) + "' has no parent.");
    }

    return parents.at(entity);
}

const std::unordered_set<ECS::Entity> &Scene::SceneGraph::children(ECS::Entity entity) const
{
    if (!hasChildren(entity))
    {
        throw std::invalid_argument("SceneGraph (children): Entity '" + std::to_string(entity) + "' has no children.");
    }

    return childrenMap.at(entity);
}

bool Scene::SceneGraph::hasParent(ECS::Entity entity) const
{
    return parents.contains(entity);
}

bool Scene::SceneGraph::hasChildren(ECS::Entity entity) const
{
    return childrenMap.contains(entity);
}

const glm::mat4 &Scene::SceneGraph::getModelMatrix(ECS::Entity entity, bool forceUpdate) const
{
    if (forceUpdate)
    {
        updateModelMatrix(entity, true, false);
    }

    if (!modelMatrices.contains(entity))
    {
        updateModelMatrix(entity, true, false);
    }

    return modelMatrices.at(entity);
}

const glm::mat4 &Scene::SceneGraph::getParentModelMatrix(ECS::Entity entity, bool forceUpdate) const
{
    if (!hasParent(entity))
    {
        return rootModelMatrix;
    }

    return getModelMatrix(parent(entity), forceUpdate);
}

void Scene::SceneGraph::updateModelMatrix(ECS::Entity entity, bool updateParent, bool updateChildren) const
{
    if (!registry->has<Core::Transform>(entity))
    {
        throw std::invalid_argument("SceneGraph (updateModelMatrices): Entity '" + std::to_string(entity) + "' does not have a transform component.");
    }

    if (!hasParent(entity))
    {
        modelMatrices[entity] = registry->get<Core::Transform>(entity).getTransformationMatrix();
    }
    else
    {
        auto parent = this->parent(entity);
        auto &parentModelMatrix = getModelMatrix(parent, updateParent);

        auto &transform = registry->get<Core::Transform>(entity);
        auto &localTransform = transform.getTransformationMatrix();

        modelMatrices[entity] = parentModelMatrix * localTransform;

        // always use z index of entity
        // 4th/last column, 3rd row of model matrix is the z index
        modelMatrices[entity][3][2] = localTransform[3][2];
    }

    if (updateChildren && hasChildren(entity))
    {
        auto &children = this->children(entity);
        for (auto &child : children)
        {
            updateModelMatrix(child, false, true);
        }
    }
}

void Scene::SceneGraph::updateModelMatrices()
{
    auto entities = registry->view<Core::Transform>();

    std::vector<ECS::Entity> rootEntities;
    rootEntities.reserve(entities.size());

    for (auto entity : entities)
    {
        if (!hasParent(entity))
        {
            rootEntities.push_back(entity);
        }
    }

    // remove entities no longer in registry
    removeEntitiesNotInRegistry();

    // recurse down tree updating model matrices
    for (auto &rootEntity : rootEntities)
    {
        updateModelMatrix(rootEntity, false, true);
    }
}

void Scene::SceneGraph::removeEntitiesNotInRegistry()
{
    for (auto &[e, _] : modelMatrices)
    {
        if (!registry->has(e) || !registry->has<Core::Transform>(e))
        {
            modelMatrices.erase(e);
            unrelate(e);
        }
    }
}
