#include "Result.hpp"
#include <vector>

std::vector<Result> Result::int_to_result(std::vector<int> tokenizerTokens) {
    std::vector<Result> resultTokens = {};

    for (const int &token_value : tokenizerTokens) {
        #ifdef DEBUG
            std::cout << "token_value: [" << token_value << "]" << std::endl; 
        #endif
        if (SymbolTable::symbol_table.find(token_value) != SymbolTable::symbol_table.end()) {
            #ifdef DEBUG
                std::cout << "\t[token_value] exists in [SymbolTable::symbol_table] with value: [" << SymbolTable::symbol_table.at(token_value) << "]" << std::endl;
            #endif
            int token_kind; // ToDo: update this val once logic has been properly configured
            // std::string val = SymbolTable::symbol_table.at(token_value);
            #define val SymbolTable::symbol_table.at(token_value)
            // ToDo: determine if [current token] is [const, identifier, or keyword]
            // - first check const (i.e. num) [1) try convert string to int, 2) if successful (1), check for existence in [SymbolTable::numbers]]
            //      - identifiers & keywords both start with a char, so [std::stoi()] works!
            try {
                #ifdef DEBUG
                    std::cout << "\tfirst checking if [token_value] is num (i.e. const)" << std::endl;
                #endif
                int int_val = std::stoi(val);
                // sanity check
                if (SymbolTable::numbers.find(val) != SymbolTable::numbers.end()) {
                    token_kind = 0; // a [number] is a [const]
                }
            } catch (...) {
                // - then check keyword [SymbolTable::keywords -> map]
                #ifdef DEBUG
                    std::cout << "\tnot num; checking if [token_value] is keyword or identifier" << std::endl;
                #endif
                if (SymbolTable::keywords.find(val) != SymbolTable::keywords.end()) {
                    #ifdef DEBUG
                        std::cout << "\t\t[token_value] is keyword" << std::endl;
                    #endif
                    token_kind = 2;
                }
                // - finally check identifier [SymbolTable::identifiers -> map]
                else if (SymbolTable::identifiers.find(val) != SymbolTable::identifiers.end()) {
                    #ifdef DEBUG
                        std::cout << "\t\t[token_value] is identifier" << std::endl;
                    #endif
                    token_kind = 1;
                }
                // - if all have been checked and we haven't found the kind of [current token], we assume error (note: will this condition/branch even occur?)
                // sanity check (extra ig :)
                else {
                    std::cout << "Expected token (int) exists with unknown type [const, ident, keyword]" << std::endl;
                    exit(EXIT_FAILURE);
                }
            }
            Result r(token_kind, token_value);
            resultTokens.push_back(r);
        } else {
            std::cout << "Expected token (int) does not exist in [SymbolTable::symbol_table!]" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    return resultTokens;
}