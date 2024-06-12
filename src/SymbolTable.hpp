#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP


// #include "Compiler.hpp"
#include <iostream>
#include <unordered_map>

#include <string>
// #include "Token.hpp" 

// TODO: cleanly move symbol table here & make sure tokenizer still works!
class SymbolTable {
public:
    SymbolTable() 
    {
    }

    static std::unordered_map<std::string, int> symbol_table;
    void print_table();
};

// extern std::unordered_map<std::string, int> SymbolTable::symbol_table;

#endif