#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP


// #include "Compiler.hpp"
#include <iostream>
#include <vector>
#include <unordered_map>

#include <string>

class SymbolTable {
public:
    SymbolTable() 
    {
    }

    static void update_table(std::string s);
    static std::vector<int> identifiers;
    static std::vector<int> numbers;
    static std::unordered_map<std::string, int> symbol_table;
    static std::unordered_map<std::string, int> operator_table;
    static void print_table();
};

// extern std::unordered_map<std::string, int> SymbolTable::symbol_table;

#endif