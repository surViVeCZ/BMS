#ifndef LDPC_OPERATIONS_H
#define LDPC_OPERATIONS_H

#include <string>
#include <vector>

void encodeAndPrint(const std::string& input, const std::string& matrix_csv);
void processLDPC(const std::string& input, const std::string& matrix_csv, const std::string& mode);

#endif /* LDPC_OPERATIONS_H */
