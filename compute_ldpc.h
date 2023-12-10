#ifndef COMPUTE_LDPC_H
#define COMPUTE_LDPC_H

#include <cmath>
#include <iostream>
#include <string>
#include <algorithm>
#include <random>
#include <utility>
#include <vector>

using namespace std;

typedef std::vector<std::vector<int>> matrix;

matrix binaryProduct(const std::vector<std::vector<int>>& X, const std::vector<std::vector<int>>& Y);

matrix codingMatrix(const matrix& H);

std::mt19937& getGenerator(int seed);

std::mt19937& checkRandomState(int seed = 42);

matrix computeFirstBlock(int num_cols, int num_ones_per_row);

matrix transposeMatrix(const std::vector<std::vector<int>>& matrix);

matrix generateParityCheckMatrix(int n_code, int d_v, int d_c, const matrix& firstBlock, int seed, const std::string &matrix_csv);

void printMatrixToFile(const matrix& mat, const std::string& filename);

std::pair<std::vector<std::vector<double>>, std::vector<std::vector<int>>> gaussJordan(std::vector<std::vector<double>> matrix, bool change);

 
#endif /* COMPUTE_LDPC_H */
