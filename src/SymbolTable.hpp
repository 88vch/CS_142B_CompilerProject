#include "Compiler.hpp"
#include <iostream>
#include <string.h>
#include <unordered_map>

// a hashtable for identifiers
class SymbolTable {
public:
    SymbolTable() : {
        this->hashTable = new std::unordered_map<int, int>();
    }
private:
    std::unordered_map<int, int>hashTable;
};