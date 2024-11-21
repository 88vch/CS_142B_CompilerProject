#include "SymbolTable.hpp"

namespace SymbolTable {
    // std::vector<int> identifiers = {};
    // std::vector<int> numbers = {};

    const std::unordered_map<std::string, int> keywords = {
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

    // holds symbol_table key's that are identifiers [identifier: literal = std::string, value = int]
    std::unordered_map<std::string, int> identifiers = {};  

    // holds symbol_table key's that are numbers [number: literal = std::string, value = int]
    std::unordered_map<std::string, int> numbers = {};

    // should make int=key & std::string=value
    std::unordered_map<int, std::string> symbol_table  = { // find() from [symbol_table]
        {-1, "EOF"},
        {0, "+"},
        {1, "-"},
        {2, "*"},
        {3, "/"},
        {4, ";"},
        {5, "var"}, 
        {6, "let"}, 
        {7, "<-"}, 
        {8, ">"}, 
        {9, "<"},  
        {10, "=="}, 
        {11, ">="}, 
        {12, "<="}, 
        {13, "("}, 
        {14, ")"},
        {15, "{"}, 
        {16, "}"},  
        {17, ","}, 
        {18, "if"}, 
        {19, "then"}, 
        {20, "else"}, 
        {21, "fi"},
        {22, "while"}, 
        {23, "do"}, 
        {24, "od"}, 
        {25, "call"}, 
        {26, "function"},
        {27, "void"}, 
        {28, "return"}, 
        {29, "main"},
        {30, "."}
    };
    
    // std::unordered_map<std::string, int> symbol_table  = { // find() from [symbol_table]
    //     {"EOF", -1},
    //     {"+", 0},
    //     {"-", 1},
    //     {"*", 2},
    //     {"/", 3},
    //     {";", 4},
    //     {"var", 5}, 
    //     {"let", 6}, 
    //     {"<-", 7}, 
    //     {">", 8}, 
    //     {"<", 9},  
    //     {"==", 10}, 
    //     {">=", 11}, 
    //     {"<=", 12}, 
    //     {"(", 13}, 
    //     {")", 14},
    //     {"{", 15}, 
    //     {"}", 16},  
    //     {",", 17}, 
    //     {"if", 18}, 
    //     {"then", 19}, 
    //     {"else", 20}, 
    //     {"fi", 21},
    //     {"while", 22}, 
    //     {"do", 23}, 
    //     {"od", 24}, 
    //     {"call", 25}, 
    //     {"function", 26},
    //     {"void", 27}, 
    //     {"return", 28}, 
    //     {"main", 29},
    //     {".", 30}
    // };
    
    std::unordered_map<std::string, int> operator_table = { // translate into operator from [operator_table]
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
        // user-defined functions (below): FIRST GET ABOVE WORKING
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
    };

    std::unordered_map<int, std::string> operator_table_reversed = { // translate into operator from [operator_table]
        {0, "const"},       // special func
        {1, "add"},
        {2, "sub"},
        {3, "mul"},
        {4, "div"},
        {5, "cmp"},
        {6, "phi"},         // special func
        {7, "end"},         // special func
        {8, "bra"},
        {9, "bne"},
        {10, "beq"},
        {11, "ble"},
        {12, "blt"},
        {13, "bge"},
        {14, "bgt"},
        // user-defined functions (below): FIRST GET ABOVE WORKING
        {15, "jsr"},
        {16, "ret"},
        {17, "getpar1"},
        {18, "getpar2"},
        {19, "getpar3"},
        {20, "setpar1"},
        {21, "setpar2"},
        {22, "setpar3"},
        // modeling built-in i/o (below):
        {23, "read"},
        {24, "write"},
        {25, "writeNL"}
    };

    void update_table(std::string s, std::string type) {
        int id = symbol_table.size() - 1;
        // symbol_table.emplace(s, id);
        symbol_table.emplace(id, s);

        if (type == "identifier") {
            // identifiers.push_back(id);

            /*
                ToDo a1: do we need to [check] & [differentiate]
                    between the same variable if it contains different values?
                        - if so, we need to check and do this in [Tokenizer], 
                            since that's where we check for existence
            */
            

            identifiers.emplace(s, id);
        } else if (type == "number") {
            numbers.emplace(s, id);
        }
    }

    void print_symbol_table() {
        std::cout << "Symbol Table;" << std::endl << "[KEYWORD/TERMINAL]: \t[INT_VAL]" << std::endl;
        for (const auto &pair : symbol_table) {
            // if ((pair.first).length() > 3) {
            //     std::cout << "[" << pair.first << "]" << ": \t\t[" << pair.second << "]" << std::endl;
            // } else {
            //     std::cout << "[" << pair.first << "]" << ": \t\t\t[" << pair.second << "]" << std::endl;
            // }
            std::cout << "[" << pair.first << "]" << ": \t\t\t[" << pair.second << "]" << std::endl;
        }
    }

    void print_identifiers_table() {
        std::cout << "Identifier Table;" << std::endl << "[i-th IDENT]: \t[INT_VAL]" << std::endl;
        // for (size_t i = 0; i < identifiers.size(); i++) {
        //     std::cout << "[" << i << "]" << ": \t\t\t[" << identifiers.at(i) << "]" << std::endl;
        // }
        
        // int i = 0;
        for (const auto &c : identifiers) {
            std::cout << "[" << c.first << "]" << ": \t\t\t[" << c.second << "]" << std::endl;
            // i++;
        }
    }

    void print_number_table() {
        std::cout << "Number Table;" << std::endl << "[i-th NUM]: \t[INT_VAL]" << std::endl;
        // for (size_t i = 0; i < numbers.size(); i++) {
        //     std::cout << "[" << i << "]" << ": \t\t\t[" << numbers.at(i) << "]" << std::endl;
        // }

        // int i = 0;
        for (const auto &c : numbers) {
            std::cout << "[" << c.first << "]" << ": \t\t\t[" << c.second << "]" << std::endl;
            // i++;
        }
    }
};