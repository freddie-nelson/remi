#pragma once

#include "Entity.h"
#include "Component.h"
#include "SparseSet.h"
#include "../TypeList.h"

#include <boost/unordered_map.hpp>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <vector>
#include <utility>
#include <iostream>

namespace ECS
{
    /**
     * The registry is responsible for creating and managing entities.
     */
    class Registry
    {
    public:
        /**
         * Creates a new registry.
         */
        Registry();

        /**
         * Destroys the registry.
         */
        ~Registry();

        /**
         * Creates a new entity.
         *
         * @returns A reference to the new entity.
         */
        Entity create();

        /**
         * Destroys an entity and all its components.
         *
         * @param entity The entity to destroy.
         */
        void destroy(Entity entity);

        /**
         * Destroys all entities and all components.
         */
        void destroyAll();

        /**
         * Returns whether or not the registry has the given entity.
         *
         * @param entity The entity to check.
         *
         * @returns Whether or not the registry has the given entity.
         */
        bool has(Entity entity) const;

        /**
         * Gets all the entities that have the given components.
         *
         * @tparam Types The types of components.
         *
         * @returns A vector of entities with the given components.
         */
        template <typename... Types>
        const std::vector<Entity> &view() const
        {
            std::set<ComponentId> cacheKey = {ComponentIdGenerator::id<Types>...};

            if (cachedViews.count(cacheKey) != 0)
            {
                // std::cout << "cached" << std::endl;
                return cachedViews[cacheKey];
            }

            std::unordered_set<Entity> entitySet;
            viewHelper<Types...>(TypeList<Types...>{}, entitySet);

            cachedViews[cacheKey] = std::vector<Entity>(entitySet.begin(), entitySet.end());

            return cachedViews[cacheKey];
        }

        /**
         * Adds a component to the given entity.
         *
         * If the component pool does not exist, it will be created.
         *
         * The component passed in will be copied, so the original component can be destroyed after this function is called.
         *
         * And the reference returned should be used for modifying the component data.
         *
         * @tparam T The type of component.
         *
         * @param entity The entity to add the component to.
         * @param component The component to add.
         *
         * @returns A reference to the component.
         */
        template <typename T>
        T &add(Entity entity, T component)
        {
            if (!hasComponentPool<T>())
            {
                createComponentPool<T>();
            }

            auto &componentPool = getComponentPool<T>();
            componentPool.add(entity, component);

            invalidateCachedViews<T>();

            return componentPool.get(entity);
        }

        /**
         * Removes a component from the given entity.
         *
         * If the component does not exist, nothing will happen.
         *
         * @tparam T The type of component.
         *
         * @param entity The entity to remove the component from.
         */
        template <typename T>
        void remove(Entity entity)
        {
            if (!hasComponentPool<T>())
            {
                return;
            }

            auto &componentPool = getComponentPool<T>();
            componentPool.remove(entity);

            invalidateCachedViews<T>();
        }

        /**
         * Gets the component for the given entity.
         *
         * @tparam T The type of component.
         *
         * @param entity The entity to get the component for.
         *
         * @returns A reference to the component.
         */
        template <typename T>
        T &get(Entity entity) const
        {
            if (!hasComponentPool<T>())
            {
                throw std::runtime_error("Registry (get): Component pool does not exist.");
            }

            return getComponentPool<T>().get(entity);
        }

        /**
         * Returns whether or not the given entity has the given component.
         *
         * @tparam T The type of component.
         *
         * @param entity The entity to check.
         *
         * @returns Whether or not the entity has the component.
         */
        template <typename T>
        bool has(Entity entity) const
        {
            if (!hasComponentPool<T>())
            {
                return false;
            }

            return getComponentPool<T>().has(entity);
        }

        /**
         * Gets all entities.
         *
         * @returns A reference to the vector of all entities.
         */
        std::vector<Entity> &getAllEntities();

    private:
        /**
         * The entities in the registry.
         */
        std::vector<Entity> entities;

        /**
         * The entities in the registry as a set.
         */
        std::unordered_set<Entity> entitiesSet;

        /**
         * The component pools.
         *
         * These store sparse sets of components for each component type.
         */
        mutable std::unordered_map<ComponentId, SparseSetBase *> componentPools;

        /**
         * The cached entity views.
         *
         * The key is a set of component IDs.
         *
         * The value is a vector of entities that have all the components in the key.
         */
        mutable boost::unordered_map<std::set<ComponentId>, std::vector<Entity>> cachedViews;

        /**
         * Invalidates the cached entity views for the given component.
         *
         * @tparam T The type of component.
         */
        template <typename T>
        void invalidateCachedViews()
        {
            auto componentId = ComponentIdGenerator::id<T>;

            for (auto &[components, view] : cachedViews)
            {
                if (components.count(componentId) != 0)
                {
                    cachedViews.erase(components);
                }
            }
        }

        /**
         * Helper function for view.
         *
         * @tparam T The type of component.
         * @tparam Types The remaining types of components.
         *
         * @param entitySet The set of entities to add to.
         * @param depth The depth of the recursion.
         */
        template <typename T, typename... Types>
        void viewHelper(TypeList<T, Types...> t, std::unordered_set<Entity> &entitySet, int depth = 0) const
        {
            viewHelper<T>(TypeList<T>{}, entitySet, depth);
            viewHelper<Types...>(TypeList<Types...>{}, entitySet, depth + 1);
        }

        /**
         * Helper function for view.
         *
         * This is the base case for the recursion.
         *
         * @tparam T The type of component.
         *
         * @param entitySet The set of entities to add to.
         * @param depth The depth of the recursion.
         */
        template <typename T>
        void viewHelper(TypeList<T> t, std::unordered_set<Entity> &entitySet, int depth = 0) const
        {
            // no possible matches
            if (!hasComponentPool<T>())
            {
                entitySet.clear();
                return;
            }

            const auto &entitiesT = getComponentPool<T>().getDenseIds();
            if (depth == 0)
            {
                for (Entity entity : entitiesT)
                {
                    entitySet.insert(entity);
                }
            }
            else
            {
                // no possible matches
                if (entitiesT.size() == 0 || entitySet.size() == 0)
                {
                    entitySet.clear();
                    return;
                }

                // remove entities that don't have the component
                std::unordered_set<Entity> entitySetT(entitiesT.begin(), entitiesT.end());

                for (Entity entity : entitySetT)
                {
                    if (entitySetT.count(entity) == 0)
                    {
                        entitySet.erase(entity);
                    }
                }
            }
        }

        /**
         * Creates a component pool for the given component type.
         *
         * If the component pool already exists, an exception will be thrown.
         *
         * @tparam T The type of component.
         *
         * @returns A reference to the component pool.
         */
        template <typename T>
        SparseSet<T> &createComponentPool()
        {
            ComponentId componentId = ComponentIdGenerator::id<T>;

            if (componentPools.count(componentId) != 0)
            {
                throw std::runtime_error("Registry (createComponentPool): Component pool already exists.");
            }

            SparseSet<T> *componentPool = new SparseSet<T>();

            componentPools[componentId] = reinterpret_cast<SparseSetBase *>(componentPool);

            return *componentPool;
        }

        /**
         * Gets the component pool for the given component type.
         *
         * If the component pool does not exist, an exception will be thrown.
         *
         * @tparam T The type of component.
         *
         * @returns A reference to the component pool.
         */
        template <typename T>
        SparseSet<T> &getComponentPool() const
        {
            ComponentId componentId = ComponentIdGenerator::id<T>;

            if (componentPools.count(componentId) == 0)
            {
                throw std::runtime_error("Registry (getComponentPool): Component pool does not exist.");
            }

            return *reinterpret_cast<SparseSet<T> *>(componentPools[componentId]);
        }

        /**
         * Returns whether or not the registry has a component pool for the given component type.
         *
         * @tparam T The type of component.
         *
         * @returns Whether or not the registry has a component pool for the given component type.
         */
        template <typename T>
        bool hasComponentPool() const
        {
            ComponentId componentId = ComponentIdGenerator::id<T>;

            return componentPools.count(componentId) != 0;
        }
    };
}