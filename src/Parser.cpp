#include "Parser.hpp"

// Recursive Descent LL(1) Parsing to generate BB IR-representation
// - assume we've generated all SSA Instructions & Have done error checking
BasicBlock Parser::parse() {
    BasicBlock r(this->instrList);
    return r; // Stub
}


// does error checking here
void Parser::parse_generate_SSA() {
    // concerned with first 15 operators first
    // ToDo: this is where we will use Recursive Descent [OldParser.*]
    // - [RESUME HERE]!!!!!!
}


void Parser::p_start() {
    this->CheckFor(Res::Result(2, 29)); // consumes `main` token

    if (this->CheckFor(Res::Result(2, 6), true)) { // check for `let` token
        #ifdef DEBUG
            std::cout << "\t[Parser::parse()]: next token is [varDecl]" << std::endl;
        #endif
        next(); // consumes `var` token
        // s->varDecl = parse_varDecl(); // ends with `;` = end of varDecl (consume it in the func)
        p_varDecl();
        #ifdef DEBUG
            std::cout << "\t[Parser::parse()]: done parsing [varDecl]'s" << std::endl;
        #endif
    }

    // check for [function start] token

    this->CheckFor(Res::Result(2, 15));

    // handle [statSeq] here
    p_statSeq();

    this->CheckFor(Res::Result(2, 16));
    this->CheckFor(Res::Result(2, 30));
}

void Parser::p_varDecl() {
    #ifdef DEBUG
        std::cout << "\t\t[Parser::parse_varDecl()]: got new var: [" << this->sym << "]" << std::endl;
    #endif
    this->varDeclarations.insert(SymbolTable::identifiers.at(this->sym.get_value()));
    next();

    // existence of a `,` indicates a next variable exists
    if (this->CheckFor(Res::Result(2, 17), true)) { // check for `,` token
        next(); // consuming the `,`
        // s->next = parse_vars();
        p_varDecl();
    }
    this->CheckFor(Res::Result(2, 4), true); // check for `;` token
}

void Parser::p_statSeq() {
    #ifdef DEBUG
        std::cout << "\t\t[Parser::parse_statSeq()]: found a statement to parse" << std::endl;
    #endif
    p_statement(); // ends with `;` = end of varDecl (consume it in the func)

    next(); // do we need this?
    if (this->CheckFor(Res::Result(2, 4), true)) { // if [optional] next token is ';', then we still have statements
        next();
        // if we see any of these tokens then it means the next thing is another statement
        if (this->CheckFor(Res::Result(2, 6), true)) {          // check `let`
            p_statSeq();
        } else if (this->CheckFor(Res::Result(2, 25), true)) {  // check `call`
            p_statSeq();
        } else if (this->CheckFor(Res::Result(2, 18), true)) {  // check `if`
            p_statSeq();
        } else if (this->CheckFor(Res::Result(2, 22), true)) {  // check `while` 
            p_statSeq();
        } else if (this->CheckFor(Res::Result(2, 28), true)) {  // check `return`
            p_statSeq();
        }
    }
}

void Parser::p_statement() {

}