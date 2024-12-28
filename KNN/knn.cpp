#include <chrono>
#include <thread>
#include <functional>
#include "knn.h"
#include "../macros.h"
using namespace std;

/**
 * @brief Calculates the squared Euclidean distance between two vectors
 *
 * @param v The first vector
 * @param q The second vector
 *
 * This function computes the squared Euclidean distance between two
 * vectors, ignoring any extra dimensions defined by NODE_EXTRAS and
 * QUERY_EXTRAS.
 *
 * @return The squared distance as a float
 */
float euclideanDistanceSq(const vector<float>& v, const vector<float>& q) {
    float distance = 0.0;
    for (size_t j = 0; j < VEC_DIM; ++j) {
        distance += pow(v[j + NODE_EXTRAS] - q[j + QUERY_EXTRAS], 2);
    }
    return distance;
}

/**
 * @brief Performs brute force KNN search
 *
 * @param ids A vector of identifiers for the dataset points
 * @param dataset A 2D vector containing the dataset points
 * @param query The query point for which KNN is to be found
 * @param k The number of nearest neighbors to return
 *
 * This function implements a brute force approach to find the K nearest
 * neighbors of a given query point by calculating the distance to all
 * points in the dataset.
 *
 * @return A vector of identifiers for the K nearest neighbors
 */
vector<uint32_t> bruteForceKNN(const vector<int>& ids, const vector<vector<float>>& dataset, const vector<float>& query, int k) {
    vector<pair<float, int>> distances; // Pair of (distance, id)

    for (int id : ids) {
        float dist = euclideanDistanceSq(dataset[id], query);
        distances.push_back({ dist, id });
    }

    // Sort by distance
    sort(distances.begin(), distances.end());

    // Extract top k results
    vector<uint32_t> knn;
    for (int i = 0; i < k && i < distances.size(); i++) {
        knn.push_back(distances[i].second);
    }
    return knn;
}

// // Multithreaded executor
// // Source: https://github.com/nmslib/hnswlib/blob/master/examples/cpp/example_mt_search.cpp
// template<class Function>
// inline void ParallelFor(size_t start, size_t end, size_t numThreads, Function fn) {
//     if (numThreads <= 0) {
//         numThreads = std::thread::hardware_concurrency();
//     }

//     if (numThreads == 1) {
//         for (size_t id = start; id < end; id++) {
//             fn(id, 0);
//         }
//     }
//     else {
//         std::vector<std::thread> threads;
//         std::atomic<size_t> current(start);

//         // keep track of exceptions in threads
//         // https://stackoverflow.com/a/32428427/1713196
//         std::exception_ptr lastException = nullptr;
//         std::mutex lastExceptMutex;

//         for (size_t threadId = 0; threadId < numThreads; ++threadId) {
//             threads.push_back(std::thread([&, threadId] {
//                 while (true) {
//                     size_t id = current.fetch_add(1);

//                     if (id >= end) {
//                         break;
//                     }

//                     try {
//                         fn(id, threadId);
//                     }
//                     catch (...) {
//                         std::unique_lock<std::mutex> lastExcepLock(lastExceptMutex);
//                         lastException = std::current_exception();
//                         /*
//                          * This will work even when current is the largest value that
//                          * size_t can fit, because fetch_add returns the previous value
//                          * before the increment (what will result in overflow
//                          * and produce 0 instead of current + 1).
//                          */
//                         current = end;
//                         break;
//                     }
//                 }
//                 }));
//         }
//         for (auto& thread : threads) {
//             thread.join();
//         }
//         if (lastException) {
//             std::rethrow_exception(lastException);
//         }
//     }
// }

/**
 * @brief Creates an HNSW graph from the dataset
 *
 * @param dataset A 2D vector containing the dataset points
 *
 * This function initializes and builds an HNSW graph using the provided
 * dataset, allowing for efficient KNN searches.
 *
 * @return A pointer to the created HNSW graph
 */
// hnswlib::HierarchicalNSW<float>* createHNSW(const vector<vector<float>>& dataset) {
//     // Start timing
//     auto start_time = std::chrono::high_resolution_clock::now();
//     hnswlib::L2Space space(VEC_DIM);
//     size_t num_points = dataset.size();
//     auto* hnsw = new hnswlib::HierarchicalNSW<float>(&space, num_points, HNSW_M, HNSW_EF_CONSTRUCTION);
//     // Multithreading
//     // Source: https://github.com/nmslib/hnswlib/blob/master/examples/cpp/example_mt_search.cpp
//     const unsigned int num_threads = thread::hardware_concurrency();
//     float* data = new float[VEC_DIM * num_points];
//     for (int i = 0; i < num_points; i++) {
//         memcpy(data + i * VEC_DIM, dataset[i].data() + NODE_EXTRAS, VEC_DIM * sizeof(float)); // Fill data array
//     }
//     // Add data to index
//     ParallelFor(0, num_points, num_threads, [&](size_t row, size_t threadId) {
//         hnsw->addPoint((void*)(data + VEC_DIM * row), row);
//         });
//     // // Query the elements for themselves and measure recall
//     // std::vector<hnswlib::labeltype> neighbors(num_points);
//     // ParallelFor(0, num_points, num_threads, [&](size_t row, size_t threadId) {
//     //     std::priority_queue<std::pair<float, hnswlib::labeltype>> result = hnsw->searchKnn(data + VEC_DIM * row, 1);
//     //     hnswlib::labeltype label = result.top().second;
//     //     neighbors[row] = label;
//     //     });
//     // float correct = 0;
//     // for (int i = 0; i < num_points; i++) {
//     //     hnswlib::labeltype label = neighbors[i];
//     //     if (label == i) correct++;
//     // }
//     // float recall = correct / num_points;
//     // std::cout << "\nHNSW Recall: " << recall << "\n";
//     // End timing
//     auto end_time = std::chrono::high_resolution_clock::now();
//     auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
//     std::cout << "HNSW graph building time: " << elapsed_time << " ms" << std::endl;

//     return hnsw;
// }

hnswlib::HierarchicalNSW<float>* createHNSW(const vector<vector<float>>& dataset) {
    // Start timing
    auto start_time = std::chrono::high_resolution_clock::now();

    hnswlib::L2Space space(VEC_DIM);   
    size_t num_points = dataset.size();
    hnswlib::HierarchicalNSW<float>* hnsw = new hnswlib::HierarchicalNSW<float>(&space, num_points, HNSW_M, HNSW_EF_CONSTRUCTION);

    // Add points to the graph
    float* data = new float[VEC_DIM * num_points];
    for (int i = 0; i < num_points; i++) {
        memcpy(data + i * VEC_DIM, dataset[i].data() + NODE_EXTRAS, VEC_DIM * sizeof(float));
    }
    for (int i = 0; i < num_points; i++) {
        hnsw->addPoint(data + i * VEC_DIM, i);
    };

    // End timing
    auto end_time = std::chrono::high_resolution_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    std::cout << "\nHNSW graph building time: " << elapsed_time << " ms" << std::endl;

    // Query the elements for themselves and measure recall
    float correct = 0;
    for (int i = 0; i < num_points; i++) {
        std::priority_queue<std::pair<float, hnswlib::labeltype>> result = hnsw->searchKnn(data + i * VEC_DIM, 1);
        hnswlib::labeltype label = result.top().second;
        if (label == i) correct++;
    }
    float recall = correct / num_points;
    std::cout << "HNSW Recall: " << recall << "\n";

    return hnsw;
}

/**
 * @brief Performs KNN search using the HNSW graph
 *
 * @param hnsw A pointer to the HNSW graph
 * @param dataset A 2D vector containing the dataset points
 * @param query The query point for which KNN is to be found
 * @param k The number of nearest neighbors to return
 * @param l The lower bound for filtering results
 * @param r The upper bound for filtering results
 *
 * This function searches for the K nearest neighbors of a query point
 * using the HNSW graph and applies post-filtering based on the specified
 * range.
 *
 * @return A vector of identifiers for the K nearest neighbors that satisfy the range [l, r]
 */
vector<uint32_t> hnswKNN(hnswlib::HierarchicalNSW<float>* hnsw, const vector<vector<float>>& dataset, const vector<float>& query, int k, float l, float r, int k_init) {
    // Perform KNN search
    auto result = hnsw->searchKnn(query.data() + QUERY_EXTRAS, k_init);

    // POST-Filtering
    vector<pair<float, int>> neighbors;
    while (!result.empty()) {
        int id = result.top().second;
        float distance = result.top().first;
        result.pop();
        // Check if the vector satisfies the range query
        if (dataset[id][NODE_CONTINUOUS_INDEX] >= l && dataset[id][NODE_CONTINUOUS_INDEX] <= r) {
            neighbors.emplace_back(distance, id);
        }
    }
    // Sort the filtered neighbors by distance in ascending order
    sort(neighbors.begin(), neighbors.end());
    // Extract the top k nearest neighbors
    vector<uint32_t> knn;
    for (int i = 0; i < k && i < neighbors.size(); ++i) {
        knn.push_back(neighbors[i].second);
    }

    return knn;
}

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
 // vector<int> computeKNN(hnswlib::HierarchicalNSW<float>* hnsw, const vector<int>& ids, const vector<vector<float>>& dataset, const vector<float>& query, int k, float l, float r, int threshold) {
 //     if (ids.size() <= threshold) {
 //         cout << "=> BRUTEEFORCE" << endl;
 //         return bruteForceKNN(ids, dataset, query, k);
 //     }
 //     else {
 //         cout << "=> HNSW" << endl;
 //         return hnswKNN(hnsw, dataset, query, k, l, r); 
 //     }
 // }