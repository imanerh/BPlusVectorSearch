#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <thread>
#include <numeric>
#include <queue>
#include <atomic>
#include <mutex>
#include <memory>
#include <iomanip> 
#include "helpers/io.h"
#include "BPTree.h"
#include "KNN/knn.h"
#include "hnswlib/hnswlib.h"
#include "macros.h"
using namespace std;

/**
 * @brief Decides which KNN method to use based on a given threshold
 *
 * @param hnsw A pointer to the HNSW graph
 * @param ids A vector of identifiers for the dataset points
 * @param dataset A 2D vector containing the dataset points
 * @param query The query point for which KNN is to be found
 * @param k The number of nearest neighbors to return
 * @param l The lower bound for filtering results
 * @param r The upper bound for filtering results
 * @param threshold The threshold for switching between methods
 *
 * This function determines whether to use brute force KNN or HNSW KNN
 * based on the specified threshold.
 *
 * @return A vector of identifiers for the K nearest neighbors satisfying the range [l, r]
 */
vector<uint32_t> computeKNN(hnswlib::HierarchicalNSW<float>* hnsw, const vector<int>& ids, const vector<vector<float>>& dataset, const vector<float>& query, int k, float l, float r, int threshold) {
    if (ids.size() <= threshold) {
        // cout << "=> BRUTEEFORCE" << endl;
        return bruteForceKNN(ids, dataset, query, k);
    }
    else {
        // cout << "=> HNSW" << endl;
        return hnswKNN(hnsw, dataset, query, k, l, r);
    }
}


int main(int argc, char* argv[]) {
    // Check if the correct number of arguments is provided
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <source_path> <query_path> <knn_save_path>\n";
        return 1;
    }
    // Extract paths from arguments
    std::string source_path = argv[1];
    std::string query_path = argv[2];
    std::string knn_save_path = argv[3];

    try {
        // 1. Reading the data
        vector<vector<float>> nodes;
        vector<vector<float>> queries;
        readData(source_path, query_path, nodes, queries);

        // 2. Initialize B+ Tree
        int order = 100;
        BPTree* bPTree = new BPTree(order);
        // Measure time for bulk loading
        auto start_time = chrono::high_resolution_clock::now();
        cout << "> Starting bulkloading... ";
        bPTree->bulkLoad(nodes);
        auto end_time = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();
        cout << "[ " << duration << " ms ]" << endl;


        // 3. Create the HNSW graph
        // hnswlib::HierarchicalNSW<float>* hnsw = createHNSW(nodes);
        cout << "> Building the HNSW Index... ";
        // Start timing
        auto start_time_hnsw  = std::chrono::high_resolution_clock::now();
        hnswlib::L2Space space(VEC_DIM);
        size_t num_points = nodes.size();
        auto* hnsw = new hnswlib::HierarchicalNSW<float>(&space, num_points, HNSW_M, HNSW_EF_CONSTRUCTION);
        // Multithreading
        // Source: https://github.com/nmslib/hnswlib/blob/master/examples/cpp/example_mt_search.cpp
        const unsigned int num_threads = thread::hardware_concurrency();
        float* data = new float[VEC_DIM * num_points];
        for (int i = 0; i < num_points; i++) {
            memcpy(data + i * VEC_DIM, nodes[i].data() + NODE_EXTRAS, VEC_DIM * sizeof(float)); // Fill data array
        }
        // Add data to index
        ParallelFor(0, num_points, num_threads, [&](size_t row, size_t threadId) {
            hnsw->addPoint((void*)(data + VEC_DIM * row), row);
            });
        // // Query the elements for themselves and measure recall
        // std::vector<hnswlib::labeltype> neighbors(num_points);
        // ParallelFor(0, num_points, num_threads, [&](size_t row, size_t threadId) {
        //     std::priority_queue<std::pair<float, hnswlib::labeltype>> result = hnsw->searchKnn(data + VEC_DIM * row, 1);
        //     hnswlib::labeltype label = result.top().second;
        //     neighbors[row] = label;
        //     });
        // float correct = 0;
        // for (int i = 0; i < num_points; i++) {
        //     hnswlib::labeltype label = neighbors[i];
        //     if (label == i) correct++;
        // }
        // float recall = correct / num_points;
        // std::cout << "\nHNSW Recall: " << recall << "\n";
        // End timing
        auto end_time_hnsw  = std::chrono::high_resolution_clock::now();
        auto elapsed_time_hnsw  = std::chrono::duration_cast<std::chrono::milliseconds>(end_time_hnsw  - start_time_hnsw ).count();
        cout << "[ " << elapsed_time_hnsw << " ms ]" << endl;


        // 4. Answering Queries
        // Measure time for queries
        auto start_time_queries = chrono::high_resolution_clock::now();

        int nbQueries = 0;
        float l, r;
        int threshold = 4000;
        vector<int> filtered_ids;
        vector<vector<uint32_t>> knn_results;
        vector<uint32_t> knn;
        float recall_query;
        float totalRecall = 0.0f;

        for (auto q : queries) {
            if (q[QUERY_TYPE_INDEX] == 2) {
                l = q[QUERY_L_INDEX];
                r = q[QUERY_R_INDEX];
                filtered_ids = bPTree->searchRange(l, r);
                // Compute KNN
                knn = computeKNN(hnsw, filtered_ids, nodes, q, K, l, r, threshold);

                // Compute recall
                // Compute ground truth using brute force KNN
                vector<uint32_t> ground_truth = bruteForceKNN(filtered_ids, nodes, q, K);
                int relevant_retrieved = 0;
                for (int id : knn) {
                    if (find(ground_truth.begin(), ground_truth.end(), id) != ground_truth.end()) {
                        relevant_retrieved++;
                    }
                }
                recall_query = (float)relevant_retrieved / ground_truth.size();
                totalRecall += recall_query;

                knn_results.push_back(std::move(knn));
                nbQueries++;
            }
        }

        auto end_time_queries = chrono::high_resolution_clock::now();
        auto duration_queries = chrono::duration_cast<chrono::milliseconds>(end_time_queries - start_time_queries).count();

        // Calculate average recall
        float avgRecall = totalRecall / nbQueries;

        cout << "\n> Stats:\n";
        // Print aligned output
        cout << left;  // Left-align all columns
        cout << setw(30) << "   Total Queries Duration:" << setw(10) << duration_queries << " ms" << endl;
        cout << setw(30) << "   Average Recall:" << setw(10) << avgRecall << endl;
        cout << setw(30) << "   QPS:" << setw(10) << (nbQueries * 1000 / duration_queries) << endl;


        // save the results
        SaveKNN(knn_results, knn_save_path);

        // Read back from binary file
        vector<vector<uint32_t>> loaded_knn = ReadKNN(knn_save_path, K);

        // Verify the data
        if (knn_results == loaded_knn) {
            cout << "\n> Data verified successfully. File saved and loaded correctly.\n";
        }
        else {
            cout << "\n> Data verification failed. Mismatch found.\n";
        }

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}