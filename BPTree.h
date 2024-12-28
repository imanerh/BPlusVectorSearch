#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <cmath>
using namespace std;

#ifndef BPTREE_H
#define BPTREE_H

/**
 * @brief A node class representing elements in a B+ tree data structure
 * 
 * This class implements both leaf and internal nodes of a B+ tree. Leaf nodes
 * store keys and are linked together for sequential access, while internal nodes
 * store keys and pointers to child nodes for tree traversal.
 */
class Node
{
private:
    bool isLeaf;              // Indicates whether this node is a leaf node
    vector<float> keys;       // Stores the keys in sorted order
    Node *ptr2next;           // Pointer to connect next node for leaf nodes
    vector<Node *> childNodes; // Array of pointers to children sub-trees for internal nodes
    vector<int> dataIds;       // Array of the data Id corresponding to the key
    friend class BPTree;      // Allows BPTree class to access private members

public:
    Node();
    Node(bool isLeaf);
};

class BPTree
{
private:
    int order;
    Node *root;
    Node *firstLeftNode(Node *cursor);

public:
    // Constructors
    BPTree();
    BPTree(int order);

    // Getters and Setters
    Node *getRoot();
    void setRoot(Node *);
    int getOrder();
    
    // Search
    pair<Node*, int> traverseToLeaf(float key);
    bool search(float key);
    pair<Node*, int> searchLowerBound(float key);
    vector<int> searchRange(float l, float r);
    
    // Bulkload
    void bulkLoad(const vector<vector<float>> &data, const float &fillFactor = 1);

    // Display & Visualization Functions
    void levelOrderDisplay(Node *cursor);
    void seqDisplay(Node *cursor);
    // Function declarations for DOT file generation
    void writeDOT(const std::string &filename);
    void generateDOT(Node *cursor, std::ofstream &out, int &nodeCounter);
};

#endif