#ifndef LDPC_OPERATIONS_H
#define LDPC_OPERATIONS_H

#include <iostream>
#include <bitset>
#include <regex>
#include <vector>
typedef std::vector<std::vector<int>> matrix;

bool validateInput(const std::string &input, const std::string &mode);

void printMatrix(const std::vector<std::vector<int>>& matrix);
void printMatrix(const std::vector<std::vector<double>>& matrix);

matrix encodeLDPC(const std::string &message, const std::vector<std::vector<int>> &parityCheckMatrix);

std::string decodeLDPC(const std::string &message, const std::string &matrix_csv);

// Function to calculate syndrome
std::vector<int> calculateSyndrome(const std::vector<int>& y, const std::vector<std::vector<int>>& H);

// Function to check if syndrome is all zero
bool isSyndromeZero(const std::vector<int>& s_n);

// Function to perform error correction
void errorCorrection(std::vector<int>& y, const std::vector<std::vector<int>>& H);

#endif // LDPC_OPERATIONS_H
