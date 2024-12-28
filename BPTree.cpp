/**
 * @file BPTree.cpp
 * @brief Implementation of B+ Tree data structure and its associated methods
 *
 * This file contains the implementation of the B+ Tree, including methods for
 * constructing the tree, bulk loading data, and retrieving tree properties.
 */

#include <iostream>
#include "BPTree.h"
#include "macros.h"

 /**
  * @brief Default constructor for Node
  *
  * Initializes a new Node as a non-leaf node.
  */
Node::Node() {
    this->isLeaf = false;
    this->ptr2next = NULL;
}

/**
 * @brief Parameterized constructor for Node
 *
 * @param isLeaf Boolean indicating if the node is a leaf
 * Initializes the Node with the specified leaf status.
 */
Node::Node(bool isLeaf) {
    this->isLeaf = isLeaf;
    this->ptr2next = NULL;
}

/**
 * @brief Default constructor for BPTree
 *
 * Initializes a new B+ Tree with an order of 1 and no root.
 */
BPTree::BPTree() {
    this->order = 1;
    this->root = NULL;
}

/**
 * @brief Parameterized constructor for BPTree
 *
 * @param order The order of the B+ Tree
 * Initializes the B+ Tree with the specified order and no root.
 */
BPTree::BPTree(int order) {
    this->order = order;
    this->root = NULL;
}

/**
 * @brief Retrieves the root of the B+ Tree
 *
 * @return Pointer to the root node of the tree
 */
Node* BPTree::getRoot() {
    return this->root;
}

/**
 * @brief Retrieves the order of the B+ Tree
 *
 * @return The order of the B+ Tree
 */
int BPTree::getOrder() {
    return this->order;
}

/**
 * @brief Sets the root of the B+ Tree
 *
 * @param ptr Pointer to the new root node
 */
void BPTree::setRoot(Node* ptr) {
    this->root = ptr;
}

/**
 * @brief Finds the first leftmost leaf node in the subtree
 *
 * @param cursor Pointer to the current node
 * @return Pointer to the leftmost leaf node
 */
Node* BPTree::firstLeftNode(Node* cursor)
{
    if (cursor->isLeaf)
        return cursor;
    for (int i = 0; i < cursor->childNodes.size(); i++)
        if (cursor->childNodes[i] != NULL)
            return firstLeftNode(cursor->childNodes[i]);

    return NULL;
}

/**
 * @brief Bulk loads data into the B+ Tree
 *
 * @param data 2D vector containing the data to be loaded
 * @param fillFactor Float representing the desired fill factor
 *
 * This method sorts the input data and constructs the B+ Tree following a
 * bottom-up approach.
 */
void BPTree::bulkLoad(const vector<vector<float>>& data, const float& fillFactor)
{
    if (data.empty())
        return;

    // Clear existing tree if any
    this->root = NULL;
    // Parameters
    const int maxKeysPerLeaf = 2 * order;
    const int desiredKeysPerLeaf = static_cast<int>(maxKeysPerLeaf * fillFactor);

    // Step 1: Sort data based on the key
    vector<pair<float, int>> sortedData; // (key value, idx in data)
    for (int i = 0; i < data.size(); i++) {
        sortedData.push_back({ data[i][NODE_CONTINUOUS_INDEX], i });
    }
    sort(sortedData.begin(), sortedData.end());

    // Step 2: Build leaf nodes
    vector<Node*> currentLevel;
    vector<float> parentKeys;
    for (int i = 0; i < sortedData.size();)
    {
        Node* leaf = new Node();
        leaf->isLeaf = true;
        // Fill leaf node up to desired fill factor
        for (int j = 0; j < desiredKeysPerLeaf && i < sortedData.size(); j++, i++) {
            leaf->keys.push_back(sortedData[i].first);
            leaf->dataIds.push_back(sortedData[i].second);
        }
        // Link to previous leaf
        if (!currentLevel.empty()) {
            currentLevel.back()->ptr2next = leaf;
        }
        currentLevel.push_back(leaf);
        // The first key of next leaf becomes parent key (except for last leaf)
        if (i < sortedData.size()) {
            parentKeys.push_back(sortedData[i].first);
        }
    }

    // Step 3: Build internal nodes bottom-up
    int levelCount = 1;
    while (currentLevel.size() > 1)
    {
        vector<Node*> nextLevel;
        vector<float> newParentKeys;

        // LOGGING
        // // Uncomment to log the construction process level by level to the console.
        // cout << "   * Level " << levelCount++ << endl;
        // cout << "Previous level nodes: ";
        // for (const Node *node : currentLevel)
        // {
        //     cout << "[ ";
        //     for (float key : node->keys)
        //     {
        //         cout << key << " ";
        //     }
        //     cout << "] ";
        // }
        // cout << "\nParent keys for this level: ";
        // for (float key : parentKeys)
        // {
        //     cout << key << " ";
        // }
        // cout << endl;
        // LOGGING

        for (int i = 0; i < currentLevel.size();)
        {
            Node* internal = new Node();
            internal->isLeaf = false;
            // Add first child pointer
            internal->childNodes.push_back(currentLevel[i]);
            i++;

            // Fill internal node
            for (int j = 0; j < 2 * order && i < currentLevel.size(); j++)
            {

                if (j == 2 * order - 1 && i < currentLevel.size() - 1)
                { // Skip adding this key at this level since it will need to be pushed up
                    break;
                }
                internal->keys.push_back(parentKeys[i - 1]);
                if (i < currentLevel.size())
                {
                    internal->childNodes.push_back(currentLevel[i]);
                }
                i++;
            }
            nextLevel.push_back(internal);
            // For next level parent keys, use the keys that should be pushed up
            if (i < currentLevel.size() - 1)
            {
                newParentKeys.push_back(parentKeys[i - 1]);
            }
        }
        currentLevel = nextLevel;
        parentKeys = newParentKeys;
    }

    this->root = currentLevel[0];
}