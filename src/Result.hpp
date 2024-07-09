#ifndef RESULT_HPP
#define RESULT_HPP

#include <vector>
#include <iostream>
#include "SymbolTable.hpp"


namespace Res {
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

        std::string get_kind() const {
            std::string res; 
            switch(this->kind) {
                case 0:
                    res = "CONSTANT";
                    break;
                case 1:
                    res = "IDENTIFIER";
                    break;
                case 2:
                    res = "KEYWORD";
                    break;
            };
            return res; 
        } 

        std::string get_value() const { 
            std::string res = SymbolTable::symbol_table.at(this->value);
            return res;
        }
    };
    
    inline std::vector<Res::Result> int_to_result(std::vector<int> tokenizerTokens) {
        std::vector<Res::Result> resultTokens = {};

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
                    if (SymbolTable::keywords.find(val) != SymbolTable::keywords.end()) {
                        token_kind = 2;
                        token_value = SymbolTable::keywords.at(val);
                    }
                    // - finally check identifier [SymbolTable::identifiers -> map]
                    else if (SymbolTable::identifiers.find(val) != SymbolTable::identifiers.end()) {
                        token_kind = 1;
                        token_value = SymbolTable::identifiers.at(val);
                    }
                    // - if all have been checked and we haven't found the kind of [current token], we assume error (note: will this condition/branch even occur?)
                    // sanity check (extra ig :)
                    else {
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

}

#endif