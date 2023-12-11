#include "compute_ldpc.h"
#include "ldpc_operations.h"
#include <iostream>
#include <string>
#include <bitset>
#include <vector>
#include <regex>
#include <cstdlib>


void encodeAndPrint(const std::string& input, const std::string& matrix_csv) {
    std::string text_bits;
    for (char c : input) {
        std::bitset<8> byte(c);
        text_bits += byte.to_string();
    }

    std::vector<int> text;
    for (char c : text_bits) {
        text.push_back(c - '0');
    }

    int n = 2 * text.size();
    int d_c = text.size();
    int d_v = d_c - 1;

    int num_cols = n;
    int num_rows = n / d_v;
    std::cout << "num rows of the first block: " << num_rows << std::endl;
    std::cout << "num cols of the first block: " << num_cols << std::endl;


    std::vector<std::vector<int>> firstBlock = computeFirstBlock(num_cols, num_rows);
    std::cout << "First block:" << std::endl;
    printMatrix(firstBlock);

    std::cout << "Input message in binary: " << text_bits << std::endl;
    std::cout << "n = " << n << std::endl;
    std::cout << "d_v = " << d_v << std::endl;
    std::cout << "d_c = " << d_c << std::endl;

    std::vector<std::vector<int>> parityCheckMatrix = generateParityCheckMatrix(n, d_v, d_c, firstBlock, 42, matrix_csv);
    std::cout << "Parity-Check Matrix (H):" << std::endl;    
    printMatrix(parityCheckMatrix);

    std::vector<std::vector<int>> encodedMessage = encodeLDPC(input, parityCheckMatrix);
    std::cout << "Encoded message: ";

    printFirstColumn(encodedMessage);
}

void printFirstColumn(const std::vector<std::vector<int>>& matrix) {
    for (size_t i = 0; i < matrix.size(); ++i) {
        std::cout << matrix[i][0];
    }
    std::cout << std::endl;
}


void processLDPC(const std::string& input, const std::string& matrix_csv, const std::string& mode) {
    if (!validateInput(input, mode)) {
        exit(1);
    }

    if (mode == "-e") {
        encodeAndPrint(input, matrix_csv);
    } else if (mode == "-d") {
        std::string decodedMessage = decodeLDPC(input, matrix_csv);
        std::cout << "Decoded message: " << decodedMessage << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 4 || std::string(argv[1]) != "-m" || (std::string(argv[3]) != "-e" && std::string(argv[3]) != "-d")) {
        std::cerr << "Usage: " << argv[0] << " -m [matrix.csv] -e/-d" << std::endl;
        return 1;
    }

    std::string matrix_csv = argv[2];
    std::string mode = argv[3];
    std::string input;
    std::getline(std::cin, input);
    processLDPC(input, matrix_csv, mode);

    return 0;
}
