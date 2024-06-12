#include "SymbolTable.hpp"

std::unordered_map<std::string, int> SymbolTable::symbol_table  = {
    {"EOF", -1},
    {"+", 0},
    {"-", 1},
    {"*", 2},
    {"/", 3},
    {";", 4},
    {"var", 5}, 
    {"let", 6}, 
    {"<-", 7}, 
    {">", 8}, 
    {"<", 9},  
    {"==", 10}, 
    {">=", 11}, 
    {"<=", 12}, 
    {"(", 13}, 
    {")", 14},
    {"{", 15}, 
    {"}", 16},  
    {",", 17}, 
    {"if", 18}, 
    {"then", 19}, 
    {"else", 20}, 
    {"fi", 21},
    {"while", 22}, 
    {"do", 23}, 
    {"od", 24}, 
    {"call", 25}, 
    {"function", 26},
    {"void", 27}, 
    {"return", 28}, 
    {"main", 29},
    {".", 30}
};

void print_table() {
    std::cout << "Symbol Table;" << std::endl << "[KEYWORD/TERMINAL]: [INT_VAL]" << std::endl;
    for (const auto &pair : SymbolTable::symbol_table) {
        std::cout << "[" << pair.first << "]" << ": [" << pair.second << "]" << std::endl;
    }
}

