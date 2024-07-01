#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP


// #include "Compiler.hpp"
#include <iostream>
#include <vector>
#include <unordered_map>

#include <string>

namespace SymbolTable {
    void print_symbol_table();
    void print_identifiers_table();
    void print_numbers_table();
    void update_table(std::string s);
    extern std::vector<int> identifiers;
    extern std::vector<int> numbers;
    extern std::unordered_map<std::string, int> symbol_table; // find() from [symbol_table]
    extern const std::unordered_map<std::string, int> operator_table; // translate into operator from [operator_table]
};

// extern std::unordered_map<std::string, int> SymbolTable::symbol_table;

#endif