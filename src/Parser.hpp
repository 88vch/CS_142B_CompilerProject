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
// parse tree symbols: [terminal || non-terminal]
struct n_terminal {
    /* 
    the only terminals we have are: [
        number, 
        letter, 
        op, relOp, 
        semicolon, period,
        keywords ex: [if | while | return | let | var | main | call | return | etc... ]
    ]
    */
};

struct n_non_terminal {
    // could we js union all the nodes in the node class we created?

};

struct symbol_pt {
    bool n_type; // [0] = terminal, [1] = non-terminal
    union {
        n_terminal *token_t;
        n_non_terminal *token_nt;
    } *data;
};

// Perform common subexpression elimination (CSE)
void performCSE(std::vector<node_pt>& ir);
// Print IR
void printIR(const std::vector<node_pt>& ir);

// to store values of registers needed [used for graph coloring later]
// std::unordered_set<int, int> REGISTERS;

// Parser class
class Parser {
public:
    Parser(const std::vector<TOKEN> &tokens) 
        : tokens(tokens)//, currentPos(0) 
    {
        const_counter = -1;
        instruction_counter = 1;
    }


    void parse_FIRSTPASS(); 
    // Get intermediate representation (IR)
    std::vector<IRNode> getIR();

private:
    int const_counter, instruction_counter;
    std::vector<TOKEN> tokens;
    // size_t currentPos;
    TOKEN currentToken;
    std::unordered_map<std::string, std::string> symbolTable; // For copy propagation
    std::vector<IRNode> ir; // Intermediate representation (IR)

    // Helper function to get next token
    TOKEN getNextToken();

    // Helper function to consume current token and advance to next token
    void consume(TOKEN_TYPE expectedType);

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
