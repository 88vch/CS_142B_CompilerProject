#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP


#include "Func.hpp"
#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>

namespace SymbolTable {
    void print_symbol_table();
    void print_identifiers_table();
    void print_numbers_table();
    void update_table(std::string s, std::string type);
    void clearFuncTable();
    
    
    extern std::unordered_map<std::string, int> identifiers;
    extern std::unordered_map<std::string, int> numbers;
    extern const std::unordered_map<std::string, int> keywords;
    extern std::unordered_map<int, std::string> symbol_table; // find() from [symbol_table]
    extern std::unordered_map<int, Func*> func_table; // find() from [func_table]
    
    // [11.21.2024]: removed const for UDF if we ned to add more arg SSA's (getpar4, getpar5, etc...)
    extern std::unordered_map<std::string, int> operator_table; // BasicBlock; translate into operator from [operator_table]
    extern std::unordered_map<int, std::string> operator_table_reversed;
};


#endif