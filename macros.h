#ifndef MACROS_H
#define MACROS_H

// Data Parameters
#define K 100  // KNN
#define VEC_DIM 100
#define NODE_EXTRAS 2 // categorical_value + continuous_value
#define NODE_CATEGORICAL_INDEX 0
#define NODE_CONTINUOUS_INDEX 1
#define NODE_DIMENSION (VEC_DIM + NODE_EXTRAS)  // categorical_value + continuous_value + vector_values
#define QUERY_EXTRAS 4 // query_type + categorical_value + l + r
#define QUERY_TYPE_INDEX 0
#define QUERY_CATEGORICAL_INDEX 1
#define QUERY_L_INDEX 2
#define QUERY_R_INDEX 3
#define QUERY_DIMENSION (VEC_DIM + QUERY_EXTRAS)  // query_type + categorical_value + l + r + vector_values

// HNSW Parameters
#define HNSW_M 24 // The number of edges per element during the graph creation in each layer
#define HNSW_EF_CONSTRUCTION 140 // The number of neighbors considered to find the nearest neighbor at each layer during graph creation

// Other
#define K_INIT 3000 // > K in the data parameters; the K nearest neighbors fetched in ANN (i.e. in case the threshold is exceeded)


#endif 