#include <iostream>
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include <unistd.h>

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

// #define file_name "simple_instrs"

// std::string f_ext = ".ty";
// std::string f_name = file_name;
// std::string f_full = f_name + f_ext;
// std::string in_f = "tst/" + f_name + f_ext;
#define in_f "tst/simple_if3.ty"

// [10.23.2024]: Silly rabbit

// [10.21.2024];
// every assignment modifies a phi-function in the current join block.
// if no phi-function exists already, create one.

// Note: check if [
//     when we look for an existing SSA, 
//     we only check our existing instructions that dominate this block bc 
//     we only care about dominating instructions
//  ] (ensure that we're not checking the entire LL or SSA-instr's that aren't DOM this current one)

// [10.22.2024]: Still IP; why not just create the join-blk at the same time (i.e. before the if-else statSeq())?
// Note: join-blk's [instrList] should NOT contain the added SSA-instructions from the if & else statements!
// - bc neither one path dominates the join-blk

// [10.22.2024]: Removed
// Note: if we have no use for (SSA_instr->child ptr [in Parser::instrList(?)]), then we should delete it
// - seems like prof doesn't use it in his videos

// TODO;
// - also good to ensure that we're only checking DOM instructions!

// [10.22.2024]: Removed
// Note: maybe we don't need [head && tail] in [LinkedList]. maybe just [tail] is enough (?)

int main() {
    std::string symbolTable_f = "res/_SymbolTable_result.txt";
    std::string identifiers_f = "res/_Identifiers_result.txt";
    std::string numbers_f = "res/_Numbers_result.txt";
    std::string tokenize_f = "res/Tokenizer_results.txt";
    std::string results_f = "res/Results_results.txt";
    std::string parser1_f = "res/Parser1_results.txt";
    std::string parser2_f = "res/Parser2_results.txt";
    std::string varVal1_f = "res/VarVal1_results.txt";
    std::string varVal2_f = "res/VarVal2_results.txt";
    std::string linkedList1_f = "res/LinkedList1_results.txt";
    std::string linkedList2_f = "res/LinkedList2_results.txt";


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

    // [10.26.2024]: Parser1
    // #ifdef DEBUG
    //     std::cout << "[Parser]: Before constructor" << std::endl;
    // #endif
    // Parser parser1(results);
    
    // #ifdef DEBUG
    //     std::cout << "[Parser]: After constructor. Before FIRST [parse](SSA Generation)" << std::endl;
    // #endif
    // parser1.p_start();
    // // parser.parse();
    // #ifdef DEBUG
    //     std::cout << "[Parser]: After FIRST [parse.parse_generate_SSA()]" << std::endl;
    // #endif

    // std::vector<SSA*> SSA_instrs1 = parser1.getSSA();
    // bool SSA_exists1 = fr.write_file_contents(parser1_f, SSA_instrs1, "SSA Instructions");
    // if (SSA_exists1) { std::cout << "SSA Instructions have successfully been written to: " << parser1_f << "(size=[" << SSA_instrs1.size() << "])" << std::endl; }
    // else { std::cout << "Error occured when trying to write results!" << std::endl; }

    // std::unordered_map<std::string, SSA*> varVals1 = parser1.getVarVal();
    // bool vv1 = fr.write_file_contents(varVal1_f, varVals1, "Var-Val");
    // if (st) { std::cout << "Results have successfully been written to: " << varVal1_f << "(size=[" << varVals1.size() << "])" << std::endl; }
    // else { std::cout << "Error occured when trying to write results!" << std::endl; }
    
    // bool ll1 = fr.write_file_contents(linkedList1_f, parser1.instrListToString(), "LinkedList");
    // if (ll1) { std::cout << "Results have successfully been written to: " << linkedList1_f << "(size=[" << parser1.getInstrListSize() << "])" << std::endl; }
    // else { std::cout << "Error occured when trying to write results!" << std::endl; }

    // [10.26.2024]: Parser2
    #ifdef DEBUG
        std::cout << "[Parser]: Before SECOND [parse](BasicBlock Generation)" << std::endl;
    #endif
    SSA::resetDebug();
    Parser parser2(results, true);
    parser2.parse();
    #ifdef DEBUG
        std::cout << "[Parser]: After SECOND [parse.parse()]" << std::endl;
    #endif

    parser2.generateDOT();

    std::vector<SSA*> SSA_instrs2 = parser2.getSSA();
    bool SSA_exists2 = fr.write_file_contents(parser2_f, SSA_instrs2, "SSA Instructions");
    if (SSA_exists2) { std::cout << "SSA Instructions have successfully been written to: " << parser2_f << "(size=[" << SSA_instrs2.size() << "])" << std::endl; }
    else { std::cout << "Error occured when trying to write results!" << std::endl; }

    std::unordered_map<std::string, SSA*> varVals2 = parser2.getVarVal();
    bool vv2 = fr.write_file_contents(varVal2_f, varVals2, "Var-Val");
    if (st) { std::cout << "Results have successfully been written to: " << varVal2_f << "(size=[" << varVals2.size() << "])" << std::endl; }
    else { std::cout << "Error occured when trying to write results!" << std::endl; }
    
    bool ll2 = fr.write_file_contents(linkedList2_f, parser2.instrListToString(), "LinkedList");
    if (ll2) { std::cout << "Results have successfully been written to: " << linkedList2_f << "(size=[" << parser2.getInstrListSize() << "])" << std::endl; }
    else { std::cout << "Error occured when trying to write results!" << std::endl; }

    system("dot -Tpng res/DOT.dot -o DOT.png");

    return 0;
}