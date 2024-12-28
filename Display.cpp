/**
 * @file Display.cpp
 * @brief Implementation of display-related functions for B+ Tree data structure
 * 
 * This file contains various visualization and display methods for the B+ Tree,
 * including level-order traversal display, sequential leaf node display, and
 * DOT file generation for GraphViz visualization.
 */

#include <iostream>
#include <queue>
#include "BPTree.h"

/**
 * @brief Displays the B+ tree in a level-order (breadth-first) traversal
 * 
 * @param cursor Pointer to the root node of the tree/subtree to display
 * 
 * Each level of the tree is displayed on a new line, with nodes represented
 * in brackets and their keys space-separated within the brackets.
 */
void BPTree::levelOrderDisplay(Node *cursor)
{
    if (cursor == NULL)
        return;
    queue<Node *> q;
    q.push(cursor);

    while (!q.empty())
    {
        int sz = q.size();
        for (int i = 0; i < sz; i++)
        { // To separate the levels of the b+ tree
            Node *u = q.front();
            q.pop();

            cout << "[ ";
            // printing keys 
            for (int val : u->keys)
                cout << val << " ";
            cout << "] "; 

            if (u->isLeaf == false)
            {
                for (Node *v : u->childNodes)
                {
                    q.push(v);
                }
            }
        }
        cout << endl; 
    }
}

/**
 * @brief Displays all leaf node values in sequential order
 * 
 * @param cursor Pointer to the root node of the tree
 * 
 * Traverses to the leftmost leaf node and then follows the leaf node
 * chain to display all values in sequential order. This represents the
 * actual sequence of data as it would be used in range queries.
 */
void BPTree::seqDisplay(Node *cursor)
{
    Node *firstLeft = firstLeftNode(cursor);

    if (firstLeft == NULL)
    {
        cout << "No Data in the Database yet!" << endl;
        return;
    }
    while (firstLeft != NULL)
    {
        for (int i = 0; i < firstLeft->keys.size(); i++)
        {
            cout << firstLeft->keys[i] << " ";
        }

        firstLeft = firstLeft->ptr2next;
    }
    cout << endl;
}

/**
 * @brief Generates DOT format representation for a node and its subtree
 * 
 * @param cursor Pointer to the current node being processed
 * @param out Output file stream for writing DOT content
 * @param nodeCounter Reference to counter for generating unique node IDs
 * 
 * Recursively generates DOT language representation of the B+ tree structure.
 * Internal nodes show ports for child connections, while leaf nodes display
 * only their values.
 */
void BPTree::generateDOT(Node *cursor, std::ofstream &out, int &nodeCounter)
{
    if (cursor == NULL)
        return;

    int currentNodeID = nodeCounter++; // Unique ID for each node

    if (!cursor->isLeaf)
    {
        // Internal node: format like "<f0> |key1|<f1> |key2|<f2>"
        std::string label = "";

        // Add all keys with their surrounding fields
        for (size_t i = 0; i < cursor->keys.size(); i++)
        {
            label += "<f" + std::to_string(i) + "> |" + std::to_string(cursor->keys[i]) + "|";
        }
        // Add the last field
        label += "<f" + std::to_string(cursor->keys.size()) + ">";

        out << "node" << currentNodeID << "[label = \"" << label << "\"];\n";
    }
    else
    {
        // Leaf node: just show the values without field ports
        std::string label = "";
        for (size_t i = 0; i < cursor->keys.size(); i++)
        {
            if (i > 0)
                label += "|";
            label += std::to_string(cursor->keys[i]);
        }

        out << "node" << currentNodeID << "[label = \"" << label << "\"];\n";
    }

    // If not a leaf node, write edges to child nodes
    if (!cursor->isLeaf)
    {
        for (size_t i = 0; i < cursor->childNodes.size(); i++)
        {
            Node *child = cursor->childNodes[i];
            int childNodeID = nodeCounter;        // ID of the child node
            generateDOT(child, out, nodeCounter); // Recursive call for the child

            // Create edges from fields to child nodes
            out << "\"node" << currentNodeID << "\":f" + std::to_string(i) + " -> \"node" << childNodeID << "\"\n";
        }
    }
}

/**
 * @brief Writes the complete B+ tree structure to a DOT file
 * 
 * @param filename Name of the output DOT file
 * 
 * Creates a DOT file that can be used with GraphViz to visualize the
 * complete B+ tree structure. The generated file includes node definitions
 * and edges representing the relationships between nodes.
 * 
 * @note The output file will be processed with GraphViz to generate visual
 * representations of the tree structure.
 */
void BPTree::writeDOT(const std::string &filename)
{
    std::ofstream out(filename);
    if (!out.is_open())
    {
        std::cerr << "Error: Unable to open file " << filename << " for writing.\n";
        return;
    }

    int nodeCounter = 0; // Unique counter for node IDs
    out << "digraph BPlusTree {\n";
    out << "node [shape=record];\n";

    generateDOT(root, out, nodeCounter);

    out << "}\n";
    out.close();
    std::cout << "DOT file successfully written to " << filename << "\n";
}