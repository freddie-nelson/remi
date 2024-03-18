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
         * @param addToLeafSurfaceAreaThreshold The threshold for the surface area of the fat AABB of leaf nodes to determine whether or not to split the leaf node.
         */
        AABBTree(float margin = 0.0f, float addToLeafSurfaceAreaThreshold = 100.0f) : margin(margin), addToLeafSurfaceAreaThreshold(addToLeafSurfaceAreaThreshold)
        {
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
                root->aabbs[id] = &aabb;
                root->ids.push_back(id);
                root->fatAabb = AABB(aabb.getMin() - margin, aabb.getMax() + margin);

                leaves[id] = root;
                ids.emplace(id);

                return;
            }

            // find the best node to insert the AABB into.
            AABBTreeNode<T> *leaf = new AABBTreeNode<T>();
            leaf->aabbs[id] = &aabb;
            leaf->ids.push_back(id);
            leaf->fatAabb = AABB(aabb.getMin() - margin, aabb.getMax() + margin);

            leaves[id] = leaf;
            ids.emplace(id);

            AABBTreeNode<T> *node = root;
            unsigned int depth = 0;

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

                auto newLeftAabb = leaf->fatAabb.merge(left->fatAabb);
                if (left->isLeaf())
                {
                    costLeft = newLeftAabb.getSurfaceArea() + minimumPushDownCost;
                }
                else
                {
                    costLeft = (newLeftAabb.getSurfaceArea() - left->fatAabb.getSurfaceArea()) + minimumPushDownCost;
                }

                auto newRightAabb = leaf->fatAabb.merge(right->fatAabb);
                if (right->isLeaf())
                {
                    costRight = newRightAabb.getSurfaceArea() + minimumPushDownCost;
                }
                else
                {
                    costRight = (newRightAabb.getSurfaceArea() - right->fatAabb.getSurfaceArea()) + minimumPushDownCost;
                }

                // if the cost of creating a new parent node here is less than descending in either direction then
                // we know we need to create a new parent node here and attach the leaf to that.
                if (newParentNodeCost < costLeft && newParentNodeCost < costRight)
                {
                    // std::cout << "Creating new parent node" << std::endl;
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

                depth++;
            }

            // the leafs sibling is going to be the node we found above and we are going to create a new
            // parent node and attach the leaf and this item
            AABBTreeNode<T> *sibling = node;

            AABBTreeNode<T> *oldParent = sibling->parent;
            AABBTreeNode<T> *newParent = new AABBTreeNode<T>();

            newParent->parent = oldParent;
            newParent->fatAabb = sibling->fatAabb.merge(leaf->fatAabb);
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

            node->aabbs.erase(id);

            for (auto it = node->ids.begin(); it != node->ids.end(); it++)
            {
                if (*it == id)
                {
                    node->ids.erase(it);
                    break;
                }
            }

            // if node isn't empty yet then we don't need to delete
            // so just recalculate fat aabb and return
            if (!node->aabbs.empty())
            {
                node->fatAabb = calculateFatAABB(node->aabbs);
                return;
            }

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

            node->aabbs[id] = &aabb;

            auto newFatAabb = node->fatAabb.merge(AABB(aabb.getMin() - margin, aabb.getMax() + margin));
            if (node->fatAabb.contains(newFatAabb))
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

            return leaves[id]->aabbs[id];
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
                    for (auto &[id, aabb] : node->aabbs)
                    {
                        leaves.erase(id);
                    }
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
         * Gets the number of leaves in the tree.
         *
         * @warning This is an expensive operation.
         *
         * @returns The number of leaves in the tree.
         */
        size_t leafCount() const
        {
            if (root == nullptr)
            {
                return 0;
            }

            std::stack<AABBTreeNode<T> *> nodes;
            nodes.push(root);

            size_t count = 0;

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
                    count++;
                }
            }

            return count;
        }

        /**
         * Gets the number of nodes in the tree.
         *
         * @warning This is an expensive operation.
         *
         * @returns The number of nodes in the tree.
         */
        size_t size() const
        {
            if (root == nullptr)
            {
                return 0;
            }

            std::stack<AABBTreeNode<T> *> nodes;
            nodes.push(root);

            size_t count = 0;

            while (!nodes.empty())
            {
                auto node = nodes.top();
                nodes.pop();

                count++;

                if (node->left != nullptr)
                {
                    nodes.push(node->left);
                }

                if (node->right != nullptr)
                {
                    nodes.push(node->right);
                }
            }

            return count;
        }

        /**
         * Gets the number of nodes in the tree that are left children of their parent.
         *
         * @warning This is an expensive operation.
         *
         * @returns The number of nodes in the tree that are left children of their parent.
         */
        size_t leftNodeCount()
        {
            if (root == nullptr)
            {
                return 0;
            }

            std::stack<AABBTreeNode<T> *> nodes;
            nodes.push(root);

            size_t count = 0;

            while (!nodes.empty())
            {
                auto node = nodes.top();
                nodes.pop();

                if (node->left != nullptr)
                {
                    nodes.push(node->left);
                    count++;
                }

                if (node->right != nullptr)
                {
                    nodes.push(node->right);
                }
            }

            return count;
        }

        /**
         * Gets the number of nodes in the tree that are right children of their parent.
         *
         * @warning This is an expensive operation.
         *
         * @returns The number of nodes in the tree that are right children of their parent.
         */
        size_t rightNodeCount()
        {
            return size() - leftNodeCount();
        }

        /**
         * Gets the height of the tree.
         *
         * Recursively calculates the height of the tree.
         *
         * @warning This is an expensive operation.
         *
         * @returns The height of the tree.
         */
        size_t height() const
        {
            if (root == nullptr)
            {
                return 0;
            }

            return root->height();
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
         * @param fastQuery Setting to true will skip querying individual aabbs in a leaf node and instead just return all of the contained aabbs in the leaf.
         * @param includeInQuery A function to determine whether or not to include an AABB in the query.
         *
         * @returns The number of nodes visited during the query.
         */
        size_t query(const AABB &aabb, std::vector<T> &overlapping, bool fastQuery = true, std::function<bool(T)> includeInQuery = nullptr) const
        {
            if (root == nullptr)
            {
                return 0;
            }

            std::stack<AABBTreeNode<T> *> nodes;
            nodes.push(root);

            size_t visited = 0;

            while (!nodes.empty())
            {
                auto node = nodes.top();
                nodes.pop();

                visited++;

                if (node->isLeaf())
                {
                    if (fastQuery)
                    {
                        for (auto &id : node->ids)
                        {
                            if (includeInQuery != nullptr && !includeInQuery(id))
                            {
                                continue;
                            }

                            overlapping.push_back(id);
                        }
                    }
                    else
                    {
                        for (auto &[id, nodeAabb] : node->aabbs)
                        {
                            if (includeInQuery != nullptr && !includeInQuery(id))
                            {
                                continue;
                            }

                            if (nodeAabb->overlaps(aabb))
                            {
                                overlapping.push_back(id);
                            }
                        }
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

            return visited;
        }

        /**
         * Gets the number of AABBs in the tree.
         *
         * @note This is an O(1) operation.
         *
         * @returns The number of AABBs in the tree.
         */
        size_t aabbCount()
        {
            return ids.size();
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

        /**
         * Outputs the tree as a string.
         *
         * This string can be used to visualize the tree using graphviz.
         *
         * The string is a valid dot file.
         *
         * @returns The tree as a string.
         */
        std::string toDebugString() const
        {
            if (root == nullptr)
            {
                return "";
            }

            // generate node indices
            std::unordered_map<AABBTreeNode<T> *, int> nodeIndices;

            std::string nodeString;
            std::string edgeString;

            std::stack<AABBTreeNode<T> *> nodes;
            nodes.push(root);

            int nodeIndex = 0;

            while (!nodes.empty())
            {
                auto node = nodes.top();
                nodes.pop();

                nodeIndices[node] = nodeIndex++;

                if (node->right != nullptr)
                {
                    nodes.push(node->right);
                }

                if (node->left != nullptr)
                {
                    nodes.push(node->left);
                }
            }

            // generate node string
            for (auto &[node, index] : nodeIndices)
            {
                nodeString += std::to_string(index) + (node->isLeaf() ? "[tooltip=\"" + std::to_string(node->aabbs.size()) + "\"]" : "") + ";\n";
            }

            // generate edge string
            for (auto &[node, index] : nodeIndices)
            {
                if (node->left != nullptr)
                {
                    edgeString += std::to_string(index) + " -- " + std::to_string(nodeIndices[node->left]) + ";\n";
                }

                if (node->right != nullptr)
                {
                    edgeString += std::to_string(index) + " -- " + std::to_string(nodeIndices[node->right]) + ";\n";
                }
            }

            return "graph aabbtree {\n" + nodeString + edgeString + "}";
        }

        /**
         * Merges the leaves of the tree until the height of the tree is minimized as much as possible.
         *
         * @returns Whether or not any leaves were merged.
         */
        bool mergeLeavesTillMin()
        {
            bool merged = false;

            while (mergeLeaves())
            {
                merged = true;
            }

            return merged;
        }

        /**
         * Merges the leaves of the tree.
         *
         * This will perform one pass of merging leaves together to reduce the height of the tree.
         *
         * This will attempt to merge small leaves together to reduce the height of the tree.
         *
         * @returns Whether or not any leaves were merged.
         */
        bool mergeLeaves()
        {
            std::unordered_set<AABBTreeNode<T> *> checkedLeaves;
            std::vector<AABBTreeNode<T> *> toDelete;

            for (auto &[id, leaf] : leaves)
            {
                if (checkedLeaves.contains(leaf))
                {
                    continue;
                }

                checkedLeaves.insert(leaf);

                auto parent = leaf->parent;
                if (parent == nullptr)
                {
                    continue;
                }

                auto left = parent->left;
                auto right = parent->right;

                if (left->isLeaf() && right->isLeaf() && parent->fatAabb.getSurfaceArea() < addToLeafSurfaceAreaThreshold)
                {
                    // merge the two leafs into one
                    for (auto &[id, aabb] : left->aabbs)
                    {
                        parent->aabbs[id] = aabb;
                        parent->ids.push_back(id);
                        leaves[id] = parent;
                    }

                    for (auto &[id, aabb] : right->aabbs)
                    {
                        parent->aabbs[id] = aabb;
                        parent->ids.push_back(id);
                        leaves[id] = parent;
                    }

                    // remove the leaves
                    parent->left = nullptr;
                    parent->right = nullptr;

                    toDelete.push_back(left);
                    toDelete.push_back(right);
                }
            }

            for (auto node : toDelete)
            {
                delete node;
            }

            return !toDelete.empty();
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
         * The threshold for the surface area of the fat AABB of leaf nodes.
         *
         * If the surface area of the fat AABB of a leaf node is less than this threshold then the leaf node is not split.
         */
        float addToLeafSurfaceAreaThreshold = 100.0f;

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
                AABBTreeNode<T> *left = currentNode->left;
                AABBTreeNode<T> *right = currentNode->right;

                currentNode->fatAabb = left->fatAabb.merge(right->fatAabb);
                currentNode = currentNode->parent;
            }
        }

        AABB calculateFatAABB(const std::unordered_map<T, const AABB *> &aabbs)
        {
            AABB fatAabb;

            for (auto &[id, aabb] : aabbs)
            {
                fatAabb = fatAabb.merge(AABB(aabb->getMin() - margin, aabb->getMax() + margin));
            }

            return fatAabb;
        }
    };
}