#pragma once

#include "./AABB.h"
#include "./AABBTreeNode.h"

#include <unordered_map>
#include <unordered_set>
#include <stdexcept>
#include <stack>

namespace Core
{
    /**
     * Represents an AABB tree.
     *
     * This implementation is based on the AABB Tree implemented [here](https://www.azurefromthetrenches.com/introductory-guide-to-aabb-tree-collision-detection/)
     *
     * @tparam T The type of the ID of the AABBs.
     */
    template <typename T>
    class AABBTree
    {
    public:
        /**
         * Creates an empty tree.
         *
         * @param margin The margin to add to each fat AABB.
         */
        AABBTree(float margin = 0.0f)
        {
            this->margin = margin;
            this->root = nullptr;
        }

        /**
         * Destroys the tree.
         */
        ~AABBTree()
        {
            clear();
        }

        /**
         * Inserts an AABB into the tree.
         *
         * @param id The ID of the AABB.
         * @param aabb The AABB to insert.
         *
         * @throws std::invalid_argument If the AABB is already in the tree.
         */
        void insert(T id, const AABB &aabb)
        {
            if (has(id))
            {
                throw std::invalid_argument("AABBTree (insert): AABB is already in the tree.");
            }

            // if the tree is empty then create the root node.
            if (root == nullptr)
            {
                root = new AABBTreeNode<T>();
                root->id = id;
                root->aabb = &aabb;
                root->fatAabb = AABB(aabb.getMin() - margin, aabb.getMax() + margin);

                leaves[id] = root;
                ids.emplace(id);

                return;
            }

            // find the best node to insert the AABB into.
            AABBTreeNode<T> *leaf = new AABBTreeNode<T>();
            leaf->id = id;
            leaf->aabb = &aabb;
            leaf->fatAabb = AABB(aabb.getMin() - margin, aabb.getMax() + margin);

            leaves[id] = leaf;
            ids.emplace(id);

            AABBTreeNode<T> *node = root;

            while (!node->isLeaf())
            {
                // okay since we are not leaf, we have to be a branch
                AABBTreeNode<T> *left = node->left;
                AABBTreeNode<T> *right = node->right;

                // create the combined leaf and current node AABB
                AABB combinedAabb = node->fatAabb.merge(leaf->fatAabb);

                float newParentNodeCost = 2.0f * combinedAabb.getSurfaceArea();
                float minimumPushDownCost = 2.0f * (combinedAabb.getSurfaceArea() - node->fatAabb.getSurfaceArea());

                // use the costs to figure out whether to create a new parent here or descend
                float costLeft;
                float costRight;

                if (left->isLeaf())
                {
                    costLeft = leaf->fatAabb.merge(left->fatAabb).getSurfaceArea() + minimumPushDownCost;
                }
                else
                {
                    AABB newLeftAabb = leaf->fatAabb.merge(left->fatAabb);
                    costLeft = (newLeftAabb.getSurfaceArea() - left->fatAabb.getSurfaceArea()) + minimumPushDownCost;
                }

                if (right->isLeaf())
                {
                    costRight = leaf->fatAabb.merge(right->fatAabb).getSurfaceArea() + minimumPushDownCost;
                }
                else
                {
                    AABB newRightAabb = leaf->fatAabb.merge(right->fatAabb);
                    costRight = (newRightAabb.getSurfaceArea() - right->fatAabb.getSurfaceArea()) + minimumPushDownCost;
                }

                // if the cost of creating a new parent node here is less than descending in either direction then
                // we know we need to create a new parent node here and attach the leaf to that.
                if (newParentNodeCost < costLeft && newParentNodeCost < costRight)
                {
                    break;
                }

                // otherwise descend in the cheapest direction
                if (costLeft < costRight)
                {
                    node = left;
                }
                else
                {
                    node = right;
                }
            }

            // the leafs sibling is going to be the node we found above and we are going to create a new
            // parent node and attach the leaf and this item
            AABBTreeNode<T> *sibling = node;

            AABBTreeNode<T> *oldParent = sibling->parent;
            AABBTreeNode<T> *newParent = new AABBTreeNode<T>();

            newParent->parent = oldParent;
            newParent->fatAabb = leaf->fatAabb.merge(sibling->fatAabb);
            newParent->left = sibling;
            newParent->right = leaf;

            leaf->parent = newParent;
            sibling->parent = newParent;

            // sibling was the root so make new parent now the root
            if (oldParent == nullptr)
            {
                // we are at the root so update that and return
                // and can just return since root so no need to fix upwards
                root = newParent;
                return;
            }

            // the sibling was not the root and so we need to patch the left or right index to
            // point to the new node
            if (oldParent->left == sibling)
            {
                oldParent->left = newParent;
            }
            else
            {
                oldParent->right = newParent;
            }

            // finally we need to walk back up the tree fixing heights and areas
            fixUpwardsTree(newParent);
        }

        /**
         * Removes an AABB from the tree.
         *
         * If the AABB is not in the tree then this function does nothing.
         *
         * @param id The ID of the AABB to remove.
         */
        void remove(T id)
        {
            if (!has(id))
            {
                return;
            }

            AABBTreeNode<T> *node = leaves[id];
            leaves.erase(id);
            ids.erase(id);

            // if the node is the root then we can just clear the root and return
            if (node == root)
            {
                delete node;

                root = nullptr;
                return;
            }

            AABBTreeNode<T> *parent = node->parent;

            // if the parent is the root then we can move the sibling up to be the new root
            if (parent->isRoot())
            {
                root = node->getSibling();
                root->parent = nullptr;

                delete node;
                delete parent;

                return;
            }

            AABBTreeNode<T> *sibling = node->getSibling();
            AABBTreeNode<T> *grandParent = parent->parent;

            if (parent->isLeftChild())
            {
                grandParent->left = sibling;
            }
            else
            {
                grandParent->right = sibling;
            }

            sibling->parent = grandParent;

            delete node;
            delete parent;

            // finally we need to walk back up the tree fixing heights and areas
            fixUpwardsTree(grandParent);
        }

        /**
         * Updates an AABB in the tree.
         *
         * @param id The ID of the AABB to update.
         * @param aabb The new AABB.
         *
         * @returns Whether or not the AABB was updated.
         *
         * @throws std::invalid_argument If the AABB is not in the tree.
         */
        bool update(T id, const AABB &aabb)
        {
            if (!has(id))
            {
                throw std::invalid_argument("AABBTree (update): AABB is not in the tree.");
            }

            AABBTreeNode<T> *node = leaves[id];

            node->aabb = &aabb;

            if (node->fatAabb.contains(*node->aabb))
            {
                return false;
            }

            remove(id);
            insert(id, aabb);

            return true;
        }

        /**
         * Gets the AABB for the given ID.
         *
         * @param id The ID of the AABB to get.
         *
         * @returns The AABB for the given ID.
         *
         * @throws std::invalid_argument If the AABB is not in the tree.
         */
        const AABB &get(T id) const
        {
            if (!has(id))
            {
                throw std::invalid_argument("AABBTree (get): AABB is not in the tree.");
            }

            return leaves[id]->aabb;
        }

        /**
         * Checks if an AABB is in the tree.
         *
         * @param id The ID of the AABB to check for.
         *
         * @returns Whether or not the AABB is in the tree.
         */
        bool has(T id) const
        {
            return ids.contains(id);
        }

        /**
         * Clears the tree.
         */
        void clear()
        {
            if (root == nullptr)
            {
                return;
            }

            std::stack<AABBTreeNode<T> *> nodes;
            nodes.push(root);

            while (!nodes.empty())
            {
                auto node = nodes.top();
                nodes.pop();

                if (node->isLeaf())
                {
                    leaves.erase(node->id);
                }
                else
                {
                    // add children to the stack
                    if (node->left != nullptr)
                    {
                        nodes.push(node->left);
                    }

                    if (node->right != nullptr)
                    {
                        nodes.push(node->right);
                    }
                }

                delete node;
            }

            root = nullptr;

            leaves.clear();
            ids.clear();
        }

        /**
         * Gets the number of AABBs/leaves in the tree.
         *
         * @returns The number of AABBs/leaves in the tree.
         */
        size_t size()
        {
            return leaves.size();
        }

        /**
         * Checks if the tree is empty.
         *
         * @returns Whether or not the tree is empty.
         */
        bool isEmpty()
        {
            return root == nullptr;
        }

        const std::unordered_set<T> &getIds() const
        {
            return ids;
        }

        /**
         * Queries the tree for AABBs that overlap the given AABB.
         *
         * @param aabb The AABB to query for.
         * @param overlapping The vector to store the IDs of the overlapping AABBs in.
         */
        void query(const AABB &aabb, std::vector<T> &overlapping) const
        {
            if (root == nullptr)
            {
                return;
            }

            std::stack<AABBTreeNode<T> *> nodes;
            nodes.push(root);

            while (!nodes.empty())
            {
                auto node = nodes.top();
                nodes.pop();

                if (node->isLeaf())
                {
                    if (node->aabb->overlaps(aabb))
                    {
                        overlapping.push_back(node->id);
                    }
                }

                // check if the node overlaps the AABB
                // if it does then we need to add both children to the stack
                else if (node->fatAabb.overlaps(aabb))
                {
                    // add children to the stack
                    if (node->left != nullptr)
                    {
                        nodes.push(node->left);
                    }

                    if (node->right != nullptr)
                    {
                        nodes.push(node->right);
                    }
                }
            }
        }

        /**
         * Gets the root of the tree.
         *
         * @returns the root of the tree.
         */
        AABBTreeNode<T> *getRoot() const
        {
            return root;
        }

    private:
        /**
         * The root node of the tree.
         */
        AABBTreeNode<T> *root;

        /**
         * The margin to add to each fat AABB.
         */
        float margin;

        /**
         * The leaves of the tree.
         */
        std::unordered_map<T, AABBTreeNode<T> *> leaves;

        /**
         * The IDs of the AABBs in the tree.
         */
        std::unordered_set<T> ids;

        /**
         * Fixes the tree upwards from the given node.
         *
         * @param node The node to fix upwards from.
         */
        void fixUpwardsTree(AABBTreeNode<T> *node)
        {
            AABBTreeNode<T> *currentNode = node;

            while (currentNode != nullptr)
            {
                // check for leaf may be unnecessary but leaving it in for now
                if (!currentNode->isLeaf())
                {
                    AABBTreeNode<T> *left = currentNode->left;
                    AABBTreeNode<T> *right = currentNode->right;

                    currentNode->fatAabb = left->fatAabb.merge(right->fatAabb);
                }

                currentNode = currentNode->parent;
            }
        }
    };
}