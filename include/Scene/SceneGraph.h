#pragma once

#include "../ECS/Registry.h"
#include "../ECS/Entity.h"
#include "../Core/Transform.h"

#include <glm/glm.hpp>
#include <vector>
#include <unordered_set>

namespace Scene
{
    /**
     * Represents a scene graph.
     *
     * The scene graph is a tree structure that represents the scene. It is used to organize objects into a parent-child hierarchy.
     *
     * An entity will only be added to the scene graph if given a parent or children.
     *
     * Every entity in the scene graph must have a transform component. This is because the main purpose of the scene graph is to calculate the world transform of each entity.
     */
    class SceneGraph
    {
    public:
        /**
         * Creates a new scene graph.
         *
         * @param registry The registry entities in the scene graph belong to.
         */
        SceneGraph(const ECS::Registry *registry);

        /**
         * Destroys the scene graph.
         */
        ~SceneGraph();

        /**
         * Creates a parent-child relationship between the given entities.
         *
         * If the child entity already has a parent, it will be removed from its current parent and added to the new parent.
         *
         * @param parent The parent entity.
         * @param child The child entity.
         *
         * @throws std::invalid_argument If the parent or child entity does not have a transform component.
         */
        void relate(ECS::Entity parent, ECS::Entity child);

        /**
         * Creates a parent-child relationship between the given entities.
         *
         * If the child entity already has a parent, it will be removed from its current parent and added to the new parent.
         *
         * @param parent The parent entity.
         * @param children The child entities.
         *
         * @throws std::invalid_argument If the parent or any child entity does not have a transform component.
         */
        void relate(ECS::Entity parent, const std::vector<ECS::Entity> &children);

        /**
         * Removes the parent-child relationship from the entity.
         *
         * This will remove the parent of the given entity. Effectively makes the entity a child of the root node.
         *
         * Does nothing if the entity does not have a parent.
         *
         * @param entity The entity to unrelate.
         */
        void unrelate(ECS::Entity entity);

        /**
         * Gets the parent of the given entity.
         *
         * @param entity The entity to get the parent of.
         *
         * @returns The parent of the entity.
         *
         * @throws std::invalid_argument If the entity does not have a parent.
         */
        ECS::Entity parent(ECS::Entity entity) const;

        /**
         * Gets the children of the given entity.
         *
         * @param entity The entity to get the children of.
         *
         * @returns The children of the entity.
         */
        const std::unordered_set<ECS::Entity> &children(ECS::Entity entity) const;

        /**
         * Checks if the given entity has a parent.
         *
         * @param entity The entity to check.
         *
         * @returns True if the entity has a parent, false otherwise.
         */
        bool hasParent(ECS::Entity entity) const;

        /**
         * Checks if the given entity has children.
         *
         * @param entity The entity to check.
         *
         * @returns True if the entity has children, false otherwise.
         */
        bool hasChildren(ECS::Entity entity) const;

        /**
         * Gets the world transform of the given entity.
         *
         * The world transform is the transform of the entity in world space. It is calculated by multiplying the local transform of the entity with the world transform of its parent.
         *
         * If the entity does not have a parent, the world transform is the same as the local transform.
         *
         * @param entity The entity to get the world transform of.
         * @param forceUpdate If true, the world transform will be recalculated even if it has already been calculated.
         */
        const glm::mat4 &getWorldTransform(ECS::Entity entity, bool forceUpdate = false) const;

        /**
         * Updates the world transform of the given entity.
         *
         * This will recalculate the world transform of the entity.
         *
         * @param entity The entity to update the world transform of.
         * @param updateParent If true, the world transform of the parent will also be updated.
         * @param updateChildren If true, the world transform of the children (recursively) will also be updated.
         */
        void updateWorldTransform(ECS::Entity entity, bool updateParent = true, bool updateChildren = false) const;

        /**
         * Updates the world transforms of all entities in the scene graph.
         *
         * This will recalculate the world transform of each entity in the scene graph.
         */
        void updateWorldTransforms();

    private:
        const ECS::Registry *registry;

        /**
         * The parent-child relationships.
         */
        std::unordered_map<ECS::Entity, ECS::Entity> parents;

        /**
         * The children of each entity.
         */
        std::unordered_map<ECS::Entity, std::unordered_set<ECS::Entity>> childrenMap;

        /**
         * The world transforms of each entity.
         */
        mutable std::unordered_map<ECS::Entity, glm::mat4> worldTransforms;
    };
}