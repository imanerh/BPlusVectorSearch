/**
 * @file Search.cpp
 * @brief Implementation of search-related functions for B+ Tree data structure
 *
 * This file contains various search methods for the B+ Tree, including
 * searching for a specific key, finding the lower bound of a key, and
 * searching for a range of keys.
 */

#include <iostream>
#include <algorithm>
#include <string>
#include "BPTree.h"

 /**
  * @brief Traverses the B+ tree to find the leaf node containing the specified key
  *
  * @param key The key to search for in the B+ tree
  * @return A pair containing a pointer to the leaf node and the lower bound index of the key
  *
  * This function performs a traversal from the root to the appropriate leaf node
  * using binary search on the internal and leaf nodes. It returns the leaf node
  * and the lower bound index of the key.
  */
pair<Node*, int> BPTree::traverseToLeaf(float key) {
    if (root == NULL) {
        cout << "NO Tuples Inserted yet" << endl;
        return { nullptr, -1 };
    }

    Node* cursor = root;
    while (cursor->isLeaf == false) {
        // Perform binary search on the internal node's keys to find which path to follow
        int idx = std::upper_bound(cursor->keys.begin(), cursor->keys.end(), key) - cursor->keys.begin();
        // Follow the left pointer of the key
        cursor = cursor->childNodes[idx];
    }
    // Now we are at the leaf node, perform binary search on the keys of the leaf node
    int idx = std::lower_bound(cursor->keys.begin(), cursor->keys.end(), key) - cursor->keys.begin();
    return { cursor, idx };
}

/**
 * @brief Searches for a specific key in the B+ tree
 *
 * @param key The key to search for
 * @return true if the key is found, false otherwise
 *
 * This function utilizes the traverseToLeaf method to locate the leaf node and the lower bound index of the key
 * then checks if the lower bound is the key itself.
 */
bool BPTree::search(float key) {
    auto [cursor, idx] = traverseToLeaf(key);
    // If all elements are smaller than the key or the key is not found
    if (cursor == nullptr || idx == cursor->keys.size() || cursor->keys[idx] != key) {
        return false;
    }
    return true;
}

/**
 * @brief Searches for the lower bound of a specified key in the B+ tree
 *
 * @param key The key for which to find the lower bound
 * @return A pair containing a pointer to the node and the index of the lower bound
 *
 * This function provides both the cursor (the node) and the index within that node to indicate where to start
 * the search in range queries.
 * It employs the traverseToLeaf method to identify the leaf node and the corresponding lower bound index
 * for the specified key. It also handles scenarios where the lower bound falls outside the node,
 * allowing it to navigate to the next leaf node if available; otherwise, it returns nullptr.
 *
 * @note This function serves as a helper for the searchRange function
 */
pair<Node*, int> BPTree::searchLowerBound(float key) {
    auto [cursor, idx] = traverseToLeaf(key);

    if (cursor == nullptr) {
        return { nullptr, -1 };
    }

    if (idx == cursor->keys.size()) {
        if (cursor->ptr2next != nullptr) { // If not the last leaf node, return the next leaf node
            cursor = cursor->ptr2next;
            idx = 0;
        }
        else {
            cursor = nullptr;
            idx = -1;
        }
    }
    return { cursor, idx };
}

/**
 * @brief Searches for all keys within a specified range in the B+ tree
 *
 * @param l The lower bound of the range
 * @param r The upper bound of the range
 * @return A vector containing all data IDs within the specified range
 *
 * This function finds all keys within the specified range [l, r] and returns
 * a vector of corresponding data IDs. It traverses the leaf nodes sequentially until
 * it exceeds the upper bound.
 */
vector<int> BPTree::searchRange(float l, float r) {
    pair<Node*, int> s = searchLowerBound(l);
    Node* cursor = s.first;
    int idx = s.second;
    if (idx == -1) {
        cout << "NO data points in this range" << endl;
        return {};
    }

    vector<int> result;
    int flag = 0;
    // int numberOfMatches = 0;
    while (cursor != NULL)
    {
        for (int i = idx; i < cursor->keys.size(); i++)
        {
            if (cursor->keys[i] >= l && cursor->keys[i] <= r) {
                result.push_back(cursor->dataIds[i]);
                // Output   
                // numberOfMatches++;
                // cout << "(" << cursor->keys[i] << ", " << cursor->dataIds[i] << ")" << endl;
            }
            else {
                flag = 1;
                break;
            }
        }
        if (flag == 1) {
            break;
        }
        cursor = cursor->ptr2next;
        idx = 0; // reset for the next loop to start at idx 0
    }
    // cout << "> Number of matches: " << numberOfMatches << endl;

    return result;
}