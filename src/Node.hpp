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

enum statement_data {
    ASSIGNMENT_S,
    FUNCCALL_S,
    IFSTAT_S,
    WHILESTAT_S,
    RETURNSTAT_S
};
enum term_data {
    IDENT_T,
    NUM_T,
    EXPR_T,
    FUNCCALL_T
};

namespace node {
    struct ident { 
        int val;
        ident(int curr)
            : val(curr) {}
    };
    struct num { 
        int val;
        num(int curr)
            : val(curr) {}
    };

    

    struct term {
        union {
        int ident;
        num *num;
        expr *expr;
        funcCall *funcCall;
        } *factor_A;
        int op; // [*, /]

        // NEW
        union {
        int ident;
        num *num;
        expr *expr;
        funcCall *funcCall;
        } *factor_B;
        // OLD
        // term *next; // iff [op] != nullptr, then should a [next] exist! (with it's [factorA] being the [factorA] of the prior [term])
        // // term *prev; [not sure if needed; js in case]

        term() {
            factor_A = nullptr;
            op = nullptr;
            factor_B = nullptr;
        }
    };

    struct expr {
        union {
            expr *e; // iff [op] != nullptr, then should a [next] exist! (with it's [termA] being the [termB] of the prior [expr])
            term *t;
        } *term_A; // in fact, this will be the last [term] (in a series of consecutive expr's)
        int op; // [+, -]
        union {
            expr *e; // iff [op] != nullptr, then should a [next] exist! (with it's [termA] being the [termB] of the prior [expr])
            term *t;
        } *term_B;

        expr() {
            term_A = nullptr;
            op = nullptr;
            term_B = nullptr;
        }
    };

    struct relation {
        expr *exprA;
        int relOp;
        expr *exprB;
    };

    struct assignment {
        int terminal_sym_let;
        int ident;
        int terminal_sym_assignment;
        expr *expr;
    };

    struct funcCall {
        int terminal_sym_call;
        int ident;
        expr *head; // this will be a linked list (bc multiple statements can exist in a single statSeq)
    };

    struct ifStat {
        int terminal_sym_if;
        relation *relation;
        int terminal_sym_then;
        statSeq *then_statSeq;
        int terminal_sym_else;
        statSeq *else_statSeq;
        int terminal_sym_fi;
    };

    struct whileStat {
        int terminal_sym_while;
        relation *relation;
        int terminal_sym_do;
        statSeq *statSeq;
        int terminal_sym_od;
    };

    struct returnStat {
        int terminal_sym_return;
        expr *expr; // optional!
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
        int terminal_sym_semi;
        statement *next;
    };

    struct statSeq { 
        statement *head;

        statSeq() { head = nullptr; }
    };

    struct var { // Q: no need to add "var" bc we know it by it's struct type right?
        int ident;
        // int terminal_sym_comma; [if parse tree]
        var *next;
        
        // Constructor
        var()
        {
            ident = nullptr;
            next = nullptr;
        }
    };

    struct varDecl { // Q: no need to add "var" bc we know it by it's struct type right?
        int terminal_sym_var;
        var *head;

        // Constructor
        varDecl()
        {
            terminal_sym_var = nullptr;
            head = nullptr;
        }
    };

    struct funcDecl { // Q: no need to add "function" bc we know it by it's struct type right?
        int terminal_sym_retType;
        int terminal_sym_func;
        int terminal_sym_name; // include [name] in here
        formalParams *head; // this will be a linked list (bc multiple [funcParam] can exist in a single [funcDecl])
        funcBody *funcBody;
    };

    struct formalParams {
        int ident;
        formalParams *next;
        // formalParams *prev; [not sure if needed; js in case]
    };

    struct funcBody {
        varDecl *varDecl;
        statSeq *statSeq;
    };

    struct computation { // Q: no need to add "main" bc we know it by it's struct type right?
        int terminal_sym_main;
        varDecl *varDecl;
        funcDecl *funcDecl;
        // int terminal_sym_startSS; [if parse tree] `{`
        statSeq *statSeq;
        // int terminal_sym_endSS; [if parse tree] `}`
        int terminal_sym_eof; 

        // Constructor
        computation()
        {
            terminal_sym_main = nullptr;
            varDecl = nullptr;
            funcDecl = nullptr;
            statSeq = nullptr;
            terminal_sym_eof = nullptr;
        }
    };
}


#endif