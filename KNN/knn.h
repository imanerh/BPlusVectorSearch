#ifndef KNN_H
#define KNN_H

#include <vector>
#include <utility>
#include <cmath>
#include <algorithm> 
#include <limits>
#include "../hnswlib/hnswlib.h"
#include "../macros.h"
using namespace std;

// Function to calculate Euclidean distance squared
float euclideanDistanceSq(const vector<float>& v, const vector<float>& q);

// Brute force KNN implementation
vector<uint32_t> bruteForceKNN(const vector<int>& ids, const vector<vector<float>>& dataset, const vector<float>& query, int k);

// Function to create an HNSW graph
hnswlib::HierarchicalNSW<float>* createHNSW(const vector<vector<float>>& dataset);

// HNSW-based KNN implementation
vector<uint32_t> hnswKNN(hnswlib::HierarchicalNSW<float>* hnsw, const vector<vector<float>>& dataset, const vector<float>& query, int k, float l, float r, int k_init);

// Function to decide which KNN method to use
vector<uint32_t> computeKNN(hnswlib::HierarchicalNSW<float>* hnsw, const vector<int>& ids, const vector<vector<float>>& dataset, const vector<float>& query, int k, float l, float r, int threshold, int k_init);


#endif 