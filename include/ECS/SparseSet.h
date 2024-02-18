#pragma once

#include <vector>
#include <stdexcept>
#include <string>

#define ECS_SPARSE_SET_MAX_ID 16777215
#define ECS_SPARSE_SET_DEFAULT_MAX_ID 65535

namespace ECS
{

    class SparseSetBase
    {
    public:
        SparseSetBase() = default;
        virtual ~SparseSetBase() = default;

        /**
         * Removes an item from the sparse set.
         *
         * @param id The ID of the item to remove.
         */
        virtual void remove(size_t id) = 0;

        /**
         * Checks if the sparse set contains the given id.
         *
         * @param id The ID to check for.
         *
         * @returns Whether or not the sparse set contains the given id.
         */
        virtual bool has(size_t id) = 0;

        /**
         * Returns the number of items in the set.
         *
         * @returns The number of items in the set.
         */
        virtual size_t size() = 0;

    protected:
        size_t maxId = 0;
        size_t NULL_INDEX = 0;
    };

    /**
     * A sparse set consists of a dense and sparse vector.
     *
     * The sparse vector is an integer array where each value is an index into the dense vector.
     *
     * The index in the sparse vector is the ID of the item we want to store.
     *
     * The dense vector is a tightly packed unsorted array of the data we want to store for the ids i.e. entities.
     *
     * The data in the set should not be pointer data for efficient cache usage.
     */
    template <typename T>
    class SparseSet : private SparseSetBase
    {
    public:
        /**
         * Creates a new sparse set.
         *
         * The max ID cannot be changed after the sparse set is created.
         *
         * @param maxId The maximum ID of the items we want to store, by default this is ECS_SPARSE_SET_DEFAULT_MAX_ID (65535). The max value is ECS_SPARSE_SET_MAX_ID.
         */
        SparseSet(size_t maxId = ECS_SPARSE_SET_DEFAULT_MAX_ID)
        {
            if (maxId > ECS_SPARSE_SET_MAX_ID)
            {
                throw std::runtime_error("SparseSet (SparseSet): Max ID is greater than " + std::to_string(ECS_SPARSE_SET_MAX_ID));
            }

            this->maxId = maxId;
            NULL_INDEX = maxId + 1;

            try {
                denseIds = std::vector<size_t>();
                dense = std::vector<T>();
                sparse = std::vector<size_t>(maxId + 1, NULL_INDEX);
            } catch (std::bad_alloc &e) {
                throw std::runtime_error("SparseSet (SparseSet): Could not allocate memory for sparse set for type '" + std::string(typeid(T).name()) + "'.");
            }
        }

        /**
         * Destroys the sparse set.
         */
        ~SparseSet()
        {
            dense.clear();
            sparse.clear();
        }

        /**
         * Adds an item to the sparse set.
         *
         * If the ID already exists, the old item will be overwritten.
         *
         * @param id The ID of the item to add.
         * @param item The item to add.
         */
        void add(size_t id, T item)
        {
            if (id > maxId)
            {
                throw std::runtime_error("SparseSet (add): ID is greater than max ID.");
            }

            if (has(id))
            {
                // update the value at the dense vector
                dense[sparse[id]] = item;
            }
            else
            {
                // add the item to the dense vector
                denseIds.push_back(id);
                dense.push_back(item);
                sparse[id] = dense.size() - 1;
            }
        }

        /**
         * Removes an item from the sparse set.
         *
         * If the ID does not exist, nothing will happen.
         *
         * @param id The ID of the item to remove.
         */
        void remove(size_t id)
        {
            if (!has(id))
            {
                return;
            }

            size_t index = sparse[id];

            // swap the item we want to remove with the last item in the dense vector
            auto last = dense.back();
            auto lastId = denseIds.back();
            auto lastIndex = dense.size() - 1;

            dense[lastIndex] = dense[index];
            denseIds[lastIndex] = denseIds[index];

            dense[index] = std::move(last);
            denseIds[index] = lastId;

            // update the sparse vector
            sparse[lastId] = index;
            sparse[id] = NULL_INDEX;

            // remove the last item in the dense vector (which is the item we want to remove)
            dense.pop_back();
            denseIds.pop_back();
        }

        /**
         * Gets an item from the sparse set.
         *
         * @param id The ID of the item to get.
         *
         * @returns A reference to the item.
         */
        T &get(size_t id)
        {
            if (!has(id))
            {
                throw std::runtime_error("SparseSet (get): ID '" + std::to_string(id) + "' does not exist in sparse set, for type '" + typeid(T).name() + "'.");
            }

            return dense[sparse[id]];
        }

        /**
         * Returns whether or not the sparse set has an item with the given ID.
         *
         * @param id The ID of the item to check for.
         *
         * @returns Whether or not the sparse set has an item with the given ID.
         */
        bool has(size_t id)
        {
            if (id > maxId)
            {
                throw std::runtime_error("SparseSet (has): ID is greater than max ID.");
            }

            return sparse[id] != NULL_INDEX;
        }

        /**
         * Returns the number of items in the set.
         *
         * This will be the same as the size of the dense vector.
         *
         * @returns The number of items in the set.
         */
        size_t size()
        {
            return dense.size();
        }

        /**
         * Returns the dense vector.
         *
         * @returns The dense vector.
         */
        const std::vector<T> &getDense()
        {
            return dense;
        }

        /**
         * Returns the dense ids vector.
         *
         * This is all the ids that have been added to the sparse set.
         *
         * @returns The dense ids vector.
         */
        const std::vector<size_t> &getDenseIds()
        {
            return denseIds;
        }

    private:
        /**
         * A parrallel vector to the dense vector that stores the ID of the item at the matching index in the dense vector.
         */
        std::vector<size_t> denseIds;

        /**
         * The dense vector.
         */
        std::vector<T> dense;

        /**
         * The sparse vector.
         */
        std::vector<size_t> sparse;
    };
}