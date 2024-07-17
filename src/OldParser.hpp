#ifndef OLD_PARSER_HPP
#define OLD_PARSER_HPP


// #include "Compiler.hpp"
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <vector>
#include <sstream>
#include <stack>

// #include "Token.hpp"
// #include "Node.hpp"
#include "SymbolTable.hpp"
#include "TinyExceptions.hpp"
#include "SSA.hpp"
#include "BasicBlock.hpp"
#include "LinkedList.hpp"

#define DEBUG
// [ST]: Start Tokens
// [T]: Tokens
#define FUNCTION_ST_SIZE 2
#define STATEMENT_ST_SIZE 5
#define EXPRESSION_OP_SIZE 2
#define TERM_OP_SIZE 2
#define RELATIONAL_OP_SIZE 5

// class SymbolTable; // forward declaration (?)

// IR Node struct
struct IRNode {
    std::string result;
    std::string operation;
    std::string operand1;
    std::string operand2;
};

// Perform common subexpression elimination (CSE)
void performCSE(std::vector<IRNode>& ir);
// Print IR
void printIR(const std::vector<IRNode>& ir);

// to store values of registers needed [used for graph coloring later]
// std::unordered_set<int, int> REGISTERS;

// Parser class
class Parser {
public:
    Parser(const std::vector<int> &in) 
        : source(std::move(in))//, currentPos(0) 
    {
        this->source_len = this->source.size();
        next(); // load first int into [sym]
        this->varDeclarations = {};
        this->funcDeclarations = {};
        this->instruction_list = {};
        for (const auto& pair : SymbolTable::operator_table) {
            LinkedList L;
            this->instruction_list.insert({pair.second, L});
        }
    }
    // ~Parser() {
    //     for (const auto& pair : this->instruction_list) {
    //         LinkedList L = pair.second;
    //         delete L;
    //     }
    // }

    inline void add_ssa_entry(SSA instr) {
        LinkedList curr = this->instruction_list.at(instr.get_operator());
        curr.InsertAtHead(instr);
    }

    void parse(); 
    // node::main* head() { return this->root; }
    BasicBlock* head() { return this->root; }


    // OLD STUFF
    // Get intermediate representation (IR)
    std::vector<IRNode> getIR();


    int sym;
    std::unordered_set<int> varDeclarations; // the int in the symbol table
    std::unordered_set<int> funcDeclarations;
    std::unordered_map<int, LinkedList> instruction_list; // the global instruction list from which we take and make local versions of (per basic block)
private:
    size_t source_len, s_index = 0;
    const std::vector<int> source; // which can be translated (as needed) to a digit or to grab the identifier/keyword/terminal associated in the SymbolTable::symbol_table
    BasicBlock *root; // BB0


    // START TOKENS
    // int func_startTokens[] = {
    std::vector<int> func_startTokens = {{
        SymbolTable::symbol_table.at("function"), 
        SymbolTable::symbol_table.at("void")
    }};
    std::vector<int> statement_startTokens = {{
        SymbolTable::symbol_table.at("let"),
        SymbolTable::symbol_table.at("call"),
        SymbolTable::symbol_table.at("if"),
        SymbolTable::symbol_table.at("while"),
        SymbolTable::symbol_table.at("return")
    }};
    std::vector<int> expression_operations = {{
        SymbolTable::symbol_table.at("+"),
        SymbolTable::symbol_table.at("-")
    }};
    std::vector<int> term_operations = {{
        SymbolTable::symbol_table.at("*"),
        SymbolTable::symbol_table.at("/")
    }};
    std::vector<int> relational_operations = {{
        SymbolTable::symbol_table.at("<"),
        SymbolTable::symbol_table.at(">"),
        SymbolTable::symbol_table.at("<="),
        SymbolTable::symbol_table.at(">="),
        SymbolTable::symbol_table.at("==")
    }};
    static constexpr int relational_operations_arr[5] = { 9, 8, 12, 11, 10 }; // the literal values taken from sym_table directly 
    
    void CheckFor(int expected_token) {
        if (this->sym != expected_token) {
            std::stringstream ss;
            ss << "Expected: " << expected_token << ". Got: " << this->sym << "." << std::endl;
            tinyExceptions_ns::SyntaxError(ss.str());
        } else { next(); }
    }
    bool CheckForOptional(int expected_token) {
        if (this->sym != expected_token) {
            return false;
        }
        return true;
    }
    bool CheckForIdentifier(int v = -1) {
        if (v == -1) {
            for (const auto &it: SymbolTable::identifiers) {
                if (this->sym == it.second) {
                    return true;
                }
            }
            return false;
        } else {
            for (const auto &it: SymbolTable::identifiers) {
                if (v == it.second) {
                    return true;
                }
            }
            return false;
        }
    }
    bool CheckForNumber(int v = -1) {
        if (v == -1) {
            for (const auto &it: SymbolTable::numbers) {
                if (this->sym == it.second) {
                    return true;
                }
            }
            return false;
        } else {
            for (const auto &it: SymbolTable::numbers) {
                if (v == it.second) {
                    return true;
                }
            }
            return false;
        }
    }
    // if we have optionals, we should check multiple symbols, since we throw a syntax error if we don't see the expected token
    void CheckForMultiple(std::vector<int> expected_tokens, int size) { 
        bool exists = false;
        std::string expected_tokens_str = "[";
        for (int i = 0; i < size; i++) {
            if (this->sym == expected_tokens.at(i)) {
                expected_tokens_str += std::to_string(expected_tokens[i]);
                exists = true;
                break;
            }
            expected_tokens_str += std::to_string(expected_tokens[i]) + ", ";
        }
        expected_tokens_str += "]";
        if (!exists) {
            std::stringstream ss;
            ss << "Expected: " << expected_tokens_str << ". Got: " << this->sym << "." << std::endl;
            tinyExceptions_ns::SyntaxError(ss.str());
        }
    }
    bool CheckForMultipleOptionals(std::vector<int> expected_tokens, int size) { // for optionals, don't throw error if dne
        bool exists = false;
        std::string expected_tokens_str = "[";
        for (int i = 0; i < size; i++) {
            if (this->sym == expected_tokens.at(i)) {
                expected_tokens_str += std::to_string(expected_tokens[i]);
                exists = true;
                break;
            }
            expected_tokens_str += std::to_string(expected_tokens[i]) + ", ";
        }
        expected_tokens_str += "]";
        return exists;
    }
    
    // OLD STUFF
    // size_t currentPos;
    std::unordered_map<std::string, std::string> symbolTable; // For copy propagation
    std::vector<IRNode> ir; // Intermediate representation (IR)


    // helper function modeled on: Lexer.hpp next() & consume()
    // peek & consume char
    inline void next() {
        if (this->s_index < this->source_len) {
            this->sym = this->source.at(this->s_index);
            this->s_index++;
        } else {
            this->sym = SymbolTable::symbol_table.at("EOF"); // what value to denote end???
        }
    }

    void parse_statSeq(BasicBlock *blk);
    void parse_statement(BasicBlock *blk);
    void parse_assignment(BasicBlock *blk);
    void parse_funcCall(BasicBlock *blk);
    BasicBlock* parse_ifStatement(BasicBlock *blk);
    void parse_whileStatement(BasicBlock *blk);
    void parse_return(BasicBlock *blk);
    void parse_relation(BasicBlock *blk);
    int parse_expr(BasicBlock *blk);
    int parse_term(BasicBlock *blk);
    int parse_factor(BasicBlock *blk);
    void parse_funcDecl();
    void parse_varDecl();
    void parse_vars(); // helper func



    // OLD STUFF [2]
    // regular grammar (should be in symbol_table)
    // node::ident* parse_ident();
    // node::num* parse_number();

    // node::factor* parse_factor();
    // node::term* parse_term();
    // node::expr* parse_expr();
    // node::relation* parse_relation();
    
    // node::assignment* parse_assignment();
    // node::funcCall* parse_funcCall();
    // node::ifStat* parse_ifStatement();
    // node::whileStat* parse_whileStatement();
    // node::returnStat* parse_return();

    // node::statSeq* parse_statSeq();
    // node::statement* parse_statement();
    
    // node::varDecl* parse_varDecl();
    // node::var* parse_vars();
    // node::funcDecl* parse_funcDecl();
    
    
    // OLD STUFF [1]
    // // Statement parsing functions
    // void parseStatement();
    // // Declaration parsing function
    // void parseDeclaration();
    // // Assignment parsing function
    // void parseAssignment();
    // // Print statement parsing function
    // void parsePrintStatement();
    // // Expression parsing function
    // std::string parseExpression();
    // // Term parsing function
    // std::string parseTerm();
    // // Factor parsing function
    // std::string parseFactor();
    // Helper function to generate IR for binary operations
    // std::string generateIRBinaryOperation(const std::string& left, const std::string& right, TOKEN_TYPE op);
    // Helper function to perform copy propagation
    std::string propagateCopy(const std::string& result);
    // Helper function to get string representation of token
    // std::string getTokenString(TOKEN_TYPE type);
};


#endif
