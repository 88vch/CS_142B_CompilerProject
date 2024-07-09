#ifndef RESULT_HPP
#define RESULT_HPP

#include <vector>
#include <iostream>
#include "SymbolTable.hpp"

struct Result {
    int kind;   // [0: const, 1: identifier, 2: keyword]
    int value;  // symbol_table value representing that kind of Result

    Result(int k, int v) {
        if (k >= 0 && k <= 2) {
            kind = k;
            value = v;
        } else {
            std::cout << "invalid [kind] for Result!" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
};

std::vector<Result> int_to_result(std::vector<int> tokenizerTokens) {
    std::vector<Result> resultTokens = {};

    for (const int &token_value : tokenizerTokens) {
        if (SymbolTable::symbol_table.find(token_value) != SymbolTable::symbol_table.end()) {
            int token_kind, token_value; // ToDo: update this val once logic has been properly configured
            std::string val = SymbolTable::symbol_table.at(token_value);
            // ToDo: determine if [current token] is [const, identifier, or keyword]
            // - first check const (i.e. num) [1) try convert string to int, 2) if successful (1), check for existence in [SymbolTable::numbers]]
            //      - identifiers & keywords both start with a char, so [std::stoi()] works!
            try {
                int int_val = std::stoi(val);
                // sanity check
                if (SymbolTable::numbers.find(val) == SymbolTable::numbers.end()) {
                    exit(EXIT_FAILURE);
                }
                token_kind = 0; // a [number] is a [const]
                token_value = SymbolTable::numbers.at(val);
            } catch (...) {
                // - then check keyword [SymbolTable::keywords -> map]
                // - finally check identifier [SymbolTable::identifiers -> map]
            }
            // - if all have been checked and we haven't found the kind of [current token], we assume error (note: will this condition/branch even occur?)
            
            Result r(token_kind, token_value);
            resultTokens.push_back(r);
        } else {
            std::cout << "Expected token (int) does not exist in [SymbolTable::symbol_table!]" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

#endif