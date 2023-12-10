#include "compute_ldpc.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <random>
#include <iostream>
#include <utility>
#include "ldpc_operations.h"
#include <cassert>

using namespace std;
typedef std::vector<std::vector<int>> matrix;

template<typename T>
std::pair<std::vector<std::vector<T>>, matrix> gaussJordan(std::vector<std::vector<T>> matrix, bool change = false) {
    int numRows = matrix.size();
    if (numRows == 0) {
        return std::make_pair(matrix, std::vector<std::vector<int>>());
    }
    int numCols = matrix[0].size();

    std::vector<std::vector<int>> P(numRows, std::vector<int>(numRows, 0));

    for (int i = 0; i < numRows; ++i) {
        P[i][i] = 1;
    }

    int old_pivot = -1;
    for (int col = 0; col < numCols; ++col) {
        int pivotRow = old_pivot + 1;
        for (int row = old_pivot + 1; row < numRows; ++row) {
            if (matrix[row][col] == 1) {
                pivotRow = row;
                break;
            }
        }

        if(matrix[pivotRow][col] == 1) {
            old_pivot++;
            if (pivotRow != old_pivot) {
                std::swap(matrix[pivotRow], matrix[old_pivot]);
                if (change) {
                    std::swap(P[pivotRow], P[old_pivot]);
                }
            }

            for (int row = 0; row < numRows; ++row) {
                if (row != old_pivot && matrix[row][col] == 1) {
                    for (int j = 0; j < numCols; ++j) {
                        matrix[row][j] = (matrix[row][j] + matrix[old_pivot][j]) % 2;
                        if (change) {
                            P[row][j] = (P[row][j] + P[old_pivot][j]) %2;
                        }
                    }
                }
            }
        }

        if(old_pivot == numRows - 1) {
            break;
        }
    }

    if (change) {
        return std::make_pair(matrix, P);
    }
    return std::make_pair(matrix, std::vector<std::vector<int>>());
}

matrix binaryProduct(const std::vector<std::vector<int>>& X, const std::vector<std::vector<int>>& Y) {
    int n = static_cast<int>(X.size());
    int m = static_cast<int>(Y.size());
    int p = static_cast<int>(Y[0].size());

    matrix Z(n, std::vector<int>(p, 0));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < p; ++j) {
            for (int k = 0; k < m; ++k) {
                Z[i][j] = (Z[i][j] + X[i][k] * Y[k][j]) % 2;
            }
        }
    }

    return Z;
}

void printMatrixToFile(const matrix& mat, const std::string& filename) {
    std::ofstream file(filename);

    if (file.is_open()) {
        for (const auto& row : mat) {
            for (size_t i = 0; i < row.size(); ++i) {
                file << row[i];
                if (i != row.size() - 1) {
                    file << ",";
                }
            }
            file << "\n";
        }
        file.close();
        std::cout << "Matrix saved to " << filename << " successfully." << std::endl;
    } else {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }
}

matrix codingMatrix(const matrix& H) {
    matrix H_transpose = transposeMatrix(H);
    cout << "H_transpose" << endl;
    printMatrix(H_transpose);

    auto Href_colonnes_pair = gaussJordan(H_transpose, true);
    auto Href_colonnes = Href_colonnes_pair.first;
    auto tQ = Href_colonnes_pair.second;
    auto Q = transposeMatrix(tQ);

    cout << "Href_colonnes" << endl;
    printMatrix(Href_colonnes);

    auto Href_diag_pair = gaussJordan(transposeMatrix(Href_colonnes));
    auto Href_diag = Href_diag_pair.first;

    int n_code = static_cast<int>(H[0].size());
    int n_bits = n_code - accumulate(Href_diag.begin(), Href_diag.end(), 0,
        [](int sum, const vector<int>& row) {
            return sum + accumulate(row.begin(), row.end(), 0);
        });

    matrix Y(n_code, vector<int>(n_bits, 0));
    for (int i = n_code - n_bits; i < n_code; ++i) {
        Y[i][i - (n_code - n_bits)] = 1;
    }


    matrix tG = binaryProduct(Q, Y);

    cout << "Q" << endl;
    printMatrix(Q);
    cout << "Y" << endl;
    printMatrix(Y);

    cout << "G:" << endl;
    printMatrix(tG);
    return tG;
}

std::mt19937& getGenerator(int seed) {
    static std::random_device rd;
    static std::mt19937 generator(seed != 0 ? seed : rd());
    return generator;
}

std::mt19937& checkRandomState(int seed) {
    return getGenerator(seed);
}

matrix computeFirstBlock(int num_cols, int num_ones_per_row) {
    int num_rows = static_cast<int>(ceil(static_cast<double>(num_cols) / num_ones_per_row));
    matrix firstBlock(num_rows, std::vector<int>(num_cols, 0));

    for (int i = 0; i < num_rows; ++i) {
        int start_col = (i % 2 == 0) ? 0 : num_ones_per_row;
        for (int j = start_col; j < num_cols; j += (2 * num_ones_per_row)) {
            for (int k = 0; k < num_ones_per_row && (j + k) < num_cols; ++k) {
                firstBlock[i][j + k] = 1;
            }
        }
    }
    return firstBlock;
}

matrix transposeMatrix(const std::vector<std::vector<int>>& matrix) {
    int rows = matrix.size();
    int cols = matrix[0].size();

    std::vector<std::vector<int>> transposed(cols, std::vector<int>(rows));

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            transposed[j][i] = matrix[i][j];
        }
    }

    return transposed;
}

matrix generateParityCheckMatrix(int n_code, int d_v, int d_c, const matrix& firstBlock, int seed, const std::string &matrix_csv) {
    int n_equations = (n_code * d_v) / d_c;
    matrix H(n_equations, std::vector<int>(n_code, 0));

    int block_size = n_equations / d_v;

    for (int i = 0; i < block_size; ++i) {
        for (int j = 0; j < n_code; ++j) {
            H[i][j] = firstBlock[i][j];
        }
    }
    std::mt19937& generator = checkRandomState(seed);


    for (int i = 1; i < d_v; ++i) {
        auto block_transposed = transposeMatrix(firstBlock);
        std::shuffle(block_transposed.begin(), block_transposed.end(), generator);
        block_transposed = transposeMatrix(block_transposed);

        for (int j = 0; j < block_size; ++j) {
            int idx = i * block_size + j;
            int col_idx = 0;

            while (col_idx < n_code) {
                int ones_in_col = 0;
                for (int k = 0; k < n_code; ++k) {
                    if (block_transposed[j][k] == 1 && ones_in_col < d_c) {
                        H[idx][k] = 1;
                        ++ones_in_col;
                        ++col_idx;
                    }
                }
            }
        }
    }
    printMatrixToFile(H, matrix_csv);
    return H;
}