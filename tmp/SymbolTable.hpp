#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP


#include "Compiler.hpp"
#include <iostream>
#include <string.h>
#include <unordered_map>

// a hashtable for identifiers
class SymbolTable {
public:
    SymbolTable() {}
private:
    std::unordered_map<int, int>hashTable();
};


#endif