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

const glm::mat4 &Scene::SceneGraph::getWorldTransform(ECS::Entity entity, bool forceUpdate) const
{
    if (forceUpdate)
    {
        updateWorldTransform(entity, true, false);
    }

    if (!worldTransforms.contains(entity))
    {
        updateWorldTransform(entity, true, true);
    }

    return worldTransforms.at(entity);
}

void Scene::SceneGraph::updateWorldTransform(ECS::Entity entity, bool updateParent, bool updateChildren) const
{
    if (!registry->has<Core::Transform>(entity))
    {
        throw std::invalid_argument("SceneGraph (updateWorldTransform): Entity '" + std::to_string(entity) + "' does not have a transform component.");
    }

    if (!hasParent(entity))
    {
        worldTransforms[entity] = registry->get<Core::Transform>(entity).getTransformationMatrix();
    }
    else
    {
        auto parent = this->parent(entity);
        auto &parentWorldTransform = getWorldTransform(parent, updateParent);

        worldTransforms[entity] = parentWorldTransform * registry->get<Core::Transform>(entity).getTransformationMatrix();
    }

    if (updateChildren && hasChildren(entity))
    {
        auto &children = this->children(entity);
        for (auto &child : children)
        {
            updateWorldTransform(child, false, true);
        }
    }
}

void Scene::SceneGraph::updateWorldTransforms()
{
    std::vector<ECS::Entity> rootEntities;
    rootEntities.reserve(childrenMap.size());

    for (auto &[parent, children] : childrenMap)
    {
        if (!hasParent(parent))
        {
            rootEntities.push_back(parent);
        }
    }

    for (auto &rootEntity : rootEntities)
    {
        updateWorldTransform(rootEntity, false, true);
    }
}
