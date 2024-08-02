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
    #ifdef DEBUG
        std::cout << "[Parser::parse_generate_SSA()]" << std::endl;
    #endif
    this->start = this->p_start();
    #ifdef DEBUG
        std::cout << "Done InITIALLY Parsing, got [" << this->SSA_instrs.size() << "] SSA Instructions" << std::endl;
    #endif
}


SSA* Parser::p_start() {
    #ifdef DEBUG
        std::cout << "[Parser::p_start(" << this->sym.to_string() << ")]" << std::endl;
    #endif
    this->CheckFor(Result(2, 29)); // consumes `main` token

    if (this->CheckFor(Result(2, 5), true)) { // check for `var` token
        #ifdef DEBUG
            std::cout << "[Parser::parse()]: next token is [varDecl]" << std::endl;
        #endif
        next(); // consumes `var` token
        p_varDecl(); // ends with `;` = end of varDecl (consume it in the func)
        #ifdef DEBUG
            std::cout << "[Parser::parse()]: done parsing [varDecl]'s" << std::endl;
        #endif
    }

    // check for [function start] token

    this->CheckFor(Result(2, 15)); // check for `{`

    // handle [statSeq] here
    SSA *statSeq = p_statSeq();

    this->CheckFor(Result(2, 16)); // check for `}`
    this->CheckFor(Result(2, 30)); // check for `.`

    return statSeq;
}

void Parser::p_varDecl() {
    #ifdef DEBUG
        std::cout << "[Parser::p_varDecl(" << this->sym.to_string() << ")]: got new var: [" << this->sym.to_string() << "]" << std::endl;
    #endif
    this->varDeclarations.insert(SymbolTable::identifiers.at(this->sym.get_value()));
    next();

    // existence of a `,` indicates a next variable exists
    while (this->CheckFor(Result(2, 17), true)) { // check for `,` token
        next(); // consuming the `,`
        // s->next = parse_vars();
        this->varDeclarations.insert(SymbolTable::identifiers.at(this->sym.get_value()));
        next();
    }
    this->CheckFor(Result(2, 4)); // check for `;` token
}

SSA* Parser::p_statSeq() {
    #ifdef DEBUG
        std::cout << "[Parser::p_statSeq(" << this->sym.to_string() << ")]: found a statement to parse" << std::endl;
    #endif
    SSA *curr_stmt;
    SSA *first = p_statement(); // ends with `;` = end of statement
    while (this->CheckFor(Result(2, 4), true)) { // if [optional] next token is ';', then we still have statements
        next();
        curr_stmt = p_statement();
        // [07/28/2024]: Should be doing something with [curr_stmt] here.
    }
    return first; // returning first [SSA_instr] in [p_statSeq()] so we know control flow
}

SSA* Parser::p_statement() {
    #ifdef DEBUG
        std::cout << "[Parser::p_statement(" << this->sym.to_string() << ")]" << std::endl;
    #endif
    SSA *stmt = nullptr;
    // if we see any of these tokens then it means the next thing is another statement
    // - [let, call, if, while, return]: checked by [p_statSeq]
    // [Assumption]: after every statement has a semicolon
    // [Assumption]: if we're in this function that means we know the next thing is a statement
   
    if (this->CheckFor(Result(2, 6), true)) {          // check `let`
        stmt = p_assignment();
    } else if (this->CheckFor(Result(2, 25), true)) {  // check `call`
        stmt = p_funcCall();
    } else if (this->CheckFor(Result(2, 18), true)) {  // check `if`
        stmt = p_ifStatement();
    } else if (this->CheckFor(Result(2, 22), true)) {  // check `while` 
        stmt = p_whileStatement();
    } else if (this->CheckFor(Result(2, 28), true)) {  // check `return`
        stmt = p_return();
    }
    // ToDo: [else {}]no more statements left to parse! (Not an error, this is possible)
    return stmt; // [07/28/2024]: Why do we return an [SSA*] here?
}

// [07/28/2024]: Return the [SSA instr] pointing to the const-assignment of the [value]
//  - Now I'm confused: do we store & compute the literal values now? or do we return ptrs to [SSA_instr's]? 
SSA* Parser::p_assignment() {
    #ifdef DEBUG
        std::cout << "[Parser::p_assignment(" << this->sym.to_string() << ")]" << std::endl;
    #endif
    // LET(TUCE GO)
    this->CheckFor(Result(2, 6)); // check for `let`

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
    this->CheckFor(Result(2, 7)); // check for `<-`

    // EXPRESSION
    SSA *value = p_expr();
    // [07/28/2024]: Add [ident : value] mapping into [symbol_table], that's it.
    //  Should not need to return any SSA value for this; will probably need more complexity when BB's are introduced

    // [07/28/2024]: this will always be a new [SSA instr] bc it contains a path not simply a value (i.e. if the path has multiple ways to get there [think if/while-statements], the value will change depending on which route the path takes)
    SSA *constVal = nullptr;
    *constVal = SSA(0, value);
    this->SSA_instrs.push_back(*constVal);
    return constVal;

    
    // Update the current block's [variable-value mapping] to ensure we have the most up to date info
    // - do we need to generate an [SSA] for the assignment? No; though we should create the constant (the value)

    // Old Code begin;
    // if (blk->updated_varval_map.find(ident) == blk->updated_varval_map.end()) {
    //     blk->updated_varval_map.insert({ident, value});
    // } else {
    //     blk->updated_varval_map[ident] = value;
    // }
    // Old Code end;
}

// ToDo: after generating the Dot & graph the first time
SSA* Parser::p_funcCall() {
    #ifdef DEBUG
        std::cout << "[Parser::p_funcCall(" << this->sym.to_string() << ")]" << std::endl;
    #endif
}

// ToDo; [07/28/2024]: what exactly are we supposed to return???
SSA* Parser::p_ifStatement() {
    #ifdef DEBUG
        std::cout << "[Parser::p_ifStatement(" << this->sym.to_string() << ")]" << std::endl;
    #endif
    /*
    [07/19/2024] Thought about this:
    ii) we could create the SSA instr WITH the empty position,
    then fill it once we see a corresponding `else`
    - I think this might be better

    Note: the curr instr list here DOMinates all 3 following blocks for [if-statement][then, else, && join]
    */
    // IF
    this->CheckFor(Result(2, 18)); // check `if`; Note: we only do this as a best practice and to consume the `if` token
    SSA *jmp_instr = p_relation(); // IF: relation

    // THEN
    this->CheckFor(Result(2, 19)); // check `then`

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
    if (this->CheckFor(Result(2, 20), true)) { // check `else`
        next();

        // // previously: do something start here
        // BasicBlock if_else = BasicBlock(blk, this->instruction_list);
        // ifStat_else = &if_else;
        // blk->children.push_back(ifStat_else);
        // // previously: do something end here

        if2 = p_statSeq(); // returns the 1st SSA instruction in the case which we jump to [case 2: if () == false]
        jmp_instr->set_operand2(if2);
    }

    // FI
    this->CheckFor(Result(2, 21)); // check `fi`

    // [07/28/2024]: Are we not just returning the [SSA *relation] either way?
    // // no need for phi since only one path! just return the [ifStat_then] block (i think)
    // // if (else_exists) {} // is this the same logic as below?
    // if (if2 == nullptr) { 
    //     // return if1; // [07/28/2024]: might need this here(?)
    //     return ifStat_then;
    // }

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

    // return join_ptr; // [07/28/2024]: might need this here(?)
    return jmp_instr; // [07/31/2024]: For now don't know what to do abt this (or if this is even right)
}

// ToDo;
SSA* Parser::p_whileStatement() {
    #ifdef DEBUG
        std::cout << "[Parser::p_whileStatement(" << this->sym.to_string() << ")]" << std::endl;
    #endif
    // WHILE
    this->CheckFor(Result(2, 22)); // check `while`; Note: we only do this as a best practice and to consume the `while` token
    SSA *jmp_instr = p_relation(); // WHILE: relation

    // DO
    this->CheckFor(Result(2, 23)); // check `do`

    // // previously: do something start here
    // BasicBlock *while_body;
    // BasicBlock body = BasicBlock(blk, this->instruction_list);
    // while_body = &body;
    // blk->children.push_back(while_body);
    // while_body->children.push_back(blk); // technically a "child" since we loop back to it
    // // previously: do something end here

    SSA *while1 = p_statSeq(); // DO: relation
    this->SSA_instrs.push_back(*while1);

    // OD
    this->CheckFor(Result(2, 24)); // check `od`

    // return while1; // [07/28/2024]: might need this here(?)
    return jmp_instr; // [07/31/2024]: js matching [p_ifStatement()], for now don't know what to do abt this
}

// ToDo; 
// [07/28/2024]; 
//      Q: Should we return [SSA*] here? Why / Why-not?
//      A: 
// 
//      Q: When will we use a [return] statement? 
//      A: For User-Defined Functions
SSA* Parser::p_return() {
    #ifdef DEBUG
        std::cout << "[Parser::p_return(" << this->sym.to_string() << ")]" << std::endl;
    #endif
    SSA *retVal = p_expr();

    SSA *ret = nullptr;
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

SSA* Parser::p_relation() {
    #ifdef DEBUG
        std::cout << "[Parser::p_relation(" << this->sym.to_string() << ")]" << std::endl;
    #endif
    SSA *x = p_expr();
    
    // Old Version
    // this->CheckForMultiple(this->relational_operations, RELATIONAL_OP_SIZE);

    // New Version [07/19/2024]
    if (!(this->CheckFor(Result(2, 8), true) || this->CheckFor(Result(2, 9), true) || 
        this->CheckFor(Result(2, 10), true) || this->CheckFor(Result(2, 11), true) || 
        this->CheckFor(Result(2, 12), true))) {
            // if none of the check's [`<`, `>`, `<=`, `>=`, `==`] return true, it's not any of these EXPECTED operators: Error!
            std::cout << "Error: relational operation expected: [`<`, `>`, `<=`, `>=`, `==`], got: [" << this->sym.to_string() << "]! exiting prematurely..." << std::endl;
            exit(EXIT_FAILURE);
        }


    // ToDo: figure this part out [branch after the comparison]
    // [07/31/2024]: What do we do here
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
    SSA *cmp_instr = nullptr;
    *cmp_instr = SSA(5, x, y); // [SymbolTable::operator_table `cmp`: 5]
    this->SSA_instrs.push_back(*cmp_instr);

    // return cmp_instr; // [07/28/2024]: might need this here(?)
    SSA *jmp_instr = nullptr;
    *jmp_instr = SSA(op, cmp_instr, nullptr); // [nullptr bc we don't know where to jump to yet]
    this->SSA_instrs.push_back(*jmp_instr);
    
    return jmp_instr;
}

// returns the corresponding value in [SymbolTable::symbol_table]
// [07/24/2024]: Should always return a [kind=0] const value bc all sub-routes lead to execution of a (+, -, *, /)
SSA* Parser::p_expr() { // Check For `+` && `-`
    #ifdef DEBUG
        std::cout << "[Parser::p_expr(" << this->sym.to_string() << ")]" << std::endl;
    #endif
    SSA *x = p_term();

    while (this->CheckFor(Result(2, 0), true) || this->CheckFor(Result(2, 1), true)) {
        next();
        SSA *y = p_expr();
        x = BuildIR(x, y); // ToDo: Create IR Block
    }
    #ifdef DEBUG
        std::cout << "[Parser::p_expr()]: returning: " << x->toString() << std::endl;
    #endif
    return x;
}

SSA* Parser::p_term() { // Check For `*` && `/`
    #ifdef DEBUG
        std::cout << "[Parser::p_term(" << this->sym.to_string() << ")]" << std::endl;
    #endif
    SSA *x = p_factor();

    while (this->CheckFor(Result(2, 2), true) || this->CheckFor(Result(2, 3), true)) {
        next();
        SSA *y = p_factor();
        x = BuildIR(x, y); // ToDo: Create IR Block
    }
    // [08/02/2024]: Segmentation Fault here (both CheckFor()'s should return false, DEBUG stmt should print (currently doesn't!))
    #ifdef DEBUG
        std::cout << "[Parser::p_term()]: returning: " << x->toString() << std::endl;
    #endif
    return x;
}

SSA* Parser::p_factor() {   
    #ifdef DEBUG
        std::cout << "[Parser::p_factor(" << this->sym.to_string() << ")]" << std::endl;
    #endif
    SSA *res = nullptr;
    if (this->sym.get_kind_literal() == 0 || this->sym.get_kind_literal() == 1) { // check [ident] or [number]
        #ifdef DEBUG
            std::cout << "\tgot [const], checking existence in [this->SSA_instrs]" << std::endl;
        #endif
        // [07/26/2024]: return the SSA for the const value
        // 1) check if value already exists as a const SSA
        // 2) if exists, use and return; else, create and return new SSA const
        res = this->CheckConstExistence();

        if (res == nullptr) { // if dne, create and return new SSA const
            int val = this->sym.get_value_literal();
            SSA tmp = SSA(0, &val);
            #ifdef DEBUG
                std::cout << "\tcreated new [tmp] SSA const instr" << std::endl;
            #endif
            res = &tmp;
            #ifdef DEBUG
                std::cout << "\t[const] val dne in [this->SSA_instrs], created new SSA instruction: [" << res->toString() << "]" << std::endl;
            #endif
        }
    } else if (this->CheckFor(Result(2, 13), true)) { // check [`(` expression `)`]
        next(); // consume `(`
        res = p_expr();
        this->CheckFor(Result(2, 14)); // be sure to consume the `)`
    } else if (0 /* stub; check for funcCall */ ) { // check [funcCall]

    } else {
        std::cout << "Error: factor expected: [ident || number || `(` expression `)` || funcCall], got: [" << this->sym.to_string() << "]! exiting prematurely..." << std::endl;
        exit(EXIT_FAILURE);
    }

    #ifdef DEBUG
        std::cout << "[Parser::p_factor()]: returning: " << res->toString() << std::endl;
    #endif
    return res;
}