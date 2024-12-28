/**
 *  Example code for IO, read binary data vectors and save KNNs to path.
 *
 */

#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include "assert.h"
#include "../macros.h"
using std::cout;
using std::endl;
using std::string;
using std::vector;

/// @brief Save knng in binary format (uint32_t) with name "output.bin"
/// @param knn a (N * 100) shape 2-D vector
/// @param path target save path, the output knng should be named as
/// "output.bin" for evaluation
void SaveKNN(const std::vector<std::vector<uint32_t>> &knns,
              const std::string &path = "output.bin") {
  std::ofstream ofs(path, std::ios::out | std::ios::binary);
  const int k = K;
  const uint32_t N = knns.size();
  if (knns.front().size() != k) {
    cout << knns.front().size() << endl;
  }
  assert(knns.front().size() == k);
  for (unsigned i = 0; i < N; ++i) {
    auto const &knn = knns[i];
    ofs.write(reinterpret_cast<char const *>(&knn[0]), k * sizeof(uint32_t));
  }
  ofs.close();
}



/// @brief Reading binary data vectors. Raw data store as a (N x dim)
/// @param file_path file path of binary data
/// @param data returned 2D data vectors
void ReadBin(const std::string &file_path,
             const int num_dimensions,
             std::vector<std::vector<float>> &data) {
  std::ifstream ifs;
  ifs.open(file_path, std::ios::binary);
  assert(ifs.is_open());
  uint32_t N;  // num of points
  ifs.read((char *)&N, sizeof(uint32_t));
  data.resize(N);
  // std::cout << "# of points: " << N << std::endl;
  std::vector<float> buff(num_dimensions);
  int counter = 0;
  while (ifs.read((char *)buff.data(), num_dimensions * sizeof(float))) {
    std::vector<float> row(num_dimensions);
    for (int d = 0; d < num_dimensions; d++) {
      row[d] = static_cast<float>(buff[d]);
    }
    data[counter++] = std::move(row);
  }
  ifs.close();
  // std::cout << "Finish Reading Data" << endl;
}


/**
 * @brief Reads data points and queries from binary files
 *
 * @param source_path The path to the binary file containing data points
 * @param query_path The path to the binary file containing queries
 * @param nodes A reference to a vector that will store the read data points
 * @param queries A reference to a vector that will store the read queries
 *
 * This function reads data points and queries from the specified binary files
 * and logs the number of data points and queries read.
 */
void readData(const string& source_path, const string& query_path, vector<vector<float>>& nodes, vector<vector<float>>& queries
) {
    cout << "> Reading data...\n";
    // Read data points
    ReadBin(source_path, NODE_DIMENSION, nodes);
    // Read queries
    ReadBin(query_path, QUERY_DIMENSION, queries);
    // Log the number of data points and queries
    uint32_t n = nodes.size();    // Number of data points
    uint32_t nq = queries.size(); // Number of queries

    cout << "   # data points:  " << n << endl;
    cout << "   # queries:      " << nq << endl;
    cout << endl;
}


/**
 * @brief Reads K nearest neighbors from a binary file.
 *
 * @param path The path to the binary file containing KNN data.
 * @param k The number of nearest neighbors to read for each entry.
 *
 * @return A vector of vectors containing the K nearest neighbors for each query.
 */
std::vector<std::vector<uint32_t>> ReadKNN(const std::string& path, uint32_t k) {
    std::vector<std::vector<uint32_t>> knns;
    std::ifstream ifs(path, std::ios::in | std::ios::binary);
    if (!ifs) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    // Determine the number of entries by the file size
    ifs.seekg(0, std::ios::end);
    size_t file_size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    // Check if file size is a multiple of K * sizeof(uint32_t)
    if (file_size % (k * sizeof(uint32_t)) != 0) {
        throw std::runtime_error("Invalid file size for specified K value");
    }
    uint32_t N = file_size / (k * sizeof(uint32_t));
    knns.resize(N, std::vector<uint32_t>(k));
    // Read data
    for (uint32_t i = 0; i < N; ++i) {
        ifs.read(reinterpret_cast<char*>(knns[i].data()), k * sizeof(uint32_t));
    }
    ifs.close();

    return knns;
}


// Multithreaded executor
// Source: https://github.com/nmslib/hnswlib/blob/master/examples/cpp/example_mt_search.cpp
template<class Function>
inline void ParallelFor(size_t start, size_t end, size_t numThreads, Function fn) {
    if (numThreads <= 0) {
        numThreads = std::thread::hardware_concurrency();
    }

    if (numThreads == 1) {
        for (size_t id = start; id < end; id++) {
            fn(id, 0);
        }
    }
    else {
        std::vector<std::thread> threads;
        std::atomic<size_t> current(start);

        // keep track of exceptions in threads
        // https://stackoverflow.com/a/32428427/1713196
        std::exception_ptr lastException = nullptr;
        std::mutex lastExceptMutex;

        for (size_t threadId = 0; threadId < numThreads; ++threadId) {
            threads.push_back(std::thread([&, threadId] {
                while (true) {
                    size_t id = current.fetch_add(1);

                    if (id >= end) {
                        break;
                    }

                    try {
                        fn(id, threadId);
                    }
                    catch (...) {
                        std::unique_lock<std::mutex> lastExcepLock(lastExceptMutex);
                        lastException = std::current_exception();
                        /*
                         * This will work even when current is the largest value that
                         * size_t can fit, because fetch_add returns the previous value
                         * before the increment (what will result in overflow
                         * and produce 0 instead of current + 1).
                         */
                        current = end;
                        break;
                    }
                }
                }));
        }
        for (auto& thread : threads) {
            thread.join();
        }
        if (lastException) {
            std::rethrow_exception(lastException);
        }
    }
}