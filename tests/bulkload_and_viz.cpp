#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <queue>
#include "BPTree.h"
using namespace std;

int test() {

    // Instantiate the BPTree
    BPTree* bPTree = new BPTree(1);

    // Test data
    vector<vector<float>> testData = {
        {0, 38}, {0, 38}, {0, 41}, {0, 35}, {0, 3}, {0, 4},
        {0, 9}, {0, 6}, {0, 11}, {0, 10}, {0, 13},
        {0, 12}, {0, 20}, {0, 22}, {0, 31}, {0, 23},
        {0, 36}, {0, 44}, {0, 50}, {0, 70}
    };

    // Bulk load the data
    bPTree->bulkLoad(testData);

    // Display the tree
    // cout << "B+ Tree Structure (Level Order):" << endl;
    // bPTree->levelOrderDisplay(bPTree->getRoot());

    // cout << "\nSequential Display of Leaf Nodes:" << endl;
    // bPTree->seqDisplay(bPTree->getRoot());

    // Visualize the B+ tree after bulkloading
    // Generate the DOT file
    string folder = "output";
    bPTree->writeDOT(folder + "/bptree.dot");
    // Run the Graphviz dot command to generate the PNG image, ie "dot -Tpng output/bptree.dot -o output/bptree.png"
    std::string command = "dot -Tpng " + folder + "/bptree.dot -o " + folder + "/bptree.png";
    int result = system(command.c_str());

    if (result == 0) {
        std::cout << "Graph image successfully generated: " << folder + "/bptree.png" << std::endl;
    }
    else {
        std::cerr << "Error: Failed to generate graph image." << std::endl;
    }

    cout << endl;
    cout << endl;
    cout << endl;
    cout << endl;

    // SEARCH TEST
    bPTree->searchRange(35.0, 41.0);


    delete bPTree;
    return 0;
}