#include <iostream>
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>

#include "src/SymbolTable.hpp"
#include "src/FileReader.hpp"
#include "src/Tokenizer.hpp"
#include "src/Parser.hpp"
// #include "src/Node.hpp"
// #include "src/Lexer.hpp"

class SymbolTable; // forward declaration (?)

// input file: [tst/temp.ty]
// input file: [tst/Lexer_results.txt]
int main() {
    const char *in_f = "tst/temp.ty";
    const std::string out_f = "tst/Lexer_results.txt";

    FileReader fr = FileReader(in_f);
    fr.read_file();
    std::string contents = fr.get_inFile_contents();

    // Tokenizer will be here
    Tokenizer tokenizer = Tokenizer(contents);
    std::vector<int> tokens = tokenizer.tokenize();
    // #ifdef DEBUG
    //     std::cout << "after tokenizer tokenize(), [tokens] size = [" << tokens.size() << "]" << std::endl;
    // #endif

    // OLD: Lexer, NEW: Parser
    // Lexer lexer = Lexer(contents);
    // std::vector<TOKEN> tokens = lexer.lex();
    Parser parser = Parser(tokens);
    parser.parse();
    std::cout << "done parsing" << std::endl;
    // node::computation *root = parser.head();
    // std::vector<TOKEN> tokens = parser.parse(); [should return a parse tree!]

    std::string out_str = "Syntax;\n[TOKEN_TYPE::DIGIT]: \t[STRING]\n";
    // // for (auto iit = tokens.begin(); iit != tokens.end(); iit++) {
    // //     std::string tmp;
    // //     if (TOKEN_TYPE_toString(iit->type).length() <= 3) { tmp = "[" + TOKEN_TYPE_toString(iit->type) + "]: \t\t\t[" + iit->lexeme + "]\n"; }
    // //     else if (TOKEN_TYPE_toString(iit->type).length() <= 6) { tmp = "[" + TOKEN_TYPE_toString(iit->type) + "]: \t\t[" + iit->lexeme + "]\n"; }
    // //     else { tmp = "[" + TOKEN_TYPE_toString(iit->type) + "]: \t[" + iit->lexeme + "]\n"; }
    // //     out_str += tmp;
    // // }
    for (int token : tokens) {
        std::string tmp;
        // Access each integer token here using the variable 'token'
        // For example:
        std::string val = "";
        for (const auto& pair : SymbolTable::symbol_table) {
            if (pair.second == token) {
                val = pair.first;
                break;
            }
        }
        if (val == "") {
            // digit
            tmp = "[DIGIT::NULL]: \t\t\t[" + std::to_string(token) + "]\n";
        } else {
            if (token < tokenizer.keyword_identifier_separator) {
                // keyword
                tmp = "[KEYWORD::" + std::to_string(token) + "]: \t\t\t[" + val + "]\n";
            } else {
                // identifier
                tmp = "[IDENTIFIER::" + std::to_string(token) + "]: \t\t[" + val + "]\n";
            }
        }
        out_str += tmp;
    }

    bool res = fr.write_file_contents(out_f, out_str);
    if (res) { std::cout << "Results have successfully been written to: " << out_f << std::endl; }
    else { std::cout << "Error occured when trying to write results!" << std::endl; }

    return 0;
}