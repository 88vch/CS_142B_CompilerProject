#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP


// #include "Compiler.hpp"
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
    extern std::unordered_map<std::string, int> identifiers;
    extern std::unordered_map<std::string, int> numbers;
    extern const std::unordered_map<std::string, int> keywords;
    // extern std::unordered_map<std::string, int> symbol_table; // find() from [symbol_table]
    extern std::unordered_map<int, std::string> symbol_table; // find() from [symbol_table]
    extern const std::unordered_map<std::string, int> operator_table; // BasicBlock; translate into operator from [operator_table]
    extern const std::unordered_map<int, std::string> operator_table_reversed;
};

// extern std::unordered_map<std::string, int> SymbolTable::symbol_table;

#endif