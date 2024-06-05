#ifndef PARSER_HPP
#define PARSER_HPP


#include "Compiler.hpp"
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <vector>
#include <sstream>
#include <stack>

#include "Node.hpp"
#include "Token.hpp"
#include "DLinkedList.hpp"

enum FUNCTION_TYPE {
    VOID,
    NON_VOID
};

/*
Rules to Draw a Parse Tree:
- All leaf nodes need to be terminals.
- All interior nodes need to be non-terminals.
- In-order traversal gives the original input string.

Given [std::vector<Token>] return the corresponding parse tree
*/

// // Perform common subexpression elimination (CSE)
// void performCSE(std::vector<node_pt>& ir);
// // Print IR
// void printIR(const std::vector<node_pt>& ir);

// to store values of registers needed [used for graph coloring later]
// std::unordered_set<int, int> REGISTERS;


class Parser {
public:
    Parser(const std::vector<TOKEN> &tokens) 
        : tokens(tokens)//, currentPos(0) 
    {
        // const_counter = -1;
        // instruction_counter = 1;

        this->tokens_len = tokens.size();
    }

    // Get intermediate representation (IR)
    // std::vector<IRNode> getIR();

    node::computation* parse(); // returns n_head of parse tree
    size_t tokens_len, s_index = 0;
    const std::vector<TOKEN> tokens;
private:
    // int const_counter, instruction_counter;
    
    // size_t currentPos;
    // TOKEN currentToken;
    // std::unordered_map<std::string, std::string> symbolTable; // For copy propagation
    // std::vector<IRNode> ir; // Intermediate representation (IR)

    // Helper function: does NOT consume char, only peeks() for next token
    inline TOKEN* next() const {
        if (this->s_index < this->tokens_len) {
            const TOKEN *ret = &(this->tokens.at(s_index));
            return const_cast<TOKEN *>(ret); 
        } // else: EOF
        // [OG]: return NULL;
        // [Revision]: we should use pointers; replace w (char *) and return nullptr
        return nullptr;
    }

    // Helper function: to consume current token and advance to next token
    inline TOKEN consume() noexcept {
        return this->tokens.at(s_index++);
    }

    node::var* parse_var();
    node::varDecl* parse_varDecl();
    node::funcDecl* parse_funcDecl();

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

    // // Helper function to generate IR for binary operations
    // std::string generateIRBinaryOperation(const std::string& left, const std::string& right, TOKEN_TYPE op);

    // // Helper function to perform copy propagation
    // std::string propagateCopy(const std::string& result);

    // // Helper function to get string representation of token
    // std::string getTokenString(TOKEN_TYPE type);
};


#endif
