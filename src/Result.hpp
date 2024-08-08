#ifndef RESULT_HPP
#define RESULT_HPP

#include <vector>
#include <iostream>
#include "SymbolTable.hpp"

#undef DEBUG_I2R
#define DEBUG

class Result {
public:
    Result(int k, int v) {
        if (k >= 0 && k <= 2) {
            kind = k;
            value = v;
        } else {
            std::cout << "invalid [kind] for Result!" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    Result() {} // why do we need this to satisfy [Parser] constructor? 

    bool compare(const Result &other) {
        return (this->kind == other.kind) ? (this->value == other.value) : false;
    }

    std::string to_string_literal() const {
        return "Result(" + std::to_string(this->kind) + ", " + std::to_string(this->value) + ")";
    }
    std::string to_string() const {
        return "Result(" + this->get_kind() + ", " + this->get_value() + ")";
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

    // get the int value of the kind
    inline int get_kind_literal() const { return this->kind; }
    // get the int value of the value
    inline int get_value_literal() const { 
        if (this->kind == 0) {
            return std::stoi(SymbolTable::symbol_table.at(this->value)); 
        } else if (this->kind == 1) {
            // return SymbolTable::symbol_table.at(this->value);
            return this->value;
        } else { // for keyword (idk if this is right but for now...?)
            return this->value;
        }
    }
    
    static std::vector<Result> int_to_result(std::vector<int> tokenizerTokens);
private:
    int kind;   // [0: const, 1: identifier, 2: keyword]
    int value;  // [symbol_table] value representing that kind of Result
};

#endif