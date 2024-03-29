#pragma once

#include "Entity.h"
#include "Component.h"
#include "SparseSet.h"
#include "../TypeList.h"
#include "../Core/Timestep.h"

#include <boost/unordered_map.hpp>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <vector>
#include <utility>
#include <iostream>
#include <queue>

namespace ECS
{
    /**
     * The registry is responsible for creating and managing entities.
     *
     * A registry cannot be copied or moved.
     */
    class Registry
    {
    public:
        /**
         * Creates a new registry.
         *
         * @param maxEntities The maximum number of entities that can be created.
         */
        Registry(size_t maxEntities);

        /**
         * Destroys the registry.
         */
        ~Registry();

        Registry(const Registry &other) = delete;

        Registry &operator=(const Registry &other) = delete;

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
         * Destroys a vector of entities and all their components.
         *
         * @param entities The entities to destroy.
         */
        void destroy(const std::vector<Entity> &entities);

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
            // std::set<ComponentId> cacheKey;
            // getCacheKey(TypeList<Types...>{}, cacheKey);

            if (cachedViews.contains(cacheKey))
            {
                // std::cout << "cached" << std::endl;
                return cachedViews[cacheKey].entities;
            }

            std::unordered_set<Entity> entitySet;
            viewHelper<Types...>(TypeList<Types...>{}, entitySet);

            auto cached = cachedViews.insert_or_assign(std::move(cacheKey), CachedView{Core::timeSinceEpochMicrosec(), std::vector<Entity>(entitySet.begin(), entitySet.end())});

            // for (Entity entity : cached.first->second)
            // {
            //     if (entity == 101)
            //     {
            //         std::cout << "entity: " << entity << std::endl;
            //     }
            // }

            return cached.first->second.entities;
        }

        /**
         * Gets the time the view of the given components was last cached.
         *
         * @tparam Types The types of components.
         *
         * @returns The time the view of the given components was last cached. If the components are not cached, 0 will be returned.
         */
        template <typename... Types>
        uint64_t viewCachedTime() const
        {
            std::set<ComponentId> cacheKey = {ComponentIdGenerator::id<Types>...};

            if (cachedViews.contains(cacheKey))
            {
                return cachedViews[cacheKey].timestamp;
            }

            return 0;
        }

        /**
         * Gets the entities that have been added since the view of the given components was last cached.
         *
         * This is useful for systems that need to know which entities have been added since the last time the view was cached.
         * i.e. for caching and performance reasons.
         *
         * @tparam Types The types of components.
         *
         * @returns The entities that have been added since the view of the given components was last cached.
         */
        template <typename... Types>
        const std::vector<Entity> &viewAddedSinceTimestamp() const
        {
            std::set<ComponentId> cacheKey = {ComponentIdGenerator::id<Types>...};

            if (cachedViews.contains(cacheKey))
            {
                return cachedViews[cacheKey].addedSinceTimestamp;
            }

            return std::vector<Entity>();
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
            if (!has(entity))
            {
                throw std::runtime_error("Registry (add): Entity '" + std::to_string(entity) + "' does not exist.");
            }

            // std::cout << "adding to " << entity << std::endl;
            // std::cout << "type name: " << typeid(T).name() << ", type id: " << ComponentIdGenerator::id<T> << std::endl;

            if (!hasComponentPool<T>())
            {
                createComponentPool<T>();
            }

            auto &componentPool = getComponentPool<T>();
            bool hasEntity = componentPool.has(entity);

            componentPool.add(entity, component);

            // only update cached views if the entity didn't already have the component
            if (!hasEntity)
            {
                updateCachedViews<T>(entity);
            }

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
            if (!has(entity))
            {
                return;
            }

            if (!hasComponentPool<T>())
            {
                return;
            }

            auto &componentPool = getComponentPool<T>();
            if (!componentPool.has(entity))
            {
                return;
            }

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
            if (!has<T>(entity))
            {
                throw std::runtime_error("Registry (get): Entity '" + std::to_string(entity) + "' does not have component '" + typeid(T).name() + "'.");
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
            if (!has(entity))
            {
                return false;
            }

            if (!hasComponentPool<T>())
            {
                return false;
            }

            SparseSet<T> &componentPool = getComponentPool<T>();
            return componentPool.has(entity);
        }

        /**
         * Gets all entities.
         *
         * @returns A reference to the vector of all entities.
         */
        const std::vector<Entity> &getEntities() const;

        /**
         * Gets the size of the registry.
         *
         * This is the number of entities in the registry.
         *
         * @returns The size of the registry.
         */
        size_t size() const;

    private:
        /**
         * The maximum number of entities that can be created.
         */
        size_t maxEntities;

        std::queue<Entity> freeEntityIds;

        /**
         * The entities in the registry.
         */
        std::vector<Entity> entities;

        /**
         * The entities in the registry as a set.
         */
        std::unordered_set<Entity> entitiesSet;

        /**
         * The threshold for when to invalidate the cached entity views instead of adding to `addedSinceTimestamp`.
         */
        size_t cacheUpdateInvalidationThreshold = 2500;

        /**
         * The component pools.
         *
         * These store sparse sets of components for each component type.
         */
        mutable std::unordered_map<ComponentId, SparseSetBase *> componentPools;

        /**
         * A cached view of entities with a set of components.
         *
         * @param timestamp The timestamp of the cache.
         * @param entities The entities in the cache.
         * @param addedSinceTimestamp The entities that have been added since the timestamp. These are entities added by `updateCachedViews`.
         */
        struct CachedView
        {
            uint64_t timestamp;
            std::vector<Entity> entities;
            std::vector<Entity> addedSinceTimestamp;
        };

        /**
         * The cached entity views.
         *
         * The key is a set of component IDs.
         *
         * The value is a vector of entities that have all the components in the key.
         */
        mutable boost::unordered_map<std::set<ComponentId>, CachedView> cachedViews;

        /**
         * Invalidates the cached entity views for the given component.
         *
         * @tparam T The type of component.
         */
        template <typename T>
        void invalidateCachedViews()
        {
            auto componentId = ComponentIdGenerator::id<T>;

            invalidateCachedViews(componentId);
        }

        /**
         * Invalidates the cached entity views for the given component.
         *
         * @param componentId The ID of the component.
         */
        void invalidateCachedViews(ComponentId componentId)
        {
            auto it = cachedViews.begin();

            while (it != cachedViews.end())
            {
                auto &[components, view] = *it;

                if (components.contains(componentId))
                {
                    // erase and update iterator to next element
                    it = cachedViews.erase(it);
                }
                else
                {
                    // increment iterator
                    it++;
                }
            }
        }

        /**
         * Adds the given entity to the cached views for the given component.
         *
         * @tparam T The type of component.
         *
         * @param e The entity to add to the cached views.
         */
        template <typename T>
        void updateCachedViews(Entity e)
        {
            auto componentId = ComponentIdGenerator::id<T>;

            updateCachedViews(componentId, e);
        }

        /**
         * Adds the given entity to the cached views for the given component.
         *
         * @param componentId The ID of the component.
         * @param e The entity to add to the cached views.
         */
        void updateCachedViews(ComponentId componentId, ECS::Entity e)
        {
            for (auto &[components, view] : cachedViews)
            {
                if (!components.contains(componentId))
                {
                    continue;
                }

                bool hasAllComponents = true;

                for (auto &componentId : components)
                {
                    if (!componentPools.contains(componentId))
                    {
                        hasAllComponents = false;
                        break;
                    }

                    auto componentPool = getComponentPool(componentId);
                    if (!componentPool->has(e))
                    {
                        hasAllComponents = false;
                        break;
                    }
                }

                if (hasAllComponents)
                {
                    view.entities.push_back(e);
                    view.addedSinceTimestamp.push_back(e);

                    if (view.addedSinceTimestamp.size() > cacheUpdateInvalidationThreshold)
                    {
                        view.addedSinceTimestamp.clear();
                        view.timestamp = Core::timeSinceEpochMicrosec();
                    }
                }
            }
        }

        /**
         * Helper function for to get cached views key.
         *
         * @param t The type list.
         * @param key The key to add to.
         *
         * @tparam T The type of component.
         * @tparam Types The remaining types of components.
         */
        template <typename T, typename... Types>
        void getCacheKey(TypeList<T, Types...> t, std::set<ComponentId> &key) const
        {
            getCacheKey<T>(TypeList<T>{}, key);
            getCacheKey<Types...>(TypeList<Types...>{}, key);
        }

        /**
         * Helper function for to get cached views key.
         *
         * This is the base case for the recursion.
         *
         * @param t The type list.
         * @param key The key to add to.
         *
         * @tparam T The type of component.
         */
        template <typename T>
        void getCacheKey(TypeList<T> t, std::set<ComponentId> &key) const
        {
            key.insert(ComponentIdGenerator::id<T>);
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
            // std::cout << "viewHelper: " << depth << std::endl;
            // std::cout << "type name: " << typeid(T).name() << ", type id: " << ComponentIdGenerator::id<T> << std::endl;

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
                    entitySet.insert(std::move(entity));
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

                for (auto it = entitySet.begin(); it != entitySet.end();)
                {
                    if (!entitySetT.contains(*it))
                    {
                        it = entitySet.erase(it);
                    }
                    else
                    {
                        it++;
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

            if (componentPools.contains(componentId))
            {
                throw std::runtime_error("Registry (createComponentPool): Component pool already exists.");
            }

            SparseSet<T> *componentPool = new SparseSet<T>(maxEntities - 1);

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

            if (!componentPools.contains(componentId))
            {
                throw std::runtime_error("Registry (getComponentPool): Component pool does not exist.");
            }

            return *reinterpret_cast<SparseSet<T> *>(componentPools[componentId]);
        }

        SparseSetBase *getComponentPool(ComponentId componentId) const
        {
            if (!componentPools.contains(componentId))
            {
                throw std::runtime_error("Registry (getComponentPool): Component pool does not exist.");
            }

            return componentPools[componentId];
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

            return componentPools.contains(componentId);
        }
    };
}