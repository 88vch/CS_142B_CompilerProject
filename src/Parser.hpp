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

enum FUNCTION_TYPE {
    VOID,
    NON_VOID
};

// abstract syntax tree (AST)
union {
    node_ident *ident;
    node_num *num;
    node_expr *expr;
    node_funcCall *funcCall;
} *node_factor;

struct node_term {
    node_factor *factorA;
    node_op *op; // [*, /]
    node_term *next; // iff [op] != nullptr, then should a [next] exist! (with it's [factorA] being the [factorA] of the prior [node_term])
    // node_term *prev; [not sure if needed; js in case]
};

struct node_expr {
    node_term *termA;
    node_op *op; // [+, -]
    node_expr *next; // iff [op] != nullptr, then should a [next] exist! (with it's [termA] being the [termB] of the prior [node_expr])
    // node_expr *prev; [not sure if needed; js in case]
};

struct node_relation {
    node_expr *exprA;
    node_relOp *relOp;
    node_expr *exprB;
};

struct node_assignment {
    node_ident *ident;
    node_expr *expr;
};

struct node_funcCall {
    node_ident *ident;
    node_expr *head; // this will be a linked list (bc multiple statements can exist in a single statSeq)
};

struct node_ifStat {
    node_relation *relation;
    node_statSeq *then_statSeq;
    node_statSeq *else_statSeq;
};

struct node_whileStat {
    node_relation *relation;
    node_statSeq *statSeq;
};

struct node_ret {
    node_expr *expr; // optional!
};

struct node_statement {
    union {
        node_assignment *assignment;
        node_funcCall *funcCall;
        node_ifStat *ifStat;
        node_whileStat *whileStat;
        node_ret *ret;
    } *data;
    node_statement *next;
    // node_statement *prev; [not sure if needed; js in case]
};

struct node_statSeq{ 
    node_statement *head; // this will be a linked list (bc multiple statements can exist in a single statSeq)
} ;

struct node_varDecl { // Q: no need to add "var" bc we know it by it's struct type right?
    node_varIdent *head; // this will be a linked list (bc multiple [varIdent] can exist in a single [varDecl])
};

struct node_funcDecl { // Q: no need to add "function" bc we know it by it's struct type right?
    FUNCTION_TYPE retType;
    std::string *retType_name_str; // include [name] in here
    node_formalParams *head; // this will be a linked list (bc multiple [funcParam] can exist in a single [funcDecl])
    node_funcBody *funcBody;
};

struct node_formalParams {
    node_ident *ident;
    node_formalParams *next;
    // node_formalParams *prev; [not sure if needed; js in case]
};

struct node_funcBody {
    node_varDecl *varDecl;
    node_statSeq *statSeq;
};

struct node_computation { // Q: no need to add "main" bc we know it by it's struct type right?
    node_varDecl *varDecl;
    node_funcDecl *funcDecl;
    node_statSeq *statSeq;
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
