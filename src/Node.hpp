#ifndef NODE_HPP
#define NODE_HPP

#include <string>
#include <unordered_set>
#include "Token.hpp"

/*
so this should be for generating Abstract Syntax Tree (AST)???
- abstract syntax tree (AST)
*/
// parse tree symbols: [terminal || non-terminal]
// everything that is NOT in a [terminal] node will be considered [non-terminal]
/* 
the only terminals we have are: [
    number, letter, op, relOp, semicolon, period,
    keywords ex: [if | while | return | let | var | main | call | return | etc... ]
]
*/
struct terminal_n { std::string val; };

// struct n_non_terminal {
//     // could we js union all the nodes in the node class we created?    
// };

// struct n_parseTree {
//     bool n_type; // [0] = terminal, [1] = non-terminal
//     union {
//         n_terminal *token_t;
//         n_non_terminal *token_nt;
//     } *data;
// };
const std::unordered_set<int> statement_start_KEYWORDS {
    TOKEN_TYPE::LET,
    TOKEN_TYPE::CALL,
    TOKEN_TYPE::IF,
    TOKEN_TYPE::WHILE,
    TOKEN_TYPE::RETURN
};

enum OP_TYPE {
    PLUS=TOKEN_TYPE::PLUS,
    MINUS=TOKEN_TYPE::MINUS,
    TIMES=TOKEN_TYPE::TIMES,
    DIVIDE=TOKEN_TYPE::DIVIDE
};

// enum ROP_TYPE {
//     PLUS=TOKEN_TYPE::PLUS,
//     MINUS=TOKEN_TYPE::MINUS,
//     TIMES=TOKEN_TYPE::TIMES,
//     DIVIDE=TOKEN_TYPE::DIVIDE
// };

namespace node {
    struct op {
        OP_TYPE t_type;
        std::string data; // [op] should only be one char; but js to keep in line w def's [relOp]
    };

    struct relOp { 
        // ROP_TYPE t_type; [not sure what to do since we've only declared one relOp [TOKEN_TYPE]]
        std::string data;
    };

    struct ident { terminal_n val };
    struct num { int val; };

    union factor {
        ident *ident;
        num *num;
        expr *expr;
        funcCall *funcCall;
    };

    struct term {
        factor *factorA;
        op *op; // [*, /]
        term *next; // iff [op] != nullptr, then should a [next] exist! (with it's [factorA] being the [factorA] of the prior [term])
        // term *prev; [not sure if needed; js in case]
    };

    struct expr {
        term *termA;
        op *op; // [+, -]
        expr *next; // iff [op] != nullptr, then should a [next] exist! (with it's [termA] being the [termB] of the prior [expr])
        // expr *prev; [not sure if needed; js in case]
    };

    struct relation {
        expr *exprA;
        relOp *relOp;
        expr *exprB;
    };

    struct assignment {
        ident *ident;
        expr *expr;
    };

    struct funcCall {
        ident *ident;
        expr *head; // this will be a linked list (bc multiple statements can exist in a single statSeq)
    };

    struct ifStat {
        relation *relation;
        statSeq *then_statSeq;
        statSeq *else_statSeq;
    };

    struct whileStat {
        relation *relation;
        statSeq *statSeq;
    };

    struct ret {
        expr *expr; // optional!
    };

    struct statement {
        union {
            assignment *assignment_S;
            funcCall *funcCall_S;
            ifStat *ifStat_S;
            whileStat *whileStat_S;
            ret *ret_S;
        } *data;
        terminal_n *terminal_sym_semi;
        statement *next;
        statement *prev;
    };

    struct statSeq { 
        statement *head;
    };

    struct var { // Q: no need to add "var" bc we know it by it's struct type right?
        ident *ident_VD; // this will be a linked list (bc multiple [varIdent] can exist in a single [varDecl])
        terminal_n *terminal_sym_comma;
        varDecl *next;
        // varDecl *prev; [not sure if needed; js in case]
    };


    struct varDecl { // Q: no need to add "var" bc we know it by it's struct type right?
        terminal_n *terminal_sym_var;
        var *head;
        // varDecl *prev; [not sure if needed; js in case]
    };

    struct funcDecl { // Q: no need to add "function" bc we know it by it's struct type right?
        terminal_n *terminal_sym_retType;
        terminal_n *terminal_sym_func;
        terminal_n *terminal_sym_name; // include [name] in here
        formalParams *head; // this will be a linked list (bc multiple [funcParam] can exist in a single [funcDecl])
        funcBody *funcBody;
    };

    struct formalParams {
        ident *ident;
        formalParams *next;
        // formalParams *prev; [not sure if needed; js in case]
    };

    struct funcBody {
        varDecl *varDecl;
        statSeq *statSeq;
    };

    struct computation { // Q: no need to add "main" bc we know it by it's struct type right?
        terminal_n *terminal_sym_main;
        varDecl *varDecl;
        funcDecl *funcDecl;
        statSeq *statSeq;
        terminal_n *terminal_sym_eof;
    };
}


#endif