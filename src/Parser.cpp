#include "Parser.hpp"

// Top-Down Recursive Descent LL(1) Parsing to generate BB IR-representation
// [0ld Version]: assumes we've generated all SSA Instructions & Have done error checking
// [09/03/2024]: Unable to pre-generate all SSA Instructions (bc some branch locations may be unknown still)
// - need BasicBlock's (IR) to get a better idea of control-flow (-> CFG's)
BasicBlock* Parser::parse() {
    this->reset();

    this->SSA_start = this->p2_start();    
    
    return this->BB0; // stub
}


// does error checking here
void Parser::parse_generate_SSA() {
    this->reset();
    
    // concerned with first 15 operators first
    // ToDo: this is where we will use Recursive Descent [OldParser.*]
    // - [RESUME HERE]!!!!!!
    #ifdef DEBUG
        std::cout << "[Parser::parse_generate_SSA()]" << std::endl;
    #endif
    this->SSA_start = this->p_start();
    #ifdef DEBUG
        std::cout << "Done InITIALLY Parsing, got [" << this->getInstrListSize() << "] SSA Instructions" << std::endl;
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

SSA* Parser::p2_start() {
    #ifdef DEBUG
        std::cout << "[Parser::p2_start(" << this->sym.to_string() << ")]" << std::endl;
    #endif
    this->CheckFor(Result(2, 29)); // consumes `main` token

    // [09/24/2024]: This should be in our first non-const BB right (i.e. not BB0, but first after that)
    if (this->CheckFor(Result(2, 5), true)) { // check for `var` token
        #ifdef DEBUG
            std::cout << "[Parser::parse()]: next token is [varDecl]" << std::endl;
        #endif
        next(); // consumes `var` token
        
        // [09/24/2024]: What we want to add to the first BB
        p_varDecl(); // ends with `;` = end of varDecl (consume it in the func)

        #ifdef DEBUG
            std::cout << "[Parser::parse()]: done parsing [varDecl]'s" << std::endl;
        #endif
    }

    // [09/05/2024]: ToDo - check for [function start] token
    this->CheckFor(Result(2, 15)); // check for `{`

    // [09/24/2024]: Not sure how to handle this
    // - big picture wise it's BB0->BB1->done; where BB1 is everything.
    this->startBB = new BasicBlock(this->instrList);
    this->currBB = this->startBB;
    this->BB0->child = this->startBB;
    this->startBB->parent = this->BB0;
    SSA *statSeq = p2_statSeq();

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
        
        // [09/20/2024]: Thsi should be all we need eh? slightly changed up the way we handle [jmp instr's]
        curr_stmt = p_statement();
        if (curr_stmt == nullptr) { // [curr_stmt] = nullptr indicates that this was the last stmt and it js had a `;` too (last stmt `;` is optional)
            #ifdef DEBUG
                std::cout << "curr_stmt returned nullptr!" << std::endl;
            #endif
            break;
        }
        
        // [07/28/2024]: Should be doing something with [curr_stmt] here(?)
        #ifdef DEBUG
            std::cout << "[curr_stmt] returned: " << curr_stmt->toString() << std::endl;
        #endif
    }
    #ifdef DEBUG
        if (first) {
            std::cout << "[Parser::p_statSeq()] returning: " << first->toString() << std::endl;
        } else {
            std::cout << "[Parser::p_statSeq()] returning: nullptr!" << std::endl;
        }
    #endif
    return first; // returning first [SSA_instr] in [p_statSeq()] so we know control flow
}

// [09/05/2024]: ToDo - resume here (after SSA_linked_list)!
// - should return the first BasicBlock that was created here
SSA* Parser::p2_statSeq() {
    #ifdef DEBUG
        std::cout << "[Parser::p2_statSeq(" << this->sym.to_string() << ")]: found a statement to parse" << std::endl;
    #endif
    // BasicBlock *curr_bb = nullptr, *prev_bb = nullptr;
    // BasicBlock *first = p2_statement(); // ends with `;` = end of statement
    // this->currBB = first;
    // prev_bb = this->currBB;
    // - the first statement should always use the initial BB: even if we have jumps, we always write an SSA first (cmp, bra, etc...)
    
    SSA *first_SSA = nullptr; // the first SSA instr executed in the statement

    while (this->CheckFor(Result(2, 4), true)) { // if [optional] next token is ';', then we still have statements
        next();

        // [09/27/2024]: NOTE we may not even need this, 
        // it ultimately depends on where we 
            // 1) notice we've created and are using a new BB, & 
            // 2) when we have the ability to attatch (link) the newly created BB and old one (i.e. this->newBB->parent = this->oldBB)

        first_SSA = p2_statement();

        if (this->prevJump) {
            this->prevInstr->set_operand2(first_SSA);

            #ifdef DEBUG
                std::cout << "prevInstr [" << this->prevInstr->toString() << "] has been updated with new [y] operand (2)!" << std::endl;
            #endif

            this->prevJump = false;
            this->prevInstr = nullptr;
        }

        // if (curr_bb != prev_bb) {
        //     prev_bb->child = curr_bb;
        //     curr_bb->parent = prev_bb;
        // }

        // if (curr_bb == nullptr) { // [curr_bb] = nullptr indicates that this was the last stmt and it js had a `;` too (last stmt `;` is optional)
        //     #ifdef DEBUG
        //         std::cout << "curr_bb returned nullptr!" << std::endl;
        //     #endif
        //     break;
        // }

        // prev_bb = curr_bb;
        // this->currBB = curr_bb; // [09/27/2024]: implicitly gets passed into all the functions

        // [07/28/2024]: Should be doing something with [curr_bb] here.
        // #ifdef DEBUG
        //     std::cout << "[curr_bb] returned: " << curr_bb->toString() << std::endl;
        // #endif
    }
    #ifdef DEBUG
        std::cout << "[Parser::p2_statSeq()] returning: " << first_SSA->toString() << std::endl;
    #endif
    return first_SSA; // returning first [SSA_instr] in [p_statSeq()] so we know control flow
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
    #ifdef DEBUG
        if (stmt == nullptr) { 
            std::cout << "stmt == nullptr" << std::endl;
        } else {
            std::cout << "stmt != nullptr; stmt = " << stmt->toString() << std::endl;
        }
    #endif
    return stmt; // [07/28/2024]: Why do we return an [SSA*] here?
}

// [10/01/2024]: ToDo
// - should return FIRST instruction executed within statement (for while-loop so we know where to jmp after)
SSA* Parser::p2_statement() {
    #ifdef DEBUG
        std::cout << "[Parser::p2_statement(" << this->sym.to_string() << ")]" << std::endl;
    #endif
    // BasicBlock *stmt = this->currBB;
    SSA *stmt = nullptr;
    // if we see any of these tokens then it means the next thing is another statement
    // - [let, call, if, while, return]: checked by [p_statSeq]
    // [Assumption]: after every statement has a semicolon
    // [Assumption]: if we're in this function that means we know the next thing is a statement
   
    if (this->CheckFor(Result(2, 6), true)) {          // check `let`
        stmt = p2_assignment();
    } else if (this->CheckFor(Result(2, 25), true)) {  // check `call`
        // stmt = p2_funcCall();
    } else if (this->CheckFor(Result(2, 18), true)) {  // check `if`
        // stmt = p2_ifStatement();
    } else if (this->CheckFor(Result(2, 22), true)) {  // check `while` 
        // stmt = p2_whileStatement();
    } else if (this->CheckFor(Result(2, 28), true)) {  // check `return`
        stmt = p2_return();
    }
    // ToDo: [else {}]no more statements left to parse! (Not an error, this is possible)
    #ifdef DEBUG
        if (stmt == nullptr) { 
            std::cout << "stmt == nullptr" << std::endl;
        } else {
            std::cout << "stmt != nullptr; stmt = " << stmt->toString() << std::endl;
        }
    #endif
    return stmt; // [10/03/2024]: Why do we return an [SSA*] here?
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
    int ident = SymbolTable::identifiers.at(this->sym.get_value()); // unused for now
    next();

    // ASSIGNMENT
    this->CheckFor(Result(2, 7)); // check for `<-`

    // EXPRESSION
    SSA *res = p_expr();
    // if (res->get_constVal()) {
    //     if (this->CheckConstExistence(*(res->get_constVal())) == nullptr) {
    //         this->addSSA(res); // [09/30/2024]: Note - there's an additional check here (return's [nullptr || SSA *])
    //     }
    // } else {
    //     if (this->CheckExistence(res->get_operator(), res->get_operand1(), res->get_operand2()) == nullptr) {
    //     }
    // }

    int VV_value = this->add_SSA_table(res);
    // [08/07/2024]: This still holds true (below); 
    // - [ident] should correspond to [SymbolTable::symbol_table] key with the value being the [value]; stoi(value)
    // - Should not need to return any SSA value for this; will probably need more complexity when BB's are introduced
    // [07/28/2024]: Add [ident : value] mapping into [symbol_table], that's it.
    #ifdef DEBUG
        std::cout << "in [Parser::p_assignment]: key=" << SymbolTable::symbol_table.at(ident) << ", current varVal value=";
        // if (this->varVals.find(SymbolTable::symbol_table.at(ident)) != this->varVals.end()) {
        //     std::cout << this->varVals.at(SymbolTable::symbol_table.at(ident))->toString() << std::endl;
        // } 
        
        if (this->VVs.find(ident) != this->VVs.end()) {
            std::cout << this->ssa_table.at(this->VVs.at(ident))->toString() << std::endl;
        } else {
            std::cout << "none!" << std::endl;
        }
    #endif
    // this->varVals.insert_or_assign(SymbolTable::symbol_table.at(ident), this->CheckExistence(instr->get_operator(), instr->get_operand1(), instr->get_operand2()));
    this->VVs.insert_or_assign(ident, VV_value);
    this->printVVs();


    // [08/07/2024]: Revised; wtf was i trying to say down there???
    #ifdef DEBUG
        std::cout << "inserted new var-val mapping: {" << SymbolTable::symbol_table.at(ident) << ", " << res->toString() << "}" << std::endl;
        std::cout << "value addr: " << &res << "; value toString(): " << res->toString() << std::endl;
    #endif
    // this->SSA_instrs.push_back(res);
    #ifdef DEBUG
        std::cout << "[Parser::p_assignment()]: returning " << res->toString() << std::endl;
    #endif
    return res;

    // [07/28/2024]: this will always be a new [SSA instr] bc it contains a path not simply a value (i.e. if the path has multiple ways to get there [think if/while-statements], the value will change depending on which route the path takes)
    // SSA *constVal = nullptr;
    // SSA tmp = SSA(0, res);
    // constVal = &tmp;
    // this->SSA_instrs.push_back(constVal);
    // #ifdef DEBUG
    //     std::cout << "\tcreated new assignment in SSA: [" << constVal->toString() << "]" << std::endl;
    // #endif
    // return constVal;

    
    // Old Code begin;
    // if (blk->updated_varval_map.find(ident) == blk->updated_varval_map.end()) {
    //     blk->updated_varval_map.insert({ident, value});
    // } else {
    //     blk->updated_varval_map[ident] = value;
    // }
    // Old Code end;
}

SSA* Parser::p2_assignment() {
    #ifdef DEBUG
        std::cout << "[Parser::p2_assignment(" << this->sym.to_string() << ")]" << std::endl;
    #endif
    // LET(TUCE GO)
    this->CheckFor(Result(2, 6)); // check for `let`

    // IDENT: not checking for a specific since this is a [variable]
    int ident = SymbolTable::identifiers.at(this->sym.get_value()); // unused for now

    // - validate that [variable] has been declared
    if (this->varDeclarations.find(ident) == this->varDeclarations.end()) {
        // [Assumption]: we don't require variables, x, to be declared (i.e. `let x;`) before they are defined, we'll emit a warning encouraging that tho
        std::cout << "Warning: var [" << this->sym.to_string() << "] hasn't been declared!" << std::endl;
        // exit(EXIT_FAILURE);

    } else {
        // [10/02/2024]: Check if it's already defined before (if so then we need a new BasicBlock!)
        if (this->VVs.find(ident) != this->VVs.end()) {
            #ifdef DEBUG
                std::cout << "ident exists with a definition: ident=" << ident << ", val=" << this->ssa_table.at(this->VVs.at(ident))->toString() << std::endl;
            #endif

            this->currBB->instrList = this->instrList;
            this->parentBB = this->currBB;
            this->currBB = new BasicBlock(this->instrList);
            this->parentBB->child = this->currBB;
            this->currBB->parent = this->parentBB;
        }
    }
    next();

    // ASSIGNMENT
    this->CheckFor(Result(2, 7)); // check for `<-`

    // EXPRESSION
    SSA *ret = nullptr, *value = p_expr();
    
    ret = this->addSSA1(value, true);
    // todo: if SSA instr already exists, no need to create new entry in [add_SSA_table()]
    // if (value->get_constVal()) {
    //     if (this->CheckConstExistence(*(value->get_constVal())) == nullptr) {
    //         ret = this->addSSA(value);
    //     }
    // } else {
    //     if (this->CheckExistence(value->get_operator(), value->get_operand1(), value->get_operand2()) == nullptr) {
    //         ret = this->addSSA(value);
    //     }
    // }

    if (ret != value) {
        #ifdef DEBUG
            std::cout << "SSA instr already exists!, using that instead!" << std::endl;
        #endif
    } else {
        // int VV_value = this->add_SSA_table(value);
        
        // - ToDo: replace with <int, int>
        // this->varVals.insert_or_assign(SymbolTable::symbol_table.at(ident), value); 
        this->VVs.insert_or_assign(ident, this->add_SSA_table(value)); 
    }


    // [08/07/2024]: This still holds true (below); 
    // - [ident] should correspond to [SymbolTable::symbol_table] key with the value being the [value]; stoi(value)
    // - Should not need to return any SSA value for this; will probably need more complexity when BB's are introduced
    // [07/28/2024]: Add [ident : value] mapping into [symbol_table], that's it.
    #ifdef DEBUG
        std::cout << "in [Parser::p2_assignment]: key=" << SymbolTable::symbol_table.at(ident) << ", current varVal value=";
        // if (this->varVals.find(SymbolTable::symbol_table.at(ident)) != this->varVals.end()) {
        //     std::cout << this->varVals.at(SymbolTable::symbol_table.at(ident))->toString() << std::endl;
        // }
        
        if (this->VVs.find(ident) != this->VVs.end()) {
            std::cout << this->ssa_table.at(this->VVs.at(ident))->toString() << std::endl;
        } else {
            std::cout << "none!" << std::endl;
        }
    #endif
    // this->currBB->updated_varval_map.insert_or_assign(SymbolTable::symbol_table.at(ident), value);
    this->printVVs();


    // [08/07/2024]: Revised; wtf was i trying to say down there???
    #ifdef DEBUG
        std::cout << "inserted new var-val mapping: {" << SymbolTable::symbol_table.at(ident) << ", " << ret->toString() << "}" << std::endl;
        std::cout << "value addr: " << &ret << "; value toString(): " << ret->toString() << std::endl;
    #endif
    // this->SSA_instrs.push_back(value);
    #ifdef DEBUG
        std::cout << "[Parser::p_assignment()]: returning " << ret->toString() << std::endl;
    #endif

    // [10/01/2024]: Don't we need to assign [this->VVs] to this->currBB first?
    return ret;
}

// ToDo: after generating the Dot & graph the first time
SSA* Parser::p_funcCall() {
    SSA *res = nullptr;
    #ifdef DEBUG
        std::cout << "[Parser::p_funcCall(" << this->sym.to_string() << ")]" << std::endl;
    #endif

    // check for `call`
    if (this->CheckFor(Result(2, 25), true)) {
        next(); 

        // check UDF's that return void
        Func f(this->sym.get_value());
        next();
        #ifdef DEBUG
            std::cout << "\tcreated func f: [" << f.getName() << "]" << std::endl;
        #endif
        // [09/30/2024]: NOTE - do we need multiple SSA-instr's for read bc of the different input values that may come in from this function call?
        // - we shouldn't because it doesn't take any values it's just the instruction for when we execute
        if (f.name == "InputNum") {
            this->CheckFor_udf_optional_paren();

            // SSA *tmp = new SSA(23);
            res = this->addSSA1(23, nullptr, nullptr, true);
            // if (res != tmp) {
            //     #ifdef DEBUG
            //         std::cout << "res: [" << res->toString() << "] != tmp: [" << tmp->toString() << "]" << std::endl;
            //     #endif
            //     delete tmp;
            //     tmp = nullptr;
            // }

            // res = this->CheckExistence(23, nullptr, nullptr);
            // if (!res) {
            //     res = this->addSSA(new SSA(23));
            // }
            
            #ifdef DEBUG
                std::cout << "\tend of [InputNum()] got res: " << res->toString() << std::endl;
            #endif
        } else if (f.name == "OutputNum") {
            int num;
            // [09/22/2024]: But what are we supposed to do with the args?
            // [08/31/2024]: ToDo: handle args for this to work
            this->CheckFor(Result(2, 13)); // check for `(`
            num = this->sym.get_value_literal();

            #ifdef DEBUG
                std::cout << "\tgot: [" << num << "]" << std::endl;
            #endif

            next();
            this->CheckFor(Result(2, 14)); // check for `)`; sanity...irl could just call [next()]

            // [09/02/2024] Do we need to compute the actual value? 
            // - Or can we simply output the SSA-instruction corresponding to the value? (currently doing this)
            #ifdef DEBUG
                std::cout << "current [varVals] mapping looks like: " << std::endl;
                this->printVVs();
            #endif
            if (this->VVs.find(num) != this->VVs.end()) {
                // res = this->varVals.at(num);
                // std::cout << res->toString() << std::endl;
                
                // SSA *tmp = new SSA(24, this->ssa_table.at(this->VVs.at(num)));
                res = this->addSSA1(24, this->ssa_table.at(this->VVs.at(num)), nullptr, true);
                // if (res != tmp) {
                //     delete tmp;
                //     tmp = nullptr;
                // }
                
                // res = this->CheckExistence(24, this->ssa_table.at(this->VVs.at(num)), nullptr);
                // if (!res) {
                //     res = this->addSSA(new SSA(24, this->VVs.at(num)));
                // }
            } else {
                try {
                    // [10/09/2024]: Can't modify constVal SSA call
                    SSA *tmp = new SSA(0, num);
                    res = this->addSSA1(tmp, true);
                    if (res != tmp) {
                        delete tmp;
                        tmp = nullptr;
                    }

                    // tmp = new SSA(24, res);
                    res = this->addSSA1(24, res, nullptr);
                    // if (res != tmp) {
                    //     delete tmp;
                    //     tmp = nullptr;
                    // }
                    
                    // res = this->CheckConstExistence(num);
                    // // [09/27/2024]: then add to basicblock
                    // if (!res) {
                    //     // [09/27/2024]: if [const-SSA] dne, create & return it [addSSA()]; use that as argument for [write-SSA]
                    //     res = this->addSSA(new SSA(24, this->addSSA(new SSA(0, num))));
                    // }
                } catch(...) {
                    std::cout << "could not recognize identifier corresponding to: [" << num << "]; exiting prematurely..." << std::endl;
                    exit(EXIT_FAILURE);
                }
            }
        } else if (f.name == "OutputNewLine") {
            this->CheckFor_udf_optional_paren();
            // std::cout << std::endl; // new line (?)
            // SSA *tmp = new SSA(25);
            res = this->addSSA1(25, nullptr, nullptr, true);
            // if (res != tmp) {
            //     delete tmp;
            //     tmp = nullptr;
            // }

            // res = this->addSSA(new SSA(25));
        } else {
            // [09/02/2024]: User-Defined Function (?)
            // check for optional `(`: determine whether a UD-function may/will have arguments or not
            if (this->CheckFor(Result(2, 13), true)) {    
                while (this->CheckFor(Result(2, 14), true) == false) { // while the next token is NOT `)`
                    #ifdef DEBUG
                        std::cout << "iterating through args, token looks like: [" << this->sym.to_string() << ", " << this->sym.to_string_literal() << "]" << std::endl;
                    #endif
                    f.args.push_back(this->sym.to_string());
                    next();
                }
                this->CheckFor(Result(2, 14)); // check for `)`; sanity...irl could just call [next()]
            } 
            // [09/02/2024]: ToDo - call the function with the arguments (if they exist)
        }
    } else {
        // check UDF's that return a num (or char?) 
        // this->CheckFor(); // [08/27/2024]: Something like this?...
        // [09/02/2024]: ORRRRR Don't they all start with the `call` token???? confirm this then delete this and js [CheckFor] `call`
    }

    #ifdef DEBUG
        std::cout << "\t[Parser::p_funcCall()] returning res: " << res->toString() << std::endl;
    #endif
    return res; // [08/31]2024]: stub (?)
}

// [10/01/2024]: Do we need to define functions before they're called? 
// - IM SCARED TO START THIS PART
// BasicBlock* Parser::p2_funcCall() {}

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

    SSA *if1 = p_statSeq(); // returns the 1st SSA instruction 

    // [Optional] ELSE
    // if (this->CheckFor(Result(2, 20), true)) { // check `else`
    //    moved below。。。
    // } else {

    if (!this->CheckFor(Result(2, 20), true)) { // if no-else statement
    // 【09/23/2024】： This is not the condition that properly checks whether or not we have an `else` condition
    // - it's possible to have a [statSeq] that doesn't use any SSA (right? Yes. Ex. OutputNum(1) OutputNewLine())
    // [07/28/2024]: Are we not just returning the [SSA *relation] either way?
    // if (if2 == nullptr) {   
        // FI
        this->CheckFor(Result(2, 21)); // check `fi`
    
        // [09/20/2024]: if `else` condition DNE, then we want to set the jump to be the next SSA instr after the `if-condition instr's`
        this->prevJump = true;
        this->prevInstrs.push(jmp_instr); // [10/10/2024]: push at the end so next added SSA-instr will be set as the jump-location

        // [07/28/2024]: might need this here(?)
        return if1; // [08/08/2024]: Good call; yes we should
    }
    // BasicBlock *ifStat_else = nullptr; // should use some similar logic to check for existence of [else block]
    SSA *if2 = nullptr, *jmpIf_instr = nullptr;
    // [09/20/2024]: if `else` exists, we need a jump at the end of `if` right?
    jmpIf_instr = this->addSSA1(8, nullptr, nullptr); // [check=false(?)]
        // [10/10/2024]: this is the last SSA of the if-BasicBlock right?
    
    next();

    // // previously: do something start here
    // BasicBlock if_else = BasicBlock(blk, this->instruction_list);
    // ifStat_else = &if_else;
    // blk->children.push_back(ifStat_else);
    // // previously: do something end here

    // [10/10/2024]: These instructions should start the else-BasicBlock right?
    if2 = p_statSeq(); // returns the 1st SSA instruction in the case which we jump to [case 2: if () == false]
    #ifdef DEBUG
        std::cout << "jmp_instr: " << jmp_instr->toString() << std::endl;
    #endif
    jmp_instr->set_operand2(if2);

    // FI
    this->CheckFor(Result(2, 21)); // check `fi`

    // [Special Instruction]: phi() goes here
    SSA *phi_instr = this->addSSA1(6, if1, if2); // [check=false]; Note - new [addSSA1()] does the same as below's commented out portion!

    // [SECTION_A]
    // 【10/10/2024】： Note that we can't [this->prevJump=true; this->prevInstrs.push(jmpIf_instr)] bc we set_operand1() rather than set_operand2()...see below
    #ifdef DEBUG
        std::cout << "jmpIf_instr: " << jmpIf_instr->toString() << std::endl;
    #endif
    jmpIf_instr->set_operand1(phi_instr); // set_operand1 since it's just a `bra` instr


    // SSA *phi_instr = new SSA(6, if1, if2); // gives us location of where to continue [SSA instr's] based on outcome of [p_relation()]
    // // this->SSA_instrs.push_back(phi_instr);
    // {
    //     SSA *tmpInstr = phi_instr;
    //     phi_instr = this->addSSA1(phi_instr); // [check=false]
    //     // SSA *addedInstr = this->addSSA(phi_instr);
    
    //     // [09/23/2024]: do something like this 
    //     // - bc we made a change in [LinkedList::contains()] && [Parser::addSSA()]
    //     if (tmpInstr != phi_instr) {
    //         // implies our new phi-SSA is a dupe of a [phi] in our LinkedList,
    //         // - so we didn't actually insert the new SSA we created
    //         delete tmpInstr; // so we should do this(?)
    //         tmpInstr = nullptr;
    //     }
    // }


    // return phi_instr; // [07/28/2024]: might need this here(?)

    // // previously: do something start here
    // BasicBlock *join_ptr;
    // BasicBlock join = BasicBlock(ifStat_then, ifStat_else, blk->updated_varval_map, this->instruction_list);
    // join_ptr = &join;
    // ifStat_then->children.push_back(join_ptr);
    // ifStat_else->children.push_back(join_ptr);
    // // previously: do something end here

    // return join_ptr; // [07/28/2024]: might need this here(?)
    // [07/31/2024]: For now don't know what to do abt this (or if this is even right)
    return phi_instr; // [08/08/2024]: For now this is good
}

SSA* Parser::p2_ifStatement() {
    #ifdef DEBUG
        std::cout << "[Parser::p2_ifStatement(" << this->sym.to_string() << ")]" << std::endl;
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

    // [10/14/2024];
    BasicBlock *then_blk = new BasicBlock(this->instrList);
    then_blk->parent = this->currBB;
    this->currBB->child = then_blk;
    BasicBlock *if_parent = this->currBB;
    this->currBB = then_blk; // [10/14/2024]: Assume [p2_statSeq()] manipulates [this->currBB]

    SSA *if1 = p2_statSeq(); // returns the 1st SSA instruction 

    // [Optional] ELSE
    // if (this->CheckFor(Result(2, 20), true)) { // check `else`
    //    moved below。。。
    // } else {

    if (!this->CheckFor(Result(2, 20), true)) { // if no-else statement
    // 【09/23/2024】： This is not the condition that properly checks whether or not we have an `else` condition
    // - it's possible to have a [statSeq] that doesn't use any SSA (right? Yes. Ex. OutputNum(1) OutputNewLine())
    // [07/28/2024]: Are we not just returning the [SSA *relation] either way?
    // if (if2 == nullptr) {   
        // FI
        this->CheckFor(Result(2, 21)); // check `fi`
    
        // [09/20/2024]: if `else` condition DNE, then we want to set the jump to be the next SSA instr after the `if-condition instr's`
        this->prevJump = true;
        this->prevInstrs.push(jmp_instr); // [10/10/2024]: push at the end so next added SSA-instr will be set as the jump-location

        BasicBlock *notThen = new BasicBlock(this->instrList);
        if_parent->child2 = notThen;
        notThen->parent = this->currBB; // [this->currBB (should) == then]
        notThen->parent2 = if_parent;
        this->currBB = notThen;

        // [07/28/2024]: might need this here(?)
        return if1; // [08/08/2024]: Good call; yes we should
    }
    // BasicBlock *ifStat_else = nullptr; // should use some similar logic to check for existence of [else block]
    SSA *if2 = nullptr, *jmpIf_instr = nullptr;
    // [09/20/2024]: if `else` exists, we need a jump at the end of `if` right?
    jmpIf_instr = this->addSSA1(8, nullptr, nullptr); // [check=false(?)]
        // [10/10/2024]: this is the last SSA of the if-BasicBlock right?
    
    next();

    // [10/14/2024];
    BasicBlock *else_blk = new BasicBlock(this->instrList);
    if_parent->child2 = else_blk;
    else_blk->parent = if_parent;
    this->currBB = else_blk; // [10/14/2024]: again, similar to above we assume [p2_statSeq()] manipulates [this->currBB]

    // [10/10/2024]: These instructions should start the else-BasicBlock right?
    if2 = p2_statSeq(); // returns the 1st SSA instruction in the case which we jump to [case 2: if () == false]
    #ifdef DEBUG
        std::cout << "jmp_instr: " << jmp_instr->toString() << std::endl;
    #endif
    jmp_instr->set_operand2(if2);

    // FI
    this->CheckFor(Result(2, 21)); // check `fi`

    // [10/14/2024];
    BasicBlock *join_blk = new BasicBlock(then_blk, else_blk, this->VVs, this->instrList);
    join_blk->parent = then_blk;
    join_blk->parent2 = else_blk;
    then_blk->child = join_blk;
    else_blk->child = join_blk;
    
    this->currBB = join_blk;

    // [Special Instruction]: phi() goes here
    SSA *phi_instr = this->addSSA1(6, if1, if2); // [check=false]; Note - new [addSSA1()] does the same as below's commented out portion!

    // [SECTION_A]
    // 【10/10/2024】： Note that we can't [this->prevJump=true; this->prevInstrs.push(jmpIf_instr)] bc we set_operand1() rather than set_operand2()...see below
    #ifdef DEBUG
        std::cout << "jmpIf_instr: " << jmpIf_instr->toString() << std::endl;
    #endif
    jmpIf_instr->set_operand1(phi_instr); // set_operand1 since it's just a `bra` instr


    // SSA *phi_instr = new SSA(6, if1, if2); // gives us location of where to continue [SSA instr's] based on outcome of [p_relation()]
    // // this->SSA_instrs.push_back(phi_instr);
    // {
    //     SSA *tmpInstr = phi_instr;
    //     phi_instr = this->addSSA1(phi_instr); // [check=false]
    //     // SSA *addedInstr = this->addSSA(phi_instr);
    
    //     // [09/23/2024]: do something like this 
    //     // - bc we made a change in [LinkedList::contains()] && [Parser::addSSA()]
    //     if (tmpInstr != phi_instr) {
    //         // implies our new phi-SSA is a dupe of a [phi] in our LinkedList,
    //         // - so we didn't actually insert the new SSA we created
    //         delete tmpInstr; // so we should do this(?)
    //         tmpInstr = nullptr;
    //     }
    // }

    // [07/31/2024]: For now don't know what to do abt this (or if this is even right)
    return phi_instr; // [08/08/2024]: For now this is good
}

SSA* Parser::p_whileStatement() {
    #ifdef DEBUG
        std::cout << "[Parser::p_whileStatement(" << this->sym.to_string() << ")]" << std::endl;
    #endif
    // WHILE
    this->CheckFor(Result(2, 22)); // check `while`; Note: we only do this as a best practice and to consume the `while` token
    SSA *jmp_instr = p_relation(); // WHILE: relation
    // this->SSA_instrs.push_back(jmp_instr);

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
    // this->SSA_instrs.push_back(while1); // returns first statement from statSeq in while
    #ifdef DEBUG
        std::cout << "[Parser::p_whileStatement()] pushes back: " << while1->toString() << std::endl;
    #endif

    // OD
    this->CheckFor(Result(2, 24)); // check `od`

    // return while1; // [07/28/2024]: might need this here(?)
    // [07/31/2024]: js matching [p_ifStatement()], for now don't know what to do abt this
    
    // [09/20/2024]: Do this before we return. so that the next SSA instr added will be the new jump from [unsuccessful relation: while-loop]
    this->prevJump = true;
    this->prevInstrs.push(jmp_instr);

    return jmp_instr; // [08/08/2024]: This is good here; [jmp_instr] was pushed-back before returing from [p_relation()]
}

// ToDo;
SSA* Parser::p2_whileStatement() {
    #ifdef DEBUG
        std::cout << "[Parser::p2_whileStatement(" << this->sym.to_string() << ")]" << std::endl;
    #endif
    // WHILE
    this->CheckFor(Result(2, 22)); // check `while`; Note: we only do this as a best practice and to consume the `while` token
    SSA *jmp_instr = p2_relation(); // WHILE: relation

    // DO
    this->CheckFor(Result(2, 23)); // check `do`

    BasicBlock *parent_blk = this->currBB;
    BasicBlock *while_blk = new BasicBlock(this->instrList);
    parent_blk->child = while_blk;
    while_blk->parent = parent_blk;
    while_blk->child = parent_blk; // [10/14/2024]: Circular...should we do this?
    this->currBB = while_blk;

    SSA *while1 = p_statSeq(); // DO: relation

    // [10/14/2024]: Do we need a SSA-instr that takes us back to the [cmp-instr]?
    // - check video

    // this->SSA_instrs.push_back(while1); // returns first statement from statSeq in while
    #ifdef DEBUG
        std::cout << "[Parser::p_whileStatement()] pushes back: " << while1->toString() << std::endl;
    #endif

    // OD
    this->CheckFor(Result(2, 24)); // check `od`

    // [09/20/2024]: Do this before we return. so that the next SSA instr added will be the new jump from [unsuccessful relation: while-loop]
    this->prevJump = true;
    this->prevInstrs.push(jmp_instr);

    // [10/14/2024];
    BasicBlock *afterWhile_blk = new BasicBlock(this->instrList);
    afterWhile_blk->parent = parent_blk;
    this->currBB = afterWhile_blk;

    return jmp_instr; // [08/08/2024]: This is good here; [jmp_instr] was pushed-back before returing from [p_relation()]
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

    // SSA *ret = nullptr, *tmp = new SSA(16, retVal); // [SSA constructor] should handle checking of [retVal](nullptr)
    // this->SSA_instrs.push_back(ret);
    // ret = this->addSSA1(tmp, true);
    SSA *ret = this->addSSA1(16, retVal, nullptr, true);

    return ret; // [08/31/2024]: compilation stub
}

SSA* Parser::p2_return() {
    #ifdef DEBUG
        std::cout << "[Parser::p2_return(" << this->sym.to_string() << ")]" << std::endl;
    #endif
    SSA *retVal = p_expr();

    SSA *ret = this->addSSA1(16, retVal, nullptr, true);
    this->add_SSA_table(ret);

    return ret; // [10/02/2024]: compilation stub
}

SSA* Parser::p_relation() {
    // SSA *ret = nullptr;

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
            op = SymbolTable::operator_table.at("ble"); // `bgt` opposite
            break;
        case 9:
            op = SymbolTable::operator_table.at("bge"); // `blt` opposite
            break;
        case 10:
            op = SymbolTable::operator_table.at("bne"); // `beq` opposite
            break;
        case 11:
            op = SymbolTable::operator_table.at("blt"); // `bge` opposite
            break;
        case 12:
            op = SymbolTable::operator_table.at("bgt"); // `ble` opposite
            break;
        default:
            std::cout << "Error: relational operation expected: [`<`, `>`, `<=`, `>=`, `==`], got: [" << this->sym.to_string() << "]! exiting prematurely..." << std::endl;
            exit(EXIT_FAILURE);
            break;
    }

    #ifdef DEBUG
        std::cout << "\tgot op: " << op << " [" << SymbolTable::operator_table_reversed.at(op) << "]" << std::endl;
    #endif

    next();
    SSA *y = p_expr();
    
    
    // ToDo: get [instr_num] from [cmp SSA] -> [cmp_instr_num] so that you can pass it below 
    // SSA *cmp_instr = new SSA(5, x, y); // [SymbolTable::operator_table `cmp`: 5]
    // this->SSA_instrs.push_back(cmp_instr);
    // ret = this->addSSA(cmp_instr);
    // ret = this->addSSA1(cmp_instr);
    SSA *cmp_instr = this->addSSA1(5, x, y); // [check=false(?)]

    // if (ret != cmp_instr) {
    //     delete cmp_instr;
    //     cmp_instr = nullptr;
    // }

    // return cmp_instr; // [07/28/2024]: might need this here(?)
    
    // SSA *jmp_instr = new SSA(op, cmp_instr, nullptr); // [nullptr bc we don't know where to jump to yet]
    // this->SSA_instrs.push_back(jmp_instr);
    // ret = this->addSSA1(jmp_instr);
    
    SSA *jmp_instr = this->addSSA1(op, cmp_instr, nullptr); // [check=false(?)]
    // this->prevJump = true;
    // this->prevInstrs.push(jmp_instr);

    // if (ret != jmp_instr) {
    //     delete jmp_instr;
    //     jmp_instr = ret;

    //     #ifdef DEBUG
    //         std::cout << "jmp_instr dupe existed already; using dupe: " << cmp_instr->toString() << std::endl;
    //     #endif
    // } else {
    //     #ifdef DEBUG
    //         std::cout << "created new jmp_instr: " << cmp_instr->toString() << std::endl;
    //     #endif
    // }

    #ifdef DEBUG
        std::cout << "returning jmp_instr: " << jmp_instr->toString() << std::endl;
    #endif
    
    return jmp_instr;
}

SSA* Parser::p2_relation() {
    return this->prevInstr; // [10/06/2024]: compilation stub
}

// returns the corresponding value in [SymbolTable::symbol_table]
// [07/24/2024]: Should always return a [kind=0] const value bc all sub-routes lead to execution of a (+, -, *, /)
SSA* Parser::p_expr() { // Check For `+` && `-`
    #ifdef DEBUG
        std::cout << "[Parser::p_expr(" << this->sym.to_string() << ")]" << std::endl;
    #endif
    SSA *x = p_term();

    while (this->CheckFor(Result(2, 0), true) || this->CheckFor(Result(2, 1), true)) {
        int op = this->sym.get_value_literal(); // [08/30/2024]: Grab the [op] before we go [next()]
        next();
        SSA *y = p_expr();
        x = BuildIR(op, x, y); // ToDo: Create IR Block
        this->addSSA1(x, true);
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
        int op = this->sym.get_value_literal(); // [08/30/2024]: Grab the [op] before we go [next()]
        next();
        SSA *y = p_factor();
        x = BuildIR(op, x, y); // ToDo: Create IR Block
        std::cout << "in while loop" << std::endl;
    }
    // [08/02/2024]: Segmentation Fault here (both CheckFor()'s should return false, DEBUG stmt should print (currently doesn't!))
    #ifdef DEBUG
        std::cout << "[Parser::p_term()]: returning: ";
        std::cout << x->toString() << std::endl;
    #endif
    return x;
}

SSA* Parser::p_factor() {   
    #ifdef DEBUG
        std::cout << "[Parser::p_factor(" << this->sym.to_string() << ")]" << std::endl;
    #endif
    SSA *res = nullptr;
    if (this->sym.get_kind_literal() == 0) { // check [const]
        #ifdef DEBUG
            std::cout << "\tgot [const], checking existence in [this->instrList]" << std::endl;
        #endif
        SSA *tmp = new SSA(0, this->sym.get_value_literal());
        res = this->addSSA1(tmp, true);
        if (res != tmp) {
            delete tmp;
            tmp = nullptr;

            #ifdef DEBUG
                std::cout << "\t[const] val EXISTS in [this->instrList], using pre-existing-SSA instruction: [" << res->toString() << "]" << std::endl;
            #endif
        } else {
            #ifdef DEBUG
                std::cout << "\t[const] val dne in [this->instrList], created new-SSA instruction: [" << res->toString() << "]" << std::endl;
            #endif
        }
        
        // [07/26/2024]: return the SSA for the const value
        // 1) check if value already exists as a const SSA
        // 2) if exists, use and return; else, create and return new-SSA const
        // res = this->CheckConstExistence(this->sym.get_value_literal());

        // if (res == nullptr) { // if dne, create and return new-SSA const
        //     // int val = this->sym.get_value_literal();
        //     #ifdef DEBUG
        //         std::cout << "\t[Parser::p_factor()] created new [tmp] SSA const instr" << std::endl;
        //     #endif
        //     res = new SSA(0, this->sym.get_value_literal());
        //     // this->SSA_instrs.push_back(res);
        //     this->addSSA(res);
        //     #ifdef DEBUG
        //         std::cout << "\t[const] val dne in [this->instrList], created new-SSA instruction: [" << res->toString() << "]" << std::endl;
        //     #endif
        // }
        next(); // [08/05/2024]: we can consume the [const-val]?
    } else if (this->sym.get_kind_literal() == 1) { // check [ident]
        // if (this->varVals.find(this->sym.get_value()) == this->varVals.end()) {
        if (this->VVs.find(this->sym.get_value_literal()) == this->VVs.end()) {
            std::cout << "Error: p_factor(ident) expected a defined variable (in [varVals]), got: [" << this->sym.to_string() << "]! exiting prematurely..." << std::endl;
            exit(EXIT_FAILURE);
        }
        res = this->ssa_table.at(this->VVs.at(this->sym.get_value_literal()));
        next(); // [08/08/2024]: must consume the [ident]?
    } else if (this->CheckFor(Result(2, 13), true)) { // check [`(` expression `)`]
        next(); // consume `(`
        res = p_expr();
        this->CheckFor(Result(2, 14)); // be sure to consume the `)`
    } else if (this->CheckFor(Result(2, 25), true)) { // check [funcCall]
        // next(); // consume `call`; [09/02/2024]: NOTE: we don't want to consume this token bc [p_funcCall] relies on it!
        res = p_funcCall(); // [09/02/2024]: Assume funcCall (or it's respective function) will consume the closing [this->sym] Result token
    } else {
        std::cout << "Error: factor expected: [ident || number || `(` expression `)` || funcCall], got: [" << this->sym.to_string() << "]! exiting prematurely..." << std::endl;
        exit(EXIT_FAILURE);
    }

    #ifdef DEBUG
        std::cout << "[Parser::p_factor()]: returning: " << res->toString() << std::endl;
    #endif
    return res;
}