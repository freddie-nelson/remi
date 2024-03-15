#pragma once

#include "AABB.h"
#include <stdexcept>
#include <unordered_map>

namespace Core
{
    /**
     * Represents a node in an AABB tree.
     */
    template <typename T>
    struct AABBTreeNode
    {
        AABBTreeNode *parent = nullptr;
        AABBTreeNode *left = nullptr;
        AABBTreeNode *right = nullptr;

        AABB fatAabb;

        std::unordered_map<T, const AABB *> aabbs;

        /**
         * Gets whether or not this node is the root of the tree.
         *
         * @returns Whether or not this node is the root of the tree.
         */
        bool isRoot()
        {
            return parent == nullptr;
        }

        /**
         * Gets whether or not this node is a leaf node.
         *
         * @returns Whether or not this node is a leaf node.
         */
        bool isLeaf()
        {
            return left == nullptr && right == nullptr;
        }

        /**
         * Gets whether or not this node is the left child of its parent.
         *
         * @returns Whether or not this node is the left child of its parent.
         */
        bool isLeftChild()
        {
            return !isRoot() && parent->left == this;
        }

        /**
         * Gets whether or not this node is the right child of its parent.
         *
         * @returns Whether or not this node is the right child of its parent.
         */
        bool isRightChild()
        {
            return !isRoot() && parent->right == this;
        }

        /**
         * Gets the sibling of this node.
         *
         * The sibling may be null.
         *
         * @returns The sibling of this node.
         */
        AABBTreeNode<T> *getSibling()
        {
            if (isRoot())
            {
                return nullptr;
            }

            return isLeftChild() ? parent->right : parent->left;
        }

        /**
         * Gets whether or not this node has a sibling.
         *
         * @returns Whether or not this node has a sibling.
         */
        bool hasSibling()
        {
            return getSibling() != nullptr;
        }

        /**
         * Gets the height of the node.
         *
         * @returns The height of the node.
         */
        size_t height()
        {
            if (isLeaf())
            {
                return 0;
            }

            return 1 + std::max(left != nullptr ? left->height() : 0, right != nullptr ? right->height() : 0);
        }
    };
}