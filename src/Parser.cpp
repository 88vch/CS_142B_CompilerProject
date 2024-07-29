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

SSA* Parser::p_statSeq() {
    #ifdef DEBUG
        std::cout << "\t\t[Parser::parse_statSeq()]: found a statement to parse" << std::endl;
    #endif
    SSA *first = p_statement(); // ends with `;` = end of statement
    while (this->CheckFor(Res::Result(2, 4), true)) { // if [optional] next token is ';', then we still have statements
        next();
        p_statement();
    }
    return first; // returning first [SSA_instr] in [p_statSeq()] so we know control flow
}

SSA* Parser::p_statement() {
    SSA *stmt = nullptr;
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
    return stmt;
}

// [07/28/2024]: Return the [SSA instr] pointing to the const-assignment of the [value]
//  - Now I'm confused: do we store & compute the literal values now? or do we return ptrs to [SSA_instr's]? 
SSA* Parser::p_assignment() {
    // LET(TUCE GO)
    this->CheckFor(Res::Result(2, 6)); // check for `let`

    // IDENT: not checking for a specific since this is a [variable]
    // - validate that [variable] has been declared
    if (this->varDeclarations.find(SymbolTable::identifiers.at(this->sym.get_value())) == this->varDeclarations.end()) {
        // [Assumption]: we require variables, x, to be declared (i.e. `let x;`) before they are defined
        std::cout << "Error: var [" << this->sym.to_string() << "] hasn't been declared! exiting prematurely..." << std::endl;
        exit(EXIT_FAILURE);
    }
    int ident = SymbolTable::identifiers.at(this->sym.get_value());
    next();

    // ASSIGNMENT
    this->CheckFor(Res::Result(2, 7)); // check for `<-`

    // EXPRESSION
    SSA *value = p_expr();
    // [07/28/2024]: Add [ident : value] mapping into [symbol_table], that's it.
    //  Should not need to return any SSA value for this; will probably need more complexity when BB's are introduced


    // SSA new_instr = SSA(0, value.get_value_literal()); // Why is this SSA hardcoded as a const????
    // // [07/22/2024]: Done
    // // ToDo: should check for existence of constant in [this->SSA_instrs] first
    // if (value.get_kind_literal() == 0) {
    //     if (!SSA_exists(new_instr)) {
    //         this->SSA_instrs.push_back(new_instr); // `const` value
    //     } else {
    //         // ToDo: when modifying curr BB's symbol table (?) point to prev existing SSA that DOM's
    //     }
    // }
    

    
    // Update the current block's [variable-value mapping] to ensure we have the most up to date info
    // - do we need to generate an [SSA] for the assignment? No; though we should create the constant (the value)

    // if (blk->updated_varval_map.find(ident) == blk->updated_varval_map.end()) {
    //     blk->updated_varval_map.insert({ident, value});
    // } else {
    //     blk->updated_varval_map[ident] = value;
    // }
}

// ToDo: after generating the Dot & graph the first time
void Parser::p_funcCall() {}

// ToDo; [07/28/2024]: what exactly are we supposed to return???
void Parser::p_ifStatement() {
    /*
    [07/19/2024] Thought about this:
    ii) we could create the SSA instr WITH the empty position,
    then fill it once we see a corresponding `else`
    - I think this might be better

    Note: the curr instr list here DOMinates all 3 following blocks for [if-statement][then, else, && join]
    */
    // IF
    this->CheckFor(Res::Result(2, 18)); // check `if`; Note: we only do this as a best practice and to consume the `if` token
    p_relation(); // IF: relation

    // THEN
    this->CheckFor(Res::Result(2, 19)); // check `then`

    // // previously: do something start here
    // BasicBlock *ifStat_then;
    // BasicBlock if_then = BasicBlock(blk, nullptr, this->instruction_list);
    // ifStat_then = &if_then;
    // blk->children.push_back(ifStat_then);
    // // previously: do something end here

    SSA *if1 = p_statSeq(); // returns the 1st SSA instruction in the case which we jump to [case 1: if () == true]

    // [Optional] ELSE
    // BasicBlock *ifStat_else = nullptr; // should use some similar logic to check for existence of [else block]
    SSA *if2 = nullptr;
    if (this->CheckFor(Res::Result(2, 20), true)) { // check `else`
        next();

        // // previously: do something start here
        // BasicBlock if_else = BasicBlock(blk, this->instruction_list);
        // ifStat_else = &if_else;
        // blk->children.push_back(ifStat_else);
        // // previously: do something end here

        if2 = p_statSeq(); // returns the 1st SSA instruction in the case which we jump to [case 2: if () == false]
    }

    // FI
    this->CheckFor(Res::Result(2, 21)); // check `fi`

    // no need for phi since only one path! just return the [ifStat_then] block (i think)
    // if (else_exists) {} // is this the same logic as below?
    if (if2 == nullptr) { 
        // return if1; // [07/28/2024]: might need this here(?)
        return ifStat_then;
    }

    // [Special Instruction]: phi() goes here

    SSA phi = SSA(6, if1, if2); // gives us location of where to continue [SSA instr's] based on outcome of [p_relation()]
    this->SSA_instrs.push_back(phi);
    // return phi; // [07/28/2024]: might need this here(?)

    // // previously: do something start here
    // BasicBlock *join_ptr;
    // BasicBlock join = BasicBlock(ifStat_then, ifStat_else, blk->updated_varval_map, this->instruction_list);
    // join_ptr = &join;
    // ifStat_then->children.push_back(join_ptr);
    // ifStat_else->children.push_back(join_ptr);
    // // previously: do something end here

    return join_ptr;
}

// ToDo;
void Parser::p_whileStatement() {
    // WHILE
    this->CheckFor(Res::Result(2, 22)); // check `while`; Note: we only do this as a best practice and to consume the `while` token
    p_relation(); // WHILE: relation

    // DO
    this->CheckFor(Res::Result(2, 23)); // check `do`

    // // previously: do something start here
    // BasicBlock *while_body;
    // BasicBlock body = BasicBlock(blk, this->instruction_list);
    // while_body = &body;
    // blk->children.push_back(while_body);
    // while_body->children.push_back(blk); // technically a "child" since we loop back to it
    // // previously: do something end here

    SSA *while1 = p_statSeq(); // DO: relation

    // OD
    this->CheckFor(Res::Result(2, 24)); // check `od`

    // return while1; // [07/28/2024]: might need this here(?)
}

// ToDo; 
// [07/28/2024]; 
//      Q: Should we return [SSA*] here? Why / Why-not?
//      A: 
// 
//      Q: When will we use a [return] statement? 
//      A: For User-Defined Functions
void Parser::p_return() {
    SSA *retVal = p_expr();

    SSA *ret;
    *ret = SSA(16, retVal); // [SSA constructor] should handle checking of [retVal](nullptr)
    // return ret; // [07/28/2024]: might need this here(?)


    // // [07/19/2024] This probably needs revising
    // if (value == -1) { // no expr (optional)
    //     // previously: do something here (add [return] SSA?)
    //     int op = SymbolTable::operator_table.at("ret");
    //     this->instruction_list.at(op).InsertAtHead(SSA(op, {value}));
    //     blk->instruction_list.at(op).InsertAtHead(SSA(op, {value}));
    // }
}

void Parser::p_relation() {
    SSA *x = p_expr();
    
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


    // ToDo: figure this part out [branch after the comparison]
    int op;
    switch (this->sym.get_value_literal()) {
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
    next();

    SSA *y = p_expr();
    
    
    // ToDo: get [instr_num] from [cmp SSA] -> [cmp_instr_num] so that you can pass it below 
    SSA *cmp_instr;
    *cmp_instr = SSA(5, x, y); // [SymbolTable::operator_table `cmp`: 5]
    this->SSA_instrs.push_back(*cmp_instr);

    // return cmp_instr; // [07/28/2024]: might need this here(?)
}

// returns the corresponding value in [SymbolTable::symbol_table]
// [07/24/2024]: Should always return a [kind=0] const value bc all sub-routes lead to execution of a (+, -, *, /)
SSA* Parser::p_expr() { // Check For `+` && `-`
    SSA *x = p_term();

    while (this->CheckFor(Res::Result(0, true), true) || this->CheckFor(Res::Result(1, true), true)) {
        next();
        SSA *y = p_expr();
        x = BuildIR(x, y); // ToDo: Create IR Block
    }
    return x;
}

SSA* Parser::p_term() { // Check For `*` && `/`
    SSA *x = p_factor();

    while (this->CheckFor(Res::Result(2, true), true) || this->CheckFor(Res::Result(3, true), true)) {
        next();
        SSA *y = p_factor();
        x = BuildIR(x, y); // ToDo: Create IR Block
    }
    return x;
}

SSA* Parser::p_factor() {
    if (this->sym.get_kind_literal() == 0 || this->sym.get_kind_literal() == 1) { // check [ident] or [number]
        // [07/26/2024]: return the SSA for the const value
        // 1) check if value already exists as a const SSA
        // 2) if exists, use and return; else, create and return new SSA const
        SSA *res = this->CheckConstExistence(this->sym.get_value_literal());
        if (!res) { // if dne, create and return new SSA const
            *res = SSA(0, this->sym.get_value_literal());
        }
        return res; // [07/26/2024]: Revised; [07/25/2024]: what do we return here?!?
    } else if (this->CheckFor(Res::Result(2, 13), true)) { // check [`(` expression `)`]
        next(); // consume `(`
        SSA *res = p_expr();
        this->CheckFor(Res::Result(2, 14)); // be sure to consume the `)`
        return res;
    } else if (/* check for funcCall */) { // check [funcCall]

    } else {
        std::cout << "Error: factor expected: [ident || number || `(` expression `)` || funcCall], got: [" << this->sym.to_string() << "]! exiting prematurely..." << std::endl;
        exit(EXIT_FAILURE);
    }
}