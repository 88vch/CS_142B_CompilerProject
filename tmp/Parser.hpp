#ifndef PARSER_HPP
#define PARSER_HPP


// #include "Compiler.hpp"
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <vector>
#include <sstream>
#include <stack>

#include "Token.hpp"
#include "Node.hpp"

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
    Parser(const std::vector<TOKEN>& in) 
        : source(std::move(in))//, currentPos(0) 
    {
        this->source_len = this->source.size();
    }


    void parse_FIRSTPASS(); 

    // OLD STUFF
    // Get intermediate representation (IR)
    std::vector<IRNode> getIR();

private:
    size_t source_len, s_index = 0;
    const std::vector<TOKEN> source;
    TOKEN *curr; // current token we're working with
    node::computation *root;
    
    
    // OLD STUFF
    // size_t currentPos;
    std::unordered_map<std::string, std::string> symbolTable; // For copy propagation
    std::vector<IRNode> ir; // Intermediate representation (IR)


    // helper function modeled on: Lexer.hpp next() & consume()
    // does NOT consume char, only peeks()
    inline TOKEN* next() const {
        if (this->s_index < this->source_len) {
            const TOKEN *ret = &(source.at(s_index));
            return const_cast<TOKEN *>(ret); 
        }
        // else: EOF
        // OG: return NULL;
        // Revision: we should use pointers
        // - replace w (char *) and return nullptr
        return nullptr;
    }
    inline TOKEN consume() noexcept { return source.at(s_index++); }

    node::statSeq* parse_statSeq();
    node::statement* parse_statement();
    node::assignment* parse_assignment();
    node::funcCall* parse_funcCall();
    node::ifStat* parse_ifStatement();
    node::whileStat* parse_whileStatement();
    node::returnStat* parse_return();

    node::expr* parse_expr();
    node::term* parse_term();

    node::funcDecl* parse_funcDecl();
    
    node::varDecl* parse_varDecl();
    node::var* parse_vars();
    
    // node::ident* parse_ident();



    // OLD STUFF
    // Statement parsing functions
    void parseStatement();
    // Declaration parsing function
    void parseDeclaration();
    // Assignment parsing function
    void parseAssignment();
    // Print statement parsing function
    void parsePrintStatement();
    // Expression parsing function
    std::string parseExpression();
    // Term parsing function
    std::string parseTerm();
    // Factor parsing function
    std::string parseFactor();
    // Helper function to generate IR for binary operations
    std::string generateIRBinaryOperation(const std::string& left, const std::string& right, TOKEN_TYPE op);
    // Helper function to perform copy propagation
    std::string propagateCopy(const std::string& result);
    // Helper function to get string representation of token
    std::string getTokenString(TOKEN_TYPE type);
};


#endif
