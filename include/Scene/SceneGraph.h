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
     *
     * The scene graph cannot be copied or moved.
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

        SceneGraph(const SceneGraph &) = delete;

        SceneGraph &operator=(const SceneGraph &) = delete;

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

         * Will also update the entity and its children's model matrices. 
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
         * Will also update the entity and its children's model matrices. 
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
         * Gets the model matrix of the given entity.
         *
         * The model matrix is the transformation of the entity in world space. It is calculated by multiplying the local transform of the entity with the model matrix of its parent.
         *
         * If the entity does not have a parent, the model matrix is the same as the local transform.
         *
         * @param entity The entity to get the model matrix of.
         * @param forceUpdate If true, the model matrix will be recalculated even if it has already been calculated.
         */
        const glm::mat4 &getModelMatrix(ECS::Entity entity, bool forceUpdate = false) const;

        /**
         * Gets the model matrix of the entity's parent.
         *
         * If the entity does not have a parent, the model matrix is the identity matrix.
         *
         * @param entity The entity to get the parent model matrix of.
         *
         * @returns The model matrix of the entity's parent.
         */
        const glm::mat4 &getParentModelMatrix(ECS::Entity entity, bool forceUpdate = false) const;

        /**
         * Updates the model matrix of the given entity.
         *
         * This will recalculate the model matrix of the entity.
         *
         * @param entity The entity to update the model matrix of.
         * @param updateParent If true, the model matrix of the parent will also be updated.
         * @param updateChildren If true, the model matrices of the children (recursively) will also be updated.
         */
        void updateModelMatrix(ECS::Entity entity, bool updateParent = true, bool updateChildren = false) const;

        /**
         * Updates the model matrices of all entities in the scene graph.
         *
         * This will recalculate the model matrices of each entity in the scene graph.
         * 
         * Also removes entities from the scene graph that are not in the registry.
         */
        void updateModelMatrices();

        /**
         * Removes entities from the scene graph that are not in the registry. 
        */
        void removeEntitiesNotInRegistry();

    private:
        const ECS::Registry *registry;

        glm::mat4 rootModelMatrix = glm::mat4(1.0f);

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
        mutable std::unordered_map<ECS::Entity, glm::mat4> modelMatrices;
    };
}