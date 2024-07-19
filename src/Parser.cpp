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
    p_statement(); // ends with `;` = end of statement
    if (this->CheckFor(Res::Result(2, 4), true)) { // if [optional] next token is ';', then we still have statements
        next();
        p_statSeq();
    }
}

void Parser::p_statement() {
    // if we see any of these tokens then it means the next thing is another statement
    // - [let, call, if, while, return]: checked by [p_statSeq]
    // assume after every statement has a semicolon
    // assume if we're in this function that means we know the next thing is a statement
    if (this->CheckFor(Res::Result(2, 6), true)) {          // check `let`
        p_assignment();
    } else if (this->CheckFor(Res::Result(2, 25), true)) {  // check `call`
        p_funcCall();
    } else if (this->CheckFor(Res::Result(2, 18), true)) {  // check `if`
        p_ifStatement();
        // need some way to check for `else`

        // Old Version;
        // BasicBlock *join = parse_ifStatement(blk);
        // if (join->parent2 == nullptr) { // means that [ifStat_then] was returned (no else)
        //     blk->children.push_back(join);
        // }
    } else if (this->CheckFor(Res::Result(2, 22), true)) {  // check `while` 
        p_whileStatement();
    } else if (this->CheckFor(Res::Result(2, 28), true)) {  // check `return`
        p_return();
    } else {
        // ToDo: no more statements left to parse!
    }
}

void Parser::p_assignment() {
    // LET(TUCE GO)
    this->CheckFor(Res::Result(2, 6)); // check for `let`

    // IDENT: not checking for a specific since this is a [variable]
    // - validate that [variable] has been declared
    if (this->varDeclarations.find(SymbolTable::identifiers.at(this->sym.get_value())) == this->varDeclarations.end()) {
        // assumes that we require variables to be declared (i.e. `let`) before they are defined
        std::cout << "Error: var [" << this->sym.to_string() << "] doesn't exist! exiting prematurely..." << std::endl;
        exit(EXIT_FAILURE);
    }
    int ident = SymbolTable::identifiers.at(this->sym.get_value());
    next();

    // ASSIGNMENT
    this->CheckFor(Res::Result(2, 7)); // check for `<-`

    // EXPRESSION
    Res::Result value = p_expr();
    if (value.get_kind_literal() == 0) {
        // ToDo: should check for existence of constant in [this->SSA_instrs] first
        this->SSA_instrs.push_back(SSA(0, value.get_value_literal())); // `const` value
    }
    
    // Update the current block's [variable-value mapping] to ensure we have the most up to date info
    // - do we need to generate an [SSA] here？No; though we should create the constant (the value)

    // if (blk->updated_varval_map.find(ident) == blk->updated_varval_map.end()) {
    //     blk->updated_varval_map.insert({ident, value});
    // } else {
    //     blk->updated_varval_map[ident] = value;
    // }
}

// ToDo: after generating the Dot & graph the first time
void Parser::p_funcCall() {}

// ToDo;
void Parser::p_ifStatement() {
    /*
    [07/19/2024] Thought about this:
    ii) or we could create the SSA instr WITH the empty position,
    then fill it once we see a corresponding `else`
    - I think this might be better
    */
    // IF
    p_relation(); // IF: relation

    // THEN
    this->CheckFor(Res::Result(2, 19)); // check `then`

    // previously: do something start here
    BasicBlock *ifStat_then;
    BasicBlock if_then = BasicBlock(blk, this->instruction_list);
    ifStat_then = &if_then;
    blk->children.push_back(ifStat_then);
    // previously: do something end here

    p_statSeq();

    // [Optional] ELSE
    BasicBlock *ifStat_else = nullptr; // should use some similar logic to check for existence of [else block]
    bool else_exists = false;
    if (this->CheckFor(Res::Result(2, 20), true)) { // check `else`
        else_exists = true;
        next();

        // previously: do something start here
        BasicBlock if_else = BasicBlock(blk, this->instruction_list);
        ifStat_else = &if_else;
        blk->children.push_back(ifStat_else);
        // previously: do something end here

        p_statSeq();
    }

    // FI
    this->CheckFor(Res::Result(2, 21)); // check `fi`

    // no need for phi since only one path! just return the [ifStat_then] block (i think)
    // if (else_exists) {} // is this the same logic as below?
    if (ifStat_else == nullptr) { return ifStat_then; }

    // phi() goes here
    BasicBlock *join_ptr;
    BasicBlock join = BasicBlock(ifStat_then, ifStat_else, blk->updated_varval_map, this->instruction_list);
    join_ptr = &join;
    ifStat_then->children.push_back(join_ptr);
    ifStat_else->children.push_back(join_ptr);

    return join_ptr;
}

// ToDo;
void Parser::p_whileStatement() {
    // WHILE
    p_relation(); // WHILE: relation

    // DO
    this->CheckFor(Res::Result(2, 23)); // check `do`

    // previously: do something start here
    BasicBlock *while_body;
    BasicBlock body = BasicBlock(blk, this->instruction_list);
    while_body = &body;
    blk->children.push_back(while_body);
    while_body->children.push_back(blk); // technically a "child" since we loop back to it
    // previously: do something end here

    p_statSeq(); // DO: relation

    // OD
    this->CheckFor(Res::Result(2, 24)); // check `od`
}

// ToDo;
void Parser::p_return() {
    Res::Result v = p_expr();

    // [07/19/2024] This probably needs revising
    if (v == -1) { // no expr (optional)
        // previously: do something here (add [return] SSA?)
        int op = SymbolTable::operator_table.at("ret");
        this->instruction_list.at(op).InsertAtHead(SSA(op, {v}));
        blk->instruction_list.at(op).InsertAtHead(SSA(op, {v}));
    }
}

void Parser::p_relation() {
    std::vector<int> operands;

    Res::Result val1 = p_expr();
    if (val1.get_kind_literal() == 0) {
        // ToDo: should check for existence of constant in [this->SSA_instrs] first
        this->SSA_instrs.push_back(SSA(0, val1.get_value_literal())); // `const` val1
    }
    
    // Old Version
    // this->CheckForMultiple(this->relational_operations, RELATIONAL_OP_SIZE);

    // New Version [07/19/2024]
    if (!(this->CheckFor(Res::Result(2, 8), true) || this->CheckFor(Res::Result(2, 9), true) || 
        this->CheckFor(Res::Result(2, 10), true) || this->CheckFor(Res::Result(2, 11), true) || 
        this->CheckFor(Res::Result(2, 12), true))) {
            // if none of the check's [`<`, `>`, `<=`, `>=`, `==`] return true, it's not any of these EXPECTED operators: Error!
            std::cout << "Error: relational operation expected: [`<`, `>`, `<=`, `>=`, `==`], got: [" << this->sym.to_string() << "]! exiting prematurely..." << std::endl;
            exit(EXIT_FAILURE);
        }
    int relOp = this->sym.get_value_literal();
    next();

    Res::Result val2 = p_expr();
    if (val2.get_kind_literal() == 0) {
        // ToDo: should check for existence of constant in [this->SSA_instrs] first
        this->SSA_instrs.push_back(SSA(0, val2.get_value_literal())); // `const` val2
    }
    
    // [SymbolTable::operator_table `cmp`]
    operands = {val1.get_value_literal(), val2.get_value_literal()};
    this->SSA_instrs.push_back(SSA(5, operands));

    // ToDo: figure this part out [branch after the comparison]
    int op;
    switch (relOp) {
        case 8:
            op = SymbolTable::operator_table.at("bgt");
            break;
        case 9:
            op = SymbolTable::operator_table.at("blt");
            break;
        case 10:
            op = SymbolTable::operator_table.at("beq");
            break;
        case 11:
            op = SymbolTable::operator_table.at("bge");
            break;
        case 12:
            op = SymbolTable::operator_table.at("ble");
            break;
        default:
            std::cout << "Error: relational operation expected: [`<`, `>`, `<=`, `>=`, `==`], got: [" << this->sym.to_string() << "]! exiting prematurely..." << std::endl;
            exit(EXIT_FAILURE);
            break;
    }

    // [07/19/2024] This is definitely wrong
    operands = {curr_instr_num, -1}; // [-1] bc we don't know the specific instr yet
    this->SSA_instrs.push_back(SSA(op, operands));
}

// returns the corresponding value in [SymbolTable::symbol_table]
Res::Result Parser::p_expr() {}