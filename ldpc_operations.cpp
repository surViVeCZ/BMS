#include "ldpc_operations.h"
#include <iostream>
#include <bitset>
#include <regex>
#include "compute_ldpc.h"


bool validateInput(const std::string &input, const std::string &mode) {
    std::regex pattern("[a-zA-Z0-9]+");
    if (mode == "-e") {
        return std::regex_match(input, pattern);
    } else if (mode == "-d") {
        std::regex zeroOnePattern("[01]+");
        return std::regex_match(input, zeroOnePattern);
    }
    return false;
}

void printMatrix(const std::vector<std::vector<int>>& matrix) {
    for (const auto& row : matrix) {
        for (int element : row) {
            std::cout << element << " ";
        }
        std::cout << std::endl;
    }
}

void printMatrix(const std::vector<std::vector<double>>& matrix) {
    for (const auto& row : matrix) {
        for (double element : row) {
            std::cout << element << " ";
        }
        std::cout << std::endl;
    }
}

std::vector<std::vector<int>> encodeLDPC(const std::string &message, const std::vector<std::vector<int>> &parityCheckMatrix) {
    std::cout << "LDPC Encoding of the message: " << message << std::endl;

    // Convert input message to a bit vector
    std::string binaryMessage = "";
    for (char c : message) {
        std::bitset<8> byte(c);
        binaryMessage += byte.to_string();
    }

    // Convert string to bit vector, MSB first
    std::vector<bool> binaryMessageVector;
    for (char c : binaryMessage) {
        binaryMessageVector.push_back(c == '1');
    }

    // Get the coding matrix G
    matrix G = codingMatrix(parityCheckMatrix);

    // Ensure the size of the binary message vector matches the size of G
    if (binaryMessageVector.size() < G.size()) {
        size_t paddingSize = G.size() - binaryMessageVector.size();
        binaryMessageVector.insert(binaryMessageVector.end(), paddingSize, false); // Adding zeros for padding
    } else if (binaryMessageVector.size() > G.size()) {
        std::cerr << "Error: Message size is larger than matrix size. Truncating the message." << std::endl;
        binaryMessageVector.resize(G.size()); 
    }

    // Create a matrix representation of the binary message
    matrix bin_mes(G.size());
    for (size_t i = 0; i < G.size(); ++i) {
        bin_mes[i].resize(1);
        bin_mes[i][0] = binaryMessageVector[i] ? 1 : 0;
    }

    // Multiply G and bin_mes
    matrix encodedMessage = binaryProduct(G, bin_mes);
    return encodedMessage;
}

vector<int> calculateSyndrome(const vector<int>& y, const vector<vector<int>>& H) {
    vector<int> syndrome;
    for (size_t j = 0; j < H.size(); j++) {
        int tmp = 0;
        for (size_t k = 0; k < y.size(); k++) {
            tmp += y[k] * H[j][k];
        }
        syndrome.push_back(tmp % 2);
    }
    return syndrome;
}

bool hasZeroSyndrome(const std::vector<int>& syndrome) {
    return (std::accumulate(syndrome.begin(), syndrome.end(), 0) == 0);
}

void errorCorrection(std::vector<int>& data, const std::vector<std::vector<int>>& parityCheckMatrix) {
    std::vector<int> syndrome = calculateSyndrome(data, parityCheckMatrix);

    if (hasZeroSyndrome(syndrome)) {
        return;
    }

    std::vector<int> errorVector;
    size_t col = 0;
    while (col < parityCheckMatrix[0].size()) {
        int sum = 0;
        size_t row = 0;
        while (row < parityCheckMatrix.size()) {
            sum += syndrome[row] * parityCheckMatrix[row][col];
            row++;
        }
        errorVector.push_back(sum);
        col++;
    }

    int maxError = *std::max_element(errorVector.begin(), errorVector.end());
    std::vector<size_t> errorIndices;
    size_t i = 0;
    while (i < errorVector.size()) {
        if (errorVector[i] == maxError) {
            errorIndices.push_back(i);
        }
        i++;
    }

    for (size_t index : errorIndices) {
        data[index] = (data[index] + 1) % 2;
    }
}


// Function to decode LDPC using syndrome calculation and error correction
std::string decodeLDPC(const std::string &message, const std::string &matrix_csv) {
    // Convert message string to vector<int> assuming '0's and '1's
    std::vector<int> receivedMessage;
    for (char c : message) {
        if (c == '0' || c == '1') {
            receivedMessage.push_back(c - '0');
        }
    }

    // Convert matrix_csv string to parity check matrix H
    std::vector<std::vector<int>> H;
    size_t size = static_cast<size_t>(std::sqrt(matrix_csv.size()));
    size_t i = 0;
    while (i < size) {
        std::vector<int> row;
        size_t j = 0;
        while (j < size) {
            row.push_back(matrix_csv[i * size + j] - '0');
            j++;
        }
        H.push_back(row);
        i++;
    }

    // Perform error correction
    errorCorrection(receivedMessage, H);
    std::string correctedMessage;
    size_t k = 0;
    while (k < receivedMessage.size()) {
        correctedMessage += std::to_string(receivedMessage[k]);
        k++;
    }
    return correctedMessage;
}

