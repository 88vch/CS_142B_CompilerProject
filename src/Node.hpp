#ifndef NODE_HPP
#define NODE_HPP

#include <string>
#include "Token.hpp"

/*
so this should be for generating Abstract Syntax Tree (AST)???
- abstract syntax tree (AST)
*/

namespace node {
    struct op {
        union {
            TOKEN_TYPE::PLUS *p;
            TOKEN_TYPE::MINUS *m;
            TOKEN_TYPE::TIMES *t;
            TOKEN_TYPE::DIVIDE *d;
        } *t_type;
        char *data; // [op] should only be one char anyways
    };

    struct relOp { TOKEN *relOp_ROP; };

    struct ident { std::string val; };
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

    union statement {
        assignment *assignment_S;
        funcCall *funcCallS;
        ifStat *ifStat_S;
        whileStat *whileStat_S;
        ret *ret_S;
    };

    struct statSeq{ 
        statement *statement_SS; // this will be a linked list (bc multiple statements can exist in a single statSeq)
        statSeq *next;
        // statSeq *prev; [not sure if needed; js in case]
    } ;

    struct varDecl { // Q: no need to add "var" bc we know it by it's struct type right?
        ident *ident_VD; // this will be a linked list (bc multiple [varIdent] can exist in a single [varDecl])
        varDecl *next;
        // varDecl *prev; [not sure if needed; js in case]

    };

    struct funcDecl { // Q: no need to add "function" bc we know it by it's struct type right?
        FUNCTION_TYPE retType;
        std::string *retType_name_str; // include [name] in here
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
        varDecl *varDecl;
        funcDecl *funcDecl;
        statSeq *statSeq;
    };
}


#endif