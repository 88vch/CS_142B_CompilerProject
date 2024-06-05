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

#include "Token.hpp"
#include "DLinkedList.hpp"

// Token struct
struct TOKEN {
    TOKEN_TYPE type;
    std::string lexeme;
};

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
