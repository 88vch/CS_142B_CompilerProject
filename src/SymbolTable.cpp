#include "SymbolTable.hpp"

void SymbolTable::update_table(std::string s) {
    int id = symbol_table.size();
    symbol_table.emplace(s, id);
    identifiers.push_back(id);
}

std::vector<int> SymbolTable::identifiers = {};
std::vector<int> SymbolTable::numbers = {};

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

std::unordered_map<std::string, int> SymbolTable::operator_table = {
    {"const", 0},       // special func
    {"add", 1},
    {"sub", 2},
    {"mul", 3},
    {"div", 4},
    {"cmp", 5},
    {"phi", 6},         // special func
    {"end", 7},         // special func
    {"bra", 8},
    {"bne", 9},
    {"beq", 10},
    {"ble", 11},
    {"blt", 12},
    {"bge", 13},
    {"bgt", 14},
    // user-defined functions (below):
    {"jsr", 15},
    {"ret", 16},
    {"getpar1", 17},
    {"getpar2", 18},
    {"getpar3", 19},
    {"setpar1", 20},
    {"setpar2", 21},
    {"setpar3", 22},
    // modeling built-in i/o (below):
    {"read", 23},
    {"write", 24},
    {"writeNL", 25}
}

void SymbolTable::print_table() {
    std::cout << "Symbol Table;" << std::endl << "[KEYWORD/TERMINAL]: \t[INT_VAL]" << std::endl;
    for (const auto &pair : SymbolTable::symbol_table) {
        if ((pair.first).length() > 3) {
            std::cout << "[" << pair.first << "]" << ": \t\t[" << pair.second << "]" << std::endl;
        } else {
            std::cout << "[" << pair.first << "]" << ": \t\t\t[" << pair.second << "]" << std::endl;
        }
    }
}
