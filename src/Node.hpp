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
    number, letter
    keywords ex: [if | while | return | let | var | main | call | return | etc... ]
]
*/
enum n_type {
    FACTOR,
    TERM,
    EXPRESSION,
    RELATION,
    ASSIGNMENT,
    FUNCTION_CALL,
    IF_STATEMENT,
    WHILE_STATEMENT,
    RETURN_STATEMENT,
    STATEMENT,
    STAT_SEQUENCE,
    VARIABLE_DECLARATION,
    FUNCTION_DECLARATION,
    FORMAL_PARAMETERS,
    FUNCTION_BODY,
    MAIN
};

std::unordered_set<n_type> terminal_nodes = {
    
};
std::unordered_set<n_type> non_terminal_nodes = {
    
};


enum statement_data {
    ASSIGNMENT_S,
    FUNCCALL_S,
    IFSTAT_S,
    WHILESTAT_S,
    RETURNSTAT_S
};
enum factor_data {
    IDENT_F,
    NUM_F,
    EXPR_F,
    FUNCCALL_F
};

namespace node {
    struct factor;
    struct term;
    struct expr;
    struct relation;
    struct assignment;
    struct funcCall;
    struct ifStat;
    struct whileStat;
    struct returnStat;
    struct statement;
    struct statSeq;
    struct var;
    struct varDecl;
    struct funcDecl;
    struct formalParams;
    struct funcBody;
    struct main;


    struct factor {
        factor_data data_type;
        union {
            int ident;
            int num;
            struct factor_expr {
                
                expr *e;
                
            } *expr;
            funcCall *funcCall;
        } *data;
    };

    struct term {
        factor *A;
        int op; // [*, /]
        union {
            factor *f;
            term *t;
        } *B;
        
        term() {
            A = nullptr;
            op = -1;
            B = nullptr;
        }
    };

    struct expr {
        term *A; // in fact, this will be the last [term] (in a series of consecutive expr's)
        int op; // [+, -]
        union {
            expr *e; // iff [op] != nullptr, then should a [next] exist! (with it's [termA] being the [termB] of the prior [expr])
            term *t;
        } *B;

        expr() {
            A = nullptr;
            op = -1;
            B = nullptr;
        }
    };

    struct relation {
        expr *exprA;
        int relOp;
        expr *exprB;

        relation() {
            exprA = nullptr;
            relOp = -1;
            exprB = nullptr;
        }
    };

    struct assignment {
        int ident;
        expr *expr;

        assignment() {
            ident = -1;
            expr = nullptr;
        }
    };

    struct funcCall {
        int ident;
        expr *head; // this will be a linked list (bc multiple statements can exist in a single statSeq)

        funcCall() {
            ident = -1;
            head = nullptr;
        }
    };

    struct ifStat {
        relation *relation;
        statSeq *then_statSeq;
        statSeq *else_statSeq;
        
        ifStat() {
            relation = nullptr;
            then_statSeq = nullptr;
            else_statSeq = nullptr;
        }
    };

    struct whileStat {
        relation *relation;
        statSeq *do_statSeq;
        
        whileStat() {
            relation = nullptr;
            do_statSeq = nullptr;
        }
    };

    struct returnStat {
        expr *expr; // optional!

        returnStat() { expr = nullptr; }
    };

    struct statement {
        statement_data type;
        union {
            assignment *assignment_S;
            funcCall *funcCall_S;
            ifStat *ifStat_S;
            whileStat *whileStat_S;
            returnStat *ret_S;
        } *data;
        statement *next;

        statement() {
            data = nullptr;
            next = nullptr;
        }
    };

    struct statSeq { 
        statement *head;

        statSeq() { head = nullptr; }
    };

    struct var { // Q: no need to add "var" bc we know it by it's struct type right?
        int ident;
        var *next;

        var() {
            ident = -1;
            next = nullptr;
        }
    };

    struct varDecl { // Q: no need to add "var" bc we know it by it's struct type right?
        var *head;
        
        varDecl() { head = nullptr; }
    };

    struct funcDecl { // Q: no need to add "function" bc we know it by it's struct type right?
        // include [name] in here
        // int retType; // (?)
        formalParams *head; // this will be a linked list (bc multiple [funcParam] can exist in a single [funcDecl])
        funcBody *funcBody;

        funcDecl() {
            head = nullptr;
            funcBody = nullptr;
        }
    };

    struct formalParams {
        int ident;
        formalParams *next;
        // formalParams *prev; [not sure if needed; js in case]

        formalParams() {
            ident = -1;
            next = nullptr;
        }
    };

    struct funcBody {
        varDecl *varDecl;
        statSeq *statSeq;

        funcBody() {
            varDecl = nullptr;
            statSeq = nullptr;
        }
    };

    struct main { // Q: no need to add "main" bc we know it by it's struct type right?
        varDecl *varDecl;
        funcDecl *funcDecl;
        statSeq *statSeq;

        main() {
            varDecl = nullptr;
            funcDecl = nullptr;
            statSeq = nullptr;
        }
    };
}


#endif