#ifndef RESULT_HPP
#define RESULT_HPP

#include <vector>
#include <iostream>
#include "SymbolTable.hpp"

#undef DEBUG

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

    // Overload the equality operator
    bool operator==(const Result& other) const {
        return (this->kind == other.kind) ? true : (this->value == other.value);
    }
    // Overload the inequality operator
    bool operator!=(const Result& other) const {
        return !(*this == other);
    }

    std::string to_string() const {
        return "Result(" + std::to_string(this->kind) + ", " + std::to_string(this->value) + ")";
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
    int get_kind_literal() const { return this->kind; }
    // get the int value of the value
    int get_value_literal() const { return this->value; }
    
    static std::vector<Result> int_to_result(std::vector<int> tokenizerTokens);
private:
    int kind;   // [0: const, 1: identifier, 2: keyword]
    int value;  // [symbol_table] value representing that kind of Result
};

#endif