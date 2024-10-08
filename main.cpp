#include <iostream>
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>

#include "src/SymbolTable.hpp"
#include "src/FileReader.hpp"
#include "src/Tokenizer.hpp"
#include "src/Parser.hpp"
#include "src/Result.hpp"
#include "src/SSA.hpp"
// #include "src/Node.hpp"
// #include "src/Lexer.hpp"

#define DEBUG

// class SymbolTable; // forward declaration (?)

#define in_f "tst/nested_if.ty"

int main() {
    std::string symbolTable_f = "res/_SymbolTable_result.txt";
    std::string identifiers_f = "res/_Identifiers_result.txt";
    std::string numbers_f = "res/_Numbers_result.txt";
    std::string tokenize_f = "res/Tokenizer_results.txt";
    std::string results_f = "res/Results_results.txt";
    std::string parser1_f = "res/Parser1_results.txt";
    std::string varVal_f = "res/VarVal_results.txt";
    std::string linkedList_f = "res/LinkedList_results.txt";


    const std::string out_f = "res/Lexer_results.txt";

    FileReader fr = FileReader(in_f);
    fr.read_file();
    std::string contents = fr.get_inFile_contents();
    #ifdef DEBUG
        std::cout << "[FileReader]: After get_inFile_contents(), [contents] size  [" << contents.size() << "]" << std::endl;
    #endif

    // Tokenizer will be here
    #ifdef DEBUG
        std::cout << "[Tokenizer]: Before constructor" << std::endl;
    #endif
    Tokenizer tokenizer = Tokenizer(contents);
    #ifdef DEBUG
        std::cout << "[Tokenizer]: After constructor. Before [tokenize()]" << std::endl;
    #endif
    std::vector<int> tokens = tokenizer.tokenize();
    #ifdef DEBUG
        std::cout << "[Tokenizer]: after tokenize(), [tokens] size = [" << tokens.size() << "]" << std::endl;
    #endif


    bool st = fr.write_file_contents(symbolTable_f, SymbolTable::symbol_table, "Symbol Table");
    if (st) { std::cout << "Results have successfully been written to: " << symbolTable_f << "(size=[" << SymbolTable::symbol_table.size() << "])" << std::endl; }
    else { std::cout << "Error occured when trying to write results!" << std::endl; }

    bool ids = fr.write_file_contents(identifiers_f, SymbolTable::identifiers, "Identifiers");
    if (ids) { std::cout << "Results have successfully been written to: " << identifiers_f << "(size=[" << SymbolTable::identifiers.size() << "])" << std::endl; }
    else { std::cout << "Error occured when trying to write results!" << std::endl; }

    bool nums = fr.write_file_contents(numbers_f, SymbolTable::numbers, "Numbers");
    if (nums) { std::cout << "Results have successfully been written to: " << numbers_f << "(size=[" << SymbolTable::numbers.size() << "])" << std::endl; }
    else { std::cout << "Error occured when trying to write results!" << std::endl; }
    
    bool toks = fr.write_file_contents(tokenize_f, tokens, "Tokenizer Tokens");
    if (toks) { std::cout << "Results have successfully been written to: " << tokenize_f << "(size=[" << tokens.size() << "])" << std::endl; }
    else { std::cout << "Error occured when trying to write results!" << std::endl; }

    std::vector<Result> results = Result::int_to_result(tokens);

    bool res = fr.write_file_contents(results_f, results, "Results");
    if (res) { std::cout << "Results have successfully been written to: " << results_f << "(size=[" << results.size() << "])" << std::endl; }
    else { std::cout << "Error occured when trying to write results!" << std::endl; }

    // OLD: Lexer, NEW: Parser
    // Lexer lexer = Lexer(contents);
    // std::vector<TOKEN> tokens = lexer.lex();
    #ifdef DEBUG
        std::cout << "[Parser]: Before constructor" << std::endl;
    #endif
    Parser parser(results);
    


    #ifdef DEBUG
        std::cout << "[Parser]: After constructor. Before FIRST [parse](SSA Generation)" << std::endl;
    #endif
    parser.parse_generate_SSA();
    #ifdef DEBUG
        std::cout << "[Parser]: After FIRST [parse]" << std::endl;
    #endif

    std::vector<SSA*> SSA_instrs = parser.getSSA();
    bool SSA_exists = fr.write_file_contents(parser1_f, SSA_instrs, "SSA Instructions");
    if (SSA_exists) { std::cout << "SSA Instructions have successfully been written to: " << parser1_f << "(size=[" << SSA_instrs.size() << "])" << std::endl; }
    else { std::cout << "Error occured when trying to write results!" << std::endl; }

    std::unordered_map<std::string, SSA*> varVals = parser.getVarVal();
    bool vv = fr.write_file_contents(varVal_f, varVals, "Var-Val");
    if (st) { std::cout << "Results have successfully been written to: " << varVal_f << "(size=[" << varVals.size() << "])" << std::endl; }
    else { std::cout << "Error occured when trying to write results!" << std::endl; }
    
    bool ll = fr.write_file_contents(linkedList_f, parser.instrListToString(), "LinkedList");
    if (ll) { std::cout << "Results have successfully been written to: " << linkedList_f << "(size=[" << parser.getInstrListSize() << "])" << std::endl; }
    else { std::cout << "Error occured when trying to write results!" << std::endl; }








    // node::computation *root = parser.head();
    // std::vector<TOKEN> tokens = parser.parse(); [should return a parse tree!]

    // std::string out_str = "Syntax;\n[TOKEN_TYPE::DIGIT]: \t[STRING]\n";
    // // // for (auto iit = tokens.begin(); iit != tokens.end(); iit++) {
    // // //     std::string tmp;
    // // //     if (TOKEN_TYPE_toString(iit->type).length() <= 3) { tmp = "[" + TOKEN_TYPE_toString(iit->type) + "]: \t\t\t[" + iit->lexeme + "]\n"; }
    // // //     else if (TOKEN_TYPE_toString(iit->type).length() <= 6) { tmp = "[" + TOKEN_TYPE_toString(iit->type) + "]: \t\t[" + iit->lexeme + "]\n"; }
    // // //     else { tmp = "[" + TOKEN_TYPE_toString(iit->type) + "]: \t[" + iit->lexeme + "]\n"; }
    // // //     out_str += tmp;
    // // // }
    // for (int token : tokens) {
    //     std::string tmp;
    //     // Access each integer token here using the variable 'token'
    //     // For example:
    //     std::string val = "";
    //     for (const auto& pair : SymbolTable::symbol_table) {
    //         if (pair.second == token) {
    //             val = pair.first;
    //             break;
    //         }
    //     }
    //     if (val == "") {
    //         // digit
    //         tmp = "[DIGIT::NULL]: \t\t\t[" + std::to_string(token) + "]\n";
    //     } else {
    //         if (token < tokenizer.keyword_identifier_separator) {
    //             // keyword
    //             tmp = "[KEYWORD::" + std::to_string(token) + "]: \t\t\t[" + val + "]\n";
    //         } else {
    //             // identifier
    //             tmp = "[IDENTIFIER::" + std::to_string(token) + "]: \t\t[" + val + "]\n";
    //         }
    //     }
    //     out_str += tmp;
    // }

    // bool res = fr.write_file_contents(out_f, out_str);
    // if (res) { std::cout << "Results have successfully been written to: " << out_f << std::endl; }
    // else { std::cout << "Error occured when trying to write results!" << std::endl; }

    return 0;
}