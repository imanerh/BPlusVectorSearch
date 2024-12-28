#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <queue>

#include "helpers/io.h"
#include "BPTree.h"
using namespace std;

int test()
{

    string source_path = "data/contest-data-release-1m.bin";
    string query_path = "data/contest-queries-release-1m.bin";
    string knn_save_path = "output.bin";
    uint32_t num_data_dimensions = 102;

    // Read data points
    vector<vector<float>> nodes;
    ReadBin(source_path, num_data_dimensions, nodes);
    cout << endl;
    // Read queries
    uint32_t num_query_dimensions = num_data_dimensions + 2;
    vector<vector<float>> queries;
    ReadBin(query_path, num_query_dimensions, queries);
    cout << endl;

    // vector <vector<uint32_t>> knn_results; // for saving knn results

    uint32_t n = nodes.size();    // n data points
    uint32_t d = nodes[0].size(); // each of dimension d
    uint32_t nq = queries.size(); // nq queries

    cout << "# data points:  " << n << "\n";
    cout << "# data point dim:  " << d << "\n";
    cout << "# queries:      " << nq << "\n";

    // cout << endl;

    // cout << nodes[0][0] << endl;
    // cout << nodes[0][1] << endl;
    // cout << queries[0][0] << endl;
    // cout << queries[0][1] << endl;
    // cout << queries[0][2] << endl;
    // cout << queries[0][3] << endl;

    // cout << endl;
    // cout << nodes[0][34];




    // Initialize B+ Tree
    BPTree* bPTree = new BPTree(100);

    // Measure time for bulk loading
    auto start_time = chrono::high_resolution_clock::now();
    bPTree->bulkLoad(nodes);
    auto end_time = chrono::high_resolution_clock::now();

    // Calculate duration in milliseconds
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
    cout << "Bulk loading time: " << duration << " ms" << endl;

    // // Root
    // cout << "Root:" << endl;
    // cout << bPTree->getRoot()->keys.size() << endl;

    // // Other
    // cout << "Other:" << endl;
    // cout << bPTree->getRoot()->childNodes[0]->keys.size() << endl;




    // Measure time for queries
    auto start_time_queries = chrono::high_resolution_clock::now();

    int nbQueries = 0;
    int type2 = 0, type3 = 0;
    float l, r;

    for (auto q : queries) {
        if (q[0] == 2 || q[0] == 3) {
            l = q[2];
            r = q[3];
            // cout << "l = " << l << "; r = " << r << endl;
            vector<int> result = bPTree->searchRange(l, r);
            nbQueries++;
        }
    }

    
    auto end_time_queries = chrono::high_resolution_clock::now();
    // Calculate duration in milliseconds
    auto duration_queries = chrono::duration_cast<chrono::milliseconds>(end_time_queries - start_time_queries).count();
    cout << "Total Queries Duration: " << duration_queries << " ms" << endl;
    cout << "QPS: " << nbQueries * 1000 / duration_queries << endl;
    


    // cout << type2 << " " << type3 << endl;


    delete bPTree; // Clean up dynamically allocated memory
    return 0;
}