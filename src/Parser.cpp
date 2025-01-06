#include "Parser.hpp"

std::unordered_map<Func*, Parser*> Parser::funcMap = {};

// Top-Down Recursive Descent LL(1) Parsing to generate BB IR-representation
// [0ld Version]: assumes we've generated all SSA Instructions & Have done error checking
// [09/03/2024]: Unable to pre-generate all SSA Instructions (bc some branch locations may be unknown still)
// - need BasicBlock's (IR) to get a better idea of control-flow (-> CFG's)
BasicBlock* Parser::parse() {
    this->SSA_start = this->p2_start();    
    
    return this->BB0; // stub
}


// does error checking here
// void Parser::parse_generate_SSA() {
//     // concerned with first 15 operators first
//     // ToDo: this is where we will use Recursive Descent [OldParser.*]
//     // - [RESUME HERE]!!!!!!
//     #ifdef DEBUG
//         std::cout << "[Parser::parse_generate_SSA()]" << std::endl;
//     #endif
//     this->SSA_start = this->p_start();
//     #ifdef DEBUG
//         std::cout << "Done InITIALLY Parsing, got [" << this->getInstrListSize() << "] SSA Instructions" << std::endl;
//     #endif
// }

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
    this->currBB = new BasicBlock(this->BB0->varVals, false, 1, false);
    this->BB0->child = this->currBB;
    this->currBB->parent = this->BB0;
    #ifdef DEBUG
        std::cout << "created new BB (p2-start)" << std::endl;
    #endif

    this->CheckFor(Result(2, 29)); // consumes `main` token

    // [09/24/2024]: This should be in our first non-const BB right (i.e. not BB0, but first after that)
    if (this->CheckFor(Result(2, 5), true)) { // check for `var` token
        #ifdef DEBUG
            std::cout << "[Parser::parse2()]: next token is [varDecl]" << std::endl;
        #endif
        next(); // consumes `var` token
        
        // [09/24/2024]: What we want to add to the first BB
        p_varDecl(); // ends with `;` = end of varDecl (consume it in the func)

        #ifdef DEBUG
            std::cout << "[Parser::parse2()]: done parsing [varDecl]'s" << std::endl;
        #endif
    }

    // [09/05/2024]: ToDo - check for [function start] token
    // check for `{`
    while(this->CheckFor(Result(2, 15), true) == false) {
        #ifdef DEBUG
            std::cout << "haven't found `{` for main->statSeq; in [p2_start()] found funcDecl() " << std::endl;
        #endif
        bool retVal = true;

        if (this->CheckFor(Result(2, 27), true)) { // check for `void`
            retVal = false;
            next();
        }

        if (this->CheckFor(Result(2, 26))) { // check for `function`
            // p_funcDecl(noRet=true); // [noRet(urn)] indicates whether a func is void or not
            p_funcDecl(retVal); // ends with `;` = end of varDecl (consume it in the func)
        }
    }
    this->CheckFor(Result(2, 15)); // check for `{`

    SSA *statSeq = p2_statSeq();

    this->CheckFor(Result(2, 16)); // check for `}`
    this->CheckFor(Result(2, 30)); // check for `.`

    this->cleanPhiDupes();

    #ifdef DEBUG
        std::cout << "done parsing p2! printing basicblocks: " << std::endl;
        this->blksSeen.clear();
        this->printBlocks(this->BB0);
    #endif

    return statSeq;
}

void Parser::handle_getpar(Func *f, int paramNo) {
    int VV_value;
    SSA *getparSSA = nullptr;

    std::string curr_getPar = "getpar" + std::to_string(paramNo);
    if (SymbolTable::operator_table.find(curr_getPar) == SymbolTable::operator_table.end()){ 
        SymbolTable::operator_table.insert({curr_getPar, SymbolTable::operator_table.size()});
        // [11.24.2024]: must also create the linkedList for this new operator
        BasicBlock::instrList.insert({SymbolTable::operator_table.at(curr_getPar), new LinkedList()});
    }
    // [11.21.2024]: we should create the new SSA in the [func's Parser] (aka: here)
    getparSSA = this->addSSA1(SymbolTable::operator_table.at(curr_getPar), nullptr, nullptr);
    VV_value = this->add_SSA_table(getparSSA);
    
    SymbolTable::update_table(this->func->getName() + "_" + this->sym.get_value(), "identifier");
    this->varDeclarations.insert(SymbolTable::identifiers.at(f->getName() + "_" + this->sym.get_value()));
    this->currBB->varVals.insert_or_assign(SymbolTable::identifiers.at(f->getName() + "_" + this->sym.get_value()), VV_value);
    this->VVs.insert_or_assign(SymbolTable::identifiers.at(f->getName() + "_" + this->sym.get_value()), VV_value);
    
    #ifdef DEBUG
        std::cout << "got new [getpar] SSA: " << getparSSA->toString() << std::endl;
        std::cout << "successfully updated [SymbolTable::symbol_table] with func's identifier" << std::endl;
        std::cout << "varVals looks like: " << std::endl;
        this->printVVs(this->currBB->varVals);
    #endif
    // modify argument ident to be "funcName_ident" to prevent mixing of other args in main{}
    // - leave extra [ident] (bc if its used in main?)(idents added during tokenizing)

    next(); // [11.24.2024]: consuming the `ident`
}

// [11.22.2024]: handles [formalParam] && [funcBody]
SSA* Parser::p2_funcStart(Func *f) {
    #ifdef DEBUG
        std::cout << "[Parser::p2_funcStart(" << this->sym.to_string() << ")]" << std::endl;
    #endif

    this->currBB = new BasicBlock(this->BB0->varVals, false, 1, false);
    this->BB0->child = this->currBB;
    this->currBB->parent = this->BB0;

    #ifdef DEBUG
        std::cout << "created new BB (p2_funcStart)" << std::endl;
    #endif

    // formalParam
    int paramNo = 1;
    this->CheckFor(Result(2, 13)); // check for `(`
    
    // check if [SymbolTable::operator_table(std::string, int) contains [getpar + std::to_string(paramNo)]]
    // - if contains then use it, else add new [getpar] (& respective [setpar]) entry into operator_table and use that
    this->handle_getpar(f, paramNo);
    paramNo++;

    while (this->CheckFor(Result(2, 17), true)) { // check for `,` token
        next(); // [11.24.2024]: consume the `,`
        this->handle_getpar(f, paramNo);
        paramNo++;
    }

    #ifdef DEBUG
        std::cout << "after handling getpar this->varVals looks like: " << std::endl;
        this->printVVs(this->currBB->varVals);
    #endif

    this->CheckFor(Result(2, 14)); // check for `)`
    this->CheckFor(Result(2, 4)); // check for `;` token

    // funcBody: can't we just recursively call [p2_start()] here?
    // - then we need to get a copy of the old values for (whatever we need) before we make the recursive call
    // - and properly ensure everything is set back to the old values after we return from the recusive call
    // - AND, most importantly, somehow store all this information into our [Func] and be able to make these changes/updates per call
    // [11.21.2024]: and if we're oging to do this, 
    // - we need ot make sure we stop after the first [statSeq()] since we won't know where it ends.
    // - so when we create a new [Parser] instance, we must add an additional parameter (bool func=false) 
    //      - so that if we're in a func, we knwo to stop after a certain spot;

    if (this->CheckFor(Result(2, 5), true)) { // check for `var` token
        next(); // consumes `var` token
        p_varDecl(); // ends with `;` = end of varDecl (consume it in the func)
    }

    // [11.22.2024]: todo - ensure that we stop once we're done parsing the [funcBody]
    // - formalParam := "(" [ident {"," ident}]")"
    // - ";"
    // - funcBody := [varDecl] "{"[statSeq]"}"

    this->CheckFor(Result(2, 15)); // check for `{`

    // [11.22.2024]: compilation stub
    SSA *statSeq = p2_statSeq();

    this->CheckFor(Result(2, 16)); // check for `}`

    #ifdef DEBUG
        std::cout << "after funcStart, this->s_index = " << this->s_index << std::endl;
        std::cout << "this->varVals looks like: " << std::endl;
        this->printVVs(this->currBB->varVals);
    #endif

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

void Parser::p_funcDecl(bool retVal) {
    #ifdef DEBUG
        std::cout << "[Parser::p_funcDecl(" << this->sym.to_string() << ")]: got new var: [" << this->sym.to_string() << "]" << std::endl;
    #endif
    int ident = SymbolTable::identifiers.at(this->sym.get_value()); // unused for now
    Func *f = new Func(this->sym.get_value(), retVal);
    // next(); // [11.24.2024]: is this skipping over the next Token? yes LO

    // Ensure the index is within bounds
    if (this->s_index >= this->source_len) {
        std::cerr << "Error: Index out of bounds!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Create a subset vector from curr_idx to the end
    std::vector<Result> subset(this->source.begin() + this->s_index, this->source.end());

    #ifdef DEBUG
        std::cout << "created [std::vector<Result> subset]; looks like: " << std::endl;
        Parser::printResultVec(subset);
    #endif
    
    // [11.22.2024]: create a new [Parser] object for this [func] starting from our currend position in the [Result] tokens
    Parser *p = new Parser(subset, true, true, f);
    
    p->p2_funcStart(f);

    // [11.25.2024]: update [this->s_idx] if needed (after parsing [funcDecl()] in [Parser::p])
    // if (this->s_index < p->getS_idx()) { // this condition should always be true since we assume there is a [funcDecl] to parse
    #ifdef DEBUG
        std::cout << "after [Parser::p] parse, [this->s_idx(" << this->s_index << ", " << this->sym.to_string() << "), p->getS_idx(" << p->getS_idx() << ", " << p->getSym().to_string() << ")]" << std::endl;
    #endif
    this->s_index += p->getS_idx() - 1;
    this->sym = this->source.at(this->s_index++);
    // }
    #ifdef DEBUG
        std::cout << "this->s_index = " << this->s_index << ", " << this->sym.to_string() << std::endl;
    #endif

    this->CheckFor(Result(2, 4)); // check for `;` token

    SymbolTable::func_table.insert({ident, f});
    Parser::funcMap.insert({f, p});

    this->updateConstBlk(p);
}

// [12.02.2024]: must figure out a way to return the SSA's from p's func-call to [this] parser obj
void Parser::p_funcCallUDF(Parser *p) {
    // [Parser *p] has updated [func.args]
    // - need to call the func 
    // - and then transfer the new SSA's that're created into [this] Parser obj
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
        std::cout << "[Parser::p2_statSeq(" << this->sym.to_string() << ", this->currBB=" << this->currBB->toString() << ")]: found a statement to parse" << std::endl;
    #endif
    // BasicBlock *curr_bb = nullptr, *prev_bb = nullptr;
    // BasicBlock *first = p2_statement(); // ends with `;` = end of statement
    // this->currBB = first;
    // prev_bb = this->currBB;
    // - the first statement should always use the initial BB: even if we have jumps, we always write an SSA first (cmp, bra, etc...)
    
    SSA *first_SSA = p2_statement(); // the first SSA instr executed in the statement

    while (this->CheckFor(Result(2, 4), true)) { // if [optional] next token is ';', then we still have statements
        next();
        p2_statement();
    }
    #ifdef DEBUG
        if (first_SSA) {
            std::cout << "[Parser::p2_statSeq()] returning: " << first_SSA->toString() << std::endl;
        } else {
            std::cout << "[Parser::p2_statSeq()] returning: nullptr!" << std::endl;
        }
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
        stmt = p2_funcCall();
    } else if (this->CheckFor(Result(2, 18), true)) {  // check `if`
        stmt = p2_ifStatement();
    } else if (this->CheckFor(Result(2, 22), true)) {  // check `while` 
        stmt = p2_whileStatement();
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

    int VV_value = this->add_SSA_table(res);
    // [08/07/2024]: This still holds true (below); 
    // - [ident] should correspond to [SymbolTable::symbol_table] key with the value being the [value]; stoi(value)
    // - Should not need to return any SSA value for this; will probably need more complexity when BB's are introduced
    // [07/28/2024]: Add [ident : value] mapping into [symbol_table], that's it.
    #ifdef DEBUG
        std::cout << "in [Parser::p_assignment]: key=" << SymbolTable::symbol_table.at(ident) << ", current varVal value="; 

        // [Parser::p_assignment()] 
        if (this->VVs.find(ident) != this->VVs.end()) {
            std::cout << BasicBlock::ssa_table.at(this->VVs.at(ident))->toString() << std::endl;
        } else {
            std::cout << "none!" << std::endl;
        }
    #endif
    // [Parser::p_assignment()]
    this->VVs.insert_or_assign(ident, VV_value);
    // this->currBB->varVals.insert_or_assign(ident, VV_value);
    Parser::printVVs(this->currBB->varVals);


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
}

// [join_blk] is always in [child2]
// [while-loop] always goes back from [child2]
SSA* Parser::p2_assignment() {
    #ifdef DEBUG
        std::cout << "[Parser::p2_assignment(" << this->sym.to_string() << ")]" << std::endl;
        std::cout << "\t[this->currBB] looks like: \n\t" << this->currBB->toString() << std::endl;
    #endif
    // LET(TUCE GO)
    this->CheckFor(Result(2, 6)); // check for `let`

    // IDENT: not checking for a specific since this is a [variable]
    // [11.25.2024]: modified to handle [func]'s ident if exists
    int ident;
    if (this->isFunc) {
        ident = this->generateFuncIdent();
    } else {
        ident = SymbolTable::identifiers.at(this->sym.get_value()); // unused for now
    }
    // int oldInt;
    SSA *oldVal = nullptr;
    bool overwrite = false;

    // [10.29.2024]: todo - update [this->varDeclarations] to only include BB's? (and inherit from parent)?
    // - validate that [variable] has been declared
    // - emit warning but continue nevertheless
    if (this->varDeclarations.find(ident) == this->varDeclarations.end()) {
        // [Assumption]: we don't require variables, x, to be declared (i.e. `let x;`) before they are defined, we'll emit a warning encouraging that tho
        std::cout << "Warning: var [" << this->sym.to_string() << "] hasn't been declared!" << std::endl;
        this->handleUninitVar(ident);
        overwrite = true; // [12.23.2024]: doing this to cancel out newBB creation since this is a shallow insert
        // [11.11.2024]: don't need to call this func here bc it'll set [var == 0]
        //      - we don't care to set it to 0 when we're already in [p2_assiggnment()] about to set it to a variable;
        // this->handleUninitVar(ident);
        // #ifdef DEBUG
        //     std::cout << "uninitialized variable has been handled" << std::endl;
        // #endif
        // exit(EXIT_FAILURE);
    }

    // [12.05.2024]: Check if previous assignment was in [THIS->currBB], if so then we need a new BB
    // - overwrite = true; implies previous assignment was in [this->currBB];
    //     - we move [this->currBB] to the newly created BB
    if ((this->currBB->varVals.find(ident) != this->currBB->varVals.end()) && 
        (this->currBB->findSSA(BasicBlock::ssa_table.at(this->currBB->varVals.at(ident)))) && !overwrite) {
        // oldInt = this->currBB->varVals.at(ident);
        oldVal = BasicBlock::ssa_table.at(this->currBB->varVals.at(ident));
        #ifdef DEBUG
            std::cout << "ident exists with a definition (created in this BB): ident=" << ident << ", val=" << BasicBlock::ssa_table.at(this->currBB->varVals.at(ident))->toString() << std::endl;
        #endif

        if (this->currBB->child == nullptr) {
            #ifdef DEBUG
                std::cout << "child blk == nullptr!" << std::endl;
            #endif
            
            // [1.4.2025]: new 
            // this->currBB->child = new BasicBlock(this->currBB->varVals, false, this->currBB->blkType, false);
            this->currBB->child = new BasicBlock(this->currBB->varVals, false, 1, false);
            this->currBB->child->parent = this->currBB;

            if (this->currBB->child2) {
                #ifdef DEBUG
                    std::cout << "child2 blk == " << this->currBB->child2->toString() << std::endl;
                #endif
                this->currBB->child->child2 = this->currBB->child2;
                if (this->currBB->child2->parent->blockNum == this->currBB->blockNum) {
                    this->currBB->child->child2->parent = this->currBB->child;
                } else {
                    this->currBB->child->child2->parent2 = this->currBB->child;
                }
                this->currBB->child2 = nullptr;
            }
        } else if (this->currBB->child) {
            #ifdef DEBUG
                std::cout << "child blk == " << this->currBB->child->toString() << std::endl;
            #endif
            BasicBlock *oldChild = this->currBB->child;

            // [1.4.2025]: new 
            // this->currBB->child = new BasicBlock(this->currBB->varVals, false, this->currBB->blkType, false);
            this->currBB->child = new BasicBlock(this->currBB->varVals, false, 1, false);
            this->currBB->child->parent = this->currBB;
            this->currBB->child->child = oldChild;
            
            // this->currBB->child->child->parent = this->currBB->child;
            if (this->currBB->child->child2->parent->blockNum == this->currBB->blockNum) {
                this->currBB->child->child2->parent = this->currBB->child;
            } else {
                this->currBB->child->child2->parent2 = this->currBB->child;
            }

            if (this->currBB->child2) {
                #ifdef DEBUG
                    std::cout << "child2 blk == " << this->currBB->child2->toString() << std::endl;
                #endif
                this->currBB->child->child2 = this->currBB->child2;
                
                // this->currBB->child->child2->parent2 = this->currBB->child;
                if (this->currBB->child->child2->parent->blockNum == this->currBB->blockNum) {
                    this->currBB->child->child2->parent = this->currBB->child;
                } else {
                    this->currBB->child->child2->parent2 = this->currBB->child;
                }
                
                this->currBB->child2 = nullptr;
            }
        }

        this->currBB = this->currBB->child;
        #ifdef DEBUG
            std::cout << "this->currBB moved now looks like: " << this->currBB->toString() << std::endl << "\twith varVals like: ";
            Parser::printVVs(this->currBB->varVals);
            std::cout << "this->currBB's child2: ";
            if (this->currBB->child2) {
                std::cout << std::endl << this->currBB->child2->toString() << std::endl;
            } else {
                std::cout << "nullptr!" << std::endl;
            }
        #endif
        overwrite = true;  // 10.29.2024: if this assignment would overwrite a previous varVal mapping
    } else {
        if (overwrite) {
            #ifdef DEBUG
                std::cout << "Warning: uninitialized variable default initialized to 0! Overwriting default value here (same-BB)" << std::endl;
            #endif
        } else {
            #ifdef DEBUG
                std::cout << "Proceeding with std insert" << std::endl;
            #endif
        }

        // [1.1.2024]: if currBB has a prev definition we'll take it
        if (this->currBB->varVals.find(ident) != this->currBB->varVals.end()) {
            // this->handleUninitVar(ident);
            oldVal = BasicBlock::ssa_table.at(this->currBB->varVals.at(ident));
            
            #ifdef DEBUG
                std::cout << "updated oldVal to: " << oldVal->toString() << std::endl;
            #endif
        }
    }

    #ifdef DEBUG
        if (overwrite) {
            std::cout << "overwrite=true;";
        } else {
            std::cout << "overwrite=false;";
        }
        std::cout << "current BB: " << this->currBB->toString() << std::endl;
    #endif
    next();

    // ASSIGNMENT
    this->CheckFor(Result(2, 7)); // check for `<-`

    // EXPRESSION; [10.24.2024]: Slightly refactored...
    SSA *value = p_expr();
    #ifdef DEBUG
        std::cout << "[Parser::p_expr()] returned: " << value->toString() << std::endl;
        // std::cout << "current BB: " << this->currBB->toString() << std::endl;
    #endif
    // [10.29.2024]: Maybe we don't need to add into SSA here?
    // if (!value->get_constVal()) {
    //     value = this->addSSA1(value->get_operator(), value->get_operand1(), value->get_operand2(), true);
    // }
    // 11.04.2024 = dummy coommit
    int table_int = this->add_SSA_table(value);
    #ifdef DEBUG
        std::cout << "returned SSA value assignment has table int of : " << std::to_string(table_int) << std::endl;
    #endif

    // [12.06.2024]: new beginnings: DO WE ADD CURRBB->varVals everytime? 
    // - if so we can just do this [std insert] rather thna placing this check in every blk-branch (new beginnings below cont.)
    
    // [std insert]
    if (overwrite) {
        #ifdef DEBUG
            std::cout << "[blkType: " << this->currBB->blkType << "] overwrite == true" << std::endl;
        #endif
        this->currBB->varVals.insert_or_assign(ident, table_int);
        this->VVs.insert_or_assign(ident, table_int);

        // [12.06.2024]: updating a blk (in if/else) requires updating the previous phi-SSA (in  join-blk)
        // [12.06.2024]: updating a blk (in while)   requires updating the previous phi-SSA (in while-blk)
    } else {
        #ifdef DEBUG
            std::cout << "[blkType: " << this->currBB->blkType << "] overwrite == false" << std::endl;
        #endif
        this->currBB->varVals.insert_or_assign(ident, table_int);
        this->VVs.insert_or_assign(ident, table_int);
    }

    this->blksSeen.clear(); // [12.11.2024]: FOR [this->currBBinLoop()] (recursive) 所以我们要先空着
    bool propagated = false;

    // [12.06.2024]: new beginnings (cont.); maybe overwrite is the last detail after we figureout/decide where specifically we are (i.e. blkType 0, 1, 2, 3)
    if (this->currBBinLoop()) { // if [this->currBB] is in a loop
        #ifdef DEBUG
            std::cout << "currBB is in a loop!" << std::endl;
        #endif
        if (this->currBB->blkType == 3) { // if/else-blk
            #ifdef DEBUG
                std::cout << "currBB is [if/else-blk];  looks like: " << std::endl << this->currBB->toString() << std::endl;
            #endif
            // [12.17.2024]: the real question is how to denote if-path vs else-path
            // - see Parser.hpp::propagateDown()
            // [12.17.2024]: "new" [updateIf] bool where we propagate till the first join-blk (assuming we know we're in a if/else [has join] && the phi in the join's already mapped valid-ly)
            // this->propagateDown(this->currBB, ident, BasicBlock::ssa_table.at(this->currBB->child2->varVals.at(ident))->get_operand1(), table_int, true, {}, true);

            // [12.08.2024]: todo asap
            // if (if/else-blk): add or update join blk for ident
            BasicBlock *join_blk = this->currBB->child2, *tmp = this->currBB;
            this->currBB = join_blk;
            
            // [12.11.2024]: unique case of [conditionalStmtPhiUpdate()]
            std::unordered_set<int> vars = {ident};
            // [this->currBB->parent->parent] migiht not be [if_parent]
            if (tmp->findSSA(value)) {
                oldVal = BasicBlock::ssa_table.at(tmp->parent->varVals.at(ident));
            }
            this->conditionalStmtPhiUpdate(vars, oldVal);
            propagated = true;

            this->currBB = tmp;
        } else if (this->currBB->blkType == 1) { // std-blk type
            // std insert; note we skip bc of above (if we choose to implement like that)
        }
        this->blksSeen.clear();
        // [12.08.2024]: new func todo - [Parser::getLoopHead()]; returns BB start of while-loop (contains cmp instr?) (NOTE: figure out how to denote start of while loop (considering nesting!!!))
        BasicBlock *loopHead = this->getLoopHead();
        #ifdef DEBUG
            std::cout << "loopHead looks like: " << loopHead->toString() << std::endl;
            std::cout << "this->currBB looks like: " << this->currBB->toString() << std::endl;
            std::cout << "oldVal looks like: ";
            if (oldVal) {
                std::cout << oldVal->toString() << std::endl;
            } else {
                std::cout << "nullptr!" << std::endl;
            }
        #endif
        BasicBlock *tmp = this->currBB;
        SSA *prevOldVal = oldVal;
        this->currBB = loopHead;

        if (this->currBB->varVals.find(ident) != this->currBB->varVals.end()) {
            oldVal = BasicBlock::ssa_table.at(this->currBB->varVals.at(ident));

            #ifdef DEBUG
                std::cout << "updated oldVal: " << oldVal->toString() << std::endl;
            #endif 
        }

        // [12.28.2024]: oldVal == nullptr simply indicates that this var hasn't been defined yet (this is first def)
        // if (this->currBB->varVals.find(ident) == this->currBB->varVals.end()) {
        //     this->handleUninitVar(ident);
        // }

        // assume that ident will be defined before a loop is entered

        if ((BasicBlock::ssa_table.find(this->currBB->varVals.at(ident)) != BasicBlock::ssa_table.end()) && BasicBlock::ssa_table.at(this->currBB->varVals.at(ident))->get_operator() == 6) {
        // if ((oldVal->get_operator() == 6)) { /* && (
        //         (this->currBB->findSSA(oldVal)) || (tmp->findSSA(oldVal)))) { */
            #ifdef DEBUG
                std::cout << "currBB's ident is PHI! doing NOTHING..." << std::endl;
            #endif
            // BasicBlock::ssa_table.at(this->currBB->varVals.at(ident))->set_operand2(value);
            oldVal = BasicBlock::ssa_table.at(this->currBB->varVals.at(ident));

            // if (oldVal->get_operand1()->compare(prevOldVal)) {
            //     #ifdef DEBUG
            //         std::cout << "phi's operand1 == prevOldVal!" << std::endl;
            //     #endif
            // } else if (oldVal->get_operand2()->compare(prevOldVal)) {
            //     #ifdef DEBUG
            //         std::cout << "phi's operand2 == prevOldVal!" << std::endl;
            //     #endif
            // }

            if (!propagated) {
                this->propagateDown(this->currBB, ident, oldVal, table_int, true);
            }
        } else {
            SSA *phi = this->addSSA1(6, oldVal, value);
            int phi_table_int = this->add_SSA_table(phi);            
            this->currBB->varVals.insert_or_assign(ident, phi_table_int);
            this->VVs.insert_or_assign(ident, phi_table_int);
            this->currBB->updateInstructions(oldVal, phi);

            // [12.06.2024]: not sure if this does what we expect it to do...
            this->propagateDown(this->currBB, ident, oldVal, phi_table_int, true);
            // this->propagateDown(this->currBB, ident, value, phi_table_int, true);
        }
        
        this->currBB = tmp;
    } else { // if [this->currBB] is NOT in a loop
        #ifdef DEBUG
            std::cout << "currBB is NOT in a loop!" << std::endl;
        #endif
        if (this->currBB->blkType == 3) { // if/else-blk
            #ifdef DEBUG
                std::cout << "currBB is [if/else-blk]" << std::endl;
            #endif
            // [12.08.2024]: todo asap
            // if (if/else-blk): add or update join blk for ident
            BasicBlock *join_blk = this->currBB->child2, *tmp = this->currBB;
            this->currBB = join_blk;
            
            // [12.11.2024]: unique case of [conditionalStmtPhiUpdate()]
            std::unordered_set<int> vars = {ident};
            this->conditionalStmtPhiUpdate(vars, oldVal);

            this->currBB = tmp;
        } else if (this->currBB->blkType == 2) { // join-blk
            // [12.09.2024]: do nothing forn ow...
        } else if (this->currBB->blkType == 1) { // std-blk type
            // [12.08.2024]: [simple_instrs.ty] successfully tests this path (with overwrite)
            // std insert; note we skip bc of above (if we choose to implement like that)
        }
    }

    // [12.22.2024]: committing to new [p2_assignment()]

    Parser::printVVs(this->currBB->varVals);
    
    #ifdef DEBUG
        std::cout << "[Parser::p2_assignment()]: returning " << value->toString() << std::endl;
        std::cout << "current BB: " << this->currBB->toString() << std::endl;
    #endif

    // [10/01/2024]: Don't we need to assign [this->VVs] to this->currBB first?
    return value;
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
        Func *f = new Func(this->sym.get_value());
        next();
        #ifdef DEBUG
            std::cout << "\tcreated func f: [" << f->getName() << "]" << std::endl;
        #endif
        // [09/30/2024]: NOTE - do we need multiple SSA-instr's for read bc of the different input values that may come in from this function call?
        // - we shouldn't because it doesn't take any values it's just the instruction for when we execute
        if (f->name == "InputNum") {
            this->CheckFor_udf_optional_paren();
            res = this->addSSA1(23, nullptr, nullptr, true);
            #ifdef DEBUG
                std::cout << "\tend of [InputNum()] got res: " << res->toString() << std::endl;
            #endif
        } else if (f->name == "OutputNum") {
            int num;
            // [09/22/2024]: But what are we supposed to do with the args?
            // [08/31/2024]: ToDo: handle args for this to work
            this->CheckFor(Result(2, 13)); // check for `(`
            
            // [11.25.2024]: modified to handle [func]'s ident if exists
            // - added if-statement since we don't want [const] to get into this func
            if (this->isFunc && this->sym.get_kind_literal() != 0) {
                num = this->generateFuncIdent(); 
            } else {
                num = this->sym.get_value_literal();
            }

            #ifdef DEBUG
                std::cout << "\tgot: [" << num << "]" << std::endl;
            #endif

            next();
            this->CheckFor(Result(2, 14)); // check for `)`; sanity...irl could just call [next()]

            // [09/02/2024] Do we need to compute the actual value? 
            // - Or can we simply output the SSA-instruction corresponding to the value? (currently doing this)
            #ifdef DEBUG
                std::cout << "current [varVals] mapping looks like: " << std::endl;
                Parser::printVVs(this->currBB->varVals);
            #endif
            if (this->currBB->varVals.find(num) != this->currBB->varVals.end()) {
                res = this->addSSA1(24, BasicBlock::ssa_table.at(this->currBB->varVals.at(num)), nullptr, true);
            } else {
                try {
                    // [10/09/2024]: Can't modify constVal SSA call
                    res = this->addSSA1(0, num, true);

                    // tmp = new SSA(24, res);
                    res = this->addSSA1(24, res, nullptr); 
                } catch(...) {
                    std::cout << "could not recognize identifier corresponding to: [" << num << "]; exiting prematurely..." << std::endl;
                    exit(EXIT_FAILURE);
                }
            }
        } else if (f->name == "OutputNewLine") {
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
            // [12.02.2024]: handled in [p2_funcCall()]...
        }
        delete f;
        f = nullptr;
    }

    #ifdef DEBUG
        std::cout << "\t[Parser::p_funcCall()] returning res: " << res->toString() << std::endl;
    #endif
    return res; // [08/31]2024]: stub (?)
}

// [10/01/2024]: Do we need to define functions before they're called? 
// - IM SCARED TO START THIS PART
// ToDo: after generating the Dot & graph the first time
SSA* Parser::p2_funcCall() {
    SSA *res = nullptr;
    #ifdef DEBUG
        std::cout << "[Parser::p2_funcCall(" << this->sym.to_string() << ")]" << std::endl;
    #endif

    // check for `call`
    if (this->CheckFor(Result(2, 25), true)) {
        next(); 

        // check UDF's that return void
        Func *f = new Func(this->sym.get_value());
        next();
        #ifdef DEBUG
            std::cout << "\tcreated func f: [" << f->getName() << "]" << std::endl;
        #endif
        // [09/30/2024]: NOTE - do we need multiple SSA-instr's for read bc of the different input values that may come in from this function call?
        // - we shouldn't because it doesn't take any values it's just the instruction for when we execute
        if (f->name == "InputNum") {
            this->CheckFor_udf_optional_paren();

            res = this->addSSA1(23, nullptr, nullptr, false); // don't check, just add new SSA's each time for [read]
            
            #ifdef DEBUG
                std::cout << "\tend of [InputNum()] got res: " << res->toString() << std::endl;
            #endif
        } else if (f->name == "OutputNum") {
            int num;
            // [09/22/2024]: But what are we supposed to do with the args?
            // [08/31/2024]: ToDo: handle args for this to work
            this->CheckFor(Result(2, 13)); // check for `(`
            
            // [11.25.2024]: modified to handle [func]'s ident if exists
            // - added if-statement since we don't want [const] to get into this func
            if (this->isFunc && this->sym.get_kind_literal() != 0) {
                num = this->generateFuncIdent(); 
            } else {
                num = this->sym.get_value_literal();
            }

            #ifdef DEBUG
                std::cout << "\tgot: [" << num << "]" << std::endl;
            #endif

            next();
            this->CheckFor(Result(2, 14)); // check for `)`; sanity...irl could just call [next()]

            // [09/02/2024] Do we need to compute the actual value? 
            // - Or can we simply output the SSA-instruction corresponding to the value? (currently doing this)
            #ifdef DEBUG
                std::cout << "current [varVals] mapping looks like: " << std::endl;
                Parser::printVVs(this->currBB->varVals);
            #endif
            if (this->currBB->varVals.find(num) != this->currBB->varVals.end()) {
                res = this->addSSA1(24, BasicBlock::ssa_table.at(this->currBB->varVals.at(num)), nullptr, true);
            } else {
                try {
                    // [10/09/2024]: Can't modify constVal SSA call
                    res = this->addSSA1(0, num, true);

                    // tmp = new SSA(24, res);
                    res = this->addSSA1(24, res, nullptr);
                } catch(...) {
                    std::cout << "could not recognize identifier corresponding to: [" << num << "]; exiting prematurely..." << std::endl;
                    exit(EXIT_FAILURE);
                }
            }
        } else if (f->name == "OutputNewLine") {
            this->CheckFor_udf_optional_paren();
            res = this->addSSA1(25, nullptr, nullptr, true);
        } else {
            // [09/02/2024]: User-Defined Function (?)
            // check for optional `(`: determine whether a UD-function may/will have arguments or not
            if (this->CheckFor(Result(2, 13), true)) {    
                while (this->CheckFor(Result(2, 14), true) == false) { // while the next token is NOT `)`
                    #ifdef DEBUG
                        std::cout << "iterating through args, token looks like: [" << this->sym.to_string() << ", " << this->sym.to_string_literal() << "]" << std::endl;
                    #endif
                    f->args.push_back(this->sym.to_string());
                    next();
                }
                this->CheckFor(Result(2, 14)); // check for `)`; sanity...irl could just call [next()]
            } 
            // [09/02/2024]: ToDo - call the function with the arguments (if they exist)
            Parser *p = this->getFuncParser(f);
            p->func = f; // update the [func]
            this->p_funcCallUDF(p);
            // [11.20.2024]: look through [SymbolTable::symbol_table] for the ident corresponding to the [func]
            // - call that function (figure out what that menas)

            // [11.24.2024]: loop through the [f->args::[std::vector<std::string>]], assign each ident the form: x=[f->name_ident]; use this for [SymbolTable::symbol_table.at(x)]
            // - for each [parameter], this->addSSA1(setpar, x, nullptr);
            // - figure out how to assign this [setpar] to the func (must create a new function in Parser) and get proper return val (i.e. new SSA(?))
        }
    }

    #ifdef DEBUG
        std::cout << "\t[Parser::p_funcCall()] returning res: " << res->toString() << std::endl;
    #endif
    return res; // [08/31]2024]: stub (?)
}

// ToDo; [07/28/2024]: what exactly are we supposed to return???
SSA* Parser::p_ifStatement() {
    #ifdef DEBUG
        std::cout << "[Parser::p_ifStatement(" << this->sym.to_string() << ")]" << std::endl;
    #endif
    // IF
    this->CheckFor(Result(2, 18)); // check `if`; Note: we only do this as a best practice and to consume the `if` token
    SSA *jmp_instr = p_relation(); // IF: relation

    // THEN
    this->CheckFor(Result(2, 19)); // check `then`

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
        return jmp_instr; // [08/08/2024]: Good call; yes we should
    }
    SSA *if2 = nullptr; // , *jmpIf_instr = nullptr;
    // [09/20/2024]: if `else` exists, we need a jump at the end of `if` right?
    // jmpIf_instr = this->addSSA1(8, nullptr, nullptr); // bra instruction; [check=false(?)]
        // [10/10/2024]: this is the last SSA of the if-BasicBlock right?
    
    next();

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
    // #ifdef DEBUG
    //     std::cout << "jmpIf_instr: " << jmpIf_instr->toString() << std::endl;
    // #endif
    // jmpIf_instr->set_operand1(phi_instr); // set_operand1 since it's just a `bra` instr

    return phi_instr; // [08/08/2024]: For now this is good
}

SSA* Parser::p2_ifStatement() {
    #ifdef DEBUG
        std::cout << "[Parser::p2_ifStatement(" << this->sym.to_string() << ")]" << std::endl;
        std::cout << "\tcurrBB looks like: " << std::endl << this->currBB->toString() << std::endl;
    #endif
    // IF
    this->CheckFor(Result(2, 18)); // check `if`; Note: we only do this as a best practice and to consume the `if` token
    SSA *jmp_instr = p_relation(); // IF: relation

    // THEN
    this->CheckFor(Result(2, 19)); // check `then`

    // [10/14/2024];
    BasicBlock *then_blk = new BasicBlock(this->currBB->varVals, false, 3, false);
    #ifdef DEBUG
        std::cout << "created new BB (then)" << std::endl;
    #endif

    BasicBlock *if_parent = this->currBB;
    BasicBlock *og_child = nullptr;
    
    bool hasChild = false;
    if (if_parent->child2) {
        #ifdef DEBUG    
            std::cout << "if_parent has a previous child, looks like: " << std::endl << if_parent->child2->toString() << std::endl;
        #endif
        hasChild = true;
        og_child = if_parent->child2;
    }
    
    then_blk->parent = if_parent;    
    if (if_parent->child == nullptr) {
        if_parent->child = then_blk;
    } else {
        if_parent->child2 = then_blk; // overwrite og_child since we assume that a filled [child] indicates this may be the second branch
    }
    // if_parent = then_blk; // [10/14/2024]: Assume [p2_statSeq()] manipulates [this->currBB]
    this->currBB = this->currBB->child;

    BasicBlock *join_blk = nullptr;
    join_blk = new BasicBlock(this->currBB->varVals, false, 2, false);

    #ifdef DEBUG
        std::cout << "created new BB (join)" << std::endl;
    #endif

    then_blk->child2 = join_blk; // [11.11.2024]: set then_blk->child2 to join_blk
    join_blk->parent = then_blk;
    join_blk->parent2 = if_parent;
    // [11.04.2024]: new; note - [then_blk->child = (old) if_parent->child]
    if (hasChild) {
        #ifdef DEBUG
            std::cout << "moved existing child back (to create new BB)" << std::endl;
        #endif
        join_blk->child2 = og_child;
        if_parent->child2 = join_blk;

        if (og_child->parent == if_parent) {
            og_child->parent = join_blk;
        } else if (og_child->parent2 == if_parent) {
            og_child->parent2 = join_blk;
        }

        #ifdef DEBUG
            std::cout << "join looks like: "  << join_blk->toString() << std::endl;
            std::cout << "currBB looks like: " << this->currBB->toString() << std::endl;
            std::cout << "ifParent looks like: " << if_parent->toString() << std::endl;
        #endif
    }

    // [11.09.2024]: warning unused var?
    // SSA *if1 = 
    p2_statSeq(); // returns the 1st SSA instruction 
    // [10.28.2024]: Guess we don't rlly need to jmp to this since its directly after the cmp

    if (!this->CheckFor(Result(2, 20), true)) { // if no-else statement
    // 【09/23/2024】： This is not the condition that properly checks whether or not we have an `else` condition
    // - it's possible to have a [statSeq] that doesn't use any SSA (right? Yes. Ex. OutputNum(1) OutputNewLine())
    // [07/28/2024]: Are we not just returning the [SSA *relation] either way?
    // if (if2 == nullptr) {   
        // FI
        this->CheckFor(Result(2, 21)); // check `fi`

        // [12.09.2024]: MOVED TO before [jmp_isntr] check
        // [11.08.2024]: shouldn't need condition here cause we handle above, right?
        // if (!hasChild) {
        if_parent->child2 = join_blk;
        join_blk->parent2 = if_parent;
        // } else {
        //     if_parent->child2 = join_blk;
        //     join_blk->parent2 = if_parent;
        //     join_blk->child2 = og_child;
        // }
        this->currBB = join_blk;
    
        // [12.09.2024]: if we have ssa-instr in child2 (join), use (could be phi from statSeq() from if1), else push to [prevInstrs] for when new ssa is encountered
        if ((if_parent->child2) && (if_parent->child2->newInstrs.empty() == false)) {
            jmp_instr->set_operand2(if_parent->child2->newInstrs.front()->instr);    
            #ifdef DEBUG
                std::cout << "updated jmp_instr with child2 first instr: [" << jmp_instr->toString() << "]" << std::endl;
            #endif
        } else {
            // [09/20/2024]: if `else` condition DNE, then we want to set the jump to be the next SSA instr after the `if-condition instr's`
            this->prevJump = true;
            this->prevInstrs.push(jmp_instr); // [10/10/2024]: push at the end so next added SSA-instr will be set as the jump-location
        }

        #ifdef DEBUG
            std::cout << "after if1 statSeq(), [if_parent] looks like: " << if_parent->toString() << std::endl;
            
            std::cout << "[if_parent->child2 (BB[" << if_parent->child2->blockNum << "])] looks like: " << std::endl;
            if (if_parent->child2) {
                std::cout << if_parent->child2->toString() << std::endl;
            } else {
                std::cout << "nullptr!" << std::endl;
            }
        #endif

        // std::unordered_set<int> vars = {};

        // for (const auto &p : if_parent->varVals) {
        //     vars.insert(p.first);
        // }

        // [12.09.2024]: new func, moved code into here
        // this->conditionalStmtPhiUpdate(vars, false, if_parent, then_blk, nullptr);

        this->currBB = join_blk;

        // [12.08.2024]: pretty much everything below here was to ensure phi's successfully created and no dupes (FOR OUR OLD LORD KNOWS HOW METHOD)
        // - handles case: [if && else] exist, 
        // [11.20.2024]: here we make the assumption that [join_blk] has both [parent] && [parent2] ptr if we have (then) && (else) condition
        // BasicBlock *then_blk_last = this->currBB->parent;
        // BasicBlock *else_blk_last = this->currBB->parent2;

        std::unordered_set<int> vars = {};

        // [12.09.2024]: redundant
        // for (const auto &p : if_parent->varVals) {
        //     vars.insert(p.first);
        // }
        // for (const auto &p : then_blk_last->varVals) {
        // [12.09.2024]: keep this cause 
        // - case where this statSeq creates new var that hasn't been declared before (since we only emit warnings and init to 0), 
        for (const auto &p : this->currBB->parent->varVals) {
            vars.insert(p.first);
        }
        // for (const auto &p : else_blk_last->varVals) {
        // for (const auto &p : this->currBB->parent2->varVals) {
        //     vars.insert(p.first);
        // }

        // [SECTION A]: check if we need to create phi-instrs
        #ifdef DEBUG
            std::cout << "then_blk: " << std::endl << then_blk->toString() << std::endl;
            // std::cout << "else_blk: " << std::endl << else_blk->toString() << std::endl;

            std::cout << "then_blk_last: " << std::endl << this->currBB->parent->toString() << std::endl;
            // std::cout << "else_blk_last: " << std::endl << this->currBB->parent2->toString() << std::endl;
        #endif
        
        // [12.09.2024]: new func, moved code into here
        // this->conditionalStmtPhiUpdate(vars, if_parent, then_blk, if_parent);
        this->conditionalStmtPhiUpdate(vars);
        // [12.09.2024]: this is why we have propagateDown(), not for loop, but to update all the subsequent blk's(???); NO, this is separate from [propagateDown()] since propagateDown()'s should be for loop while THIS is simply a check after [if/else-blk] to see if we need to create additional phi's(?)


        //[10.21.2024]: Isn't this actually the first instruction?
        return jmp_instr->get_operand1();
    }
    // BasicBlock *ifStat_else = nullptr; // should use some similar logic to check for existence of [else block]
    SSA *if2 = nullptr;
    // [11.07.2024]: nah, it just falls through...
    // [09/20/2024]: if `else` exists, we need a jump at the end of `if` right?
    // jmpIf_instr = this->addSSA1(8, nullptr, nullptr); // [check=false(?)]
        // [10/10/2024]: this is the last SSA of the if-BasicBlock right?
    
    SSA *if1_bra = new SSA(8, nullptr, nullptr);
    this->addSSA1(if1_bra);
    next();

    // [10/14/2024];
    BasicBlock *else_blk = new BasicBlock(if_parent->varVals, false, 3, false);
    #ifdef DEBUG
        std::cout << "created new BB (else)" << std::endl;
    #endif
    if_parent->child2 = else_blk;
    else_blk->parent = if_parent;
    else_blk->child2 = join_blk; // [11.11.2024]: set else_blk->child2 to join_blk
    join_blk->parent2 = else_blk;

    #ifdef DEBUG
        std::cout << "\tset [else_blk]->child = join_blk; join_blk looks like: ";
        if (join_blk) {
            std::cout << join_blk->toString() << std::endl;
        } else {
            std::cout << "nullptr" << std::endl;
        }
        std::cout << "else_blk looks like: " << else_blk->toString() << std::endl;
        std::cout << "then_blk looks like: " << then_blk->toString() << std::endl;
        std::cout << "if_parent looks like: " << if_parent->toString() << std::endl;
    #endif

    this->currBB = else_blk; // [10/14/2024]: again, similar to above we assume [p2_statSeq()] manipulates [this->currBB]

    // [10/10/2024]: These instructions should start the else-BasicBlock right?
    if2 = p2_statSeq(); // returns the 1st SSA instruction in the case which we jump to [case 2: if () == false]
    jmp_instr->set_operand2(if2);
    #ifdef DEBUG
        std::cout << "p2_statSeq() returned: " << if2->toString() << std::endl;
        std::cout << "jmp_instr (after set): " << jmp_instr->toString() << std::endl;
    #endif

    // FI
    this->CheckFor(Result(2, 21)); // check `fi`

    if (then_blk->child == nullptr) { // [11.20.2024]: added condition to prevent accidental replacment of a nested blk
        then_blk->child = join_blk;
    }
    if (else_blk->child == nullptr) { // [11.20.2024]: added condition to prevent accidental replacment of a nested blk
        else_blk->child = join_blk;
    }

    this->currBB = join_blk;

    // [12.08.2024]: pretty much everything below here was to ensure phi's successfully created and no dupes (FOR OUR OLD LORD KNOWS HOW METHOD)
    // - handles case: [if && else] exist, 
    // [11.20.2024]: here we make the assumption that [join_blk] has both [parent] && [parent2] ptr if we have (then) && (else) condition
    // BasicBlock *then_blk_last = this->currBB->parent;
    // BasicBlock *else_blk_last = this->currBB->parent2;

    std::unordered_set<int> vars = {};

    // [12.09.2024]: redundant
    // for (const auto &p : if_parent->varVals) {
    //     vars.insert(p.first);
    // }
    // for (const auto &p : then_blk_last->varVals) {
    for (const auto &p : this->currBB->parent->varVals) {
        vars.insert(p.first);
    }
    // for (const auto &p : else_blk_last->varVals) {
    for (const auto &p : this->currBB->parent2->varVals) {
        vars.insert(p.first);
    }

    // [SECTION A]: check if we need to create phi-instrs
    #ifdef DEBUG
        std::cout << "then_blk: " << std::endl << then_blk->toString() << std::endl;
        std::cout << "else_blk: " << std::endl << else_blk->toString() << std::endl;

        std::cout << "then_blk_last: " << std::endl << this->currBB->parent->toString() << std::endl;
        std::cout << "else_blk_last: " << std::endl << this->currBB->parent2->toString() << std::endl;
    #endif
    
    // [12.09.2024]: new func, moved code into here
    // this->conditionalStmtPhiUpdate(vars, if_parent, then_blk, else_blk);
    this->conditionalStmtPhiUpdate(vars);
    // [12.09.2024]: this is why we have propagateDown(), not for loop, but to update all the subsequent blk's(???); NO, this is separate from [propagateDown()] since propagateDown()'s should be for loop while THIS is simply a check after [if/else-blk] to see if we need to create additional phi's(?)

    if (this->currBB->newInstrs.empty()) {
        this->prevJump = true;
        this->prevInstrs.push(if1_bra);
    } else {
        if1_bra->set_operand1(this->currBB->newInstrs.at(0)->instr);
    }

    return jmp_instr->get_operand1(); // [11.07.2024]: ?
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

    SSA *while1 = p_statSeq(); // DO: relation
    // this->SSA_instrs.push_back(while1); // returns first statement from statSeq in while

    // OD
    this->CheckFor(Result(2, 24)); // check `od`
    
    #ifdef DEBUG
    if (while1) {
        std::cout << "[Parser::p_whileStatement()] pushes back: " << while1->toString() << std::endl;
    } else {
        std::cout << "[Parser::p_whileStatement()] pushes back: nullptr!" << std::endl;
    }
    #endif

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
    SSA *jmp_instr = p_relation(); // WHILE: relation
    #ifdef DEBUG
        std::cout << "\t[p_relation()] got: " << jmp_instr->toString() << std::endl;
    #endif

    // DO
    this->CheckFor(Result(2, 23)); // check `do`

    // [10/17/2024]: BasicBlock Excerpt BEIGNN
    BasicBlock *parent_blk = this->currBB;
    parent_blk->setMainWhile(); // [12.11.2024]: this should be the proper mainWhile
    BasicBlock *while_blk = new BasicBlock(this->currBB->varVals, false, 1, false);
    #ifdef DEBUG
        std::cout << "created new BB (while-body)" << std::endl << while_blk->toString() << std::endl;
    #endif
    // 11.04.2024: dummy modification to write commit
    parent_blk->child = while_blk;
    while_blk->parent = parent_blk;
    while_blk->child2 = parent_blk; // [10/30/2024]: changed to child2
    // this->currBB = while_blk;
    // this->currBB = this->currBB->child; // [10.21.2024]: same as right above
    // END

    // [10.24.2024]: Moved; 10.30.2024: this is essentially the join-blk
    BasicBlock *afterWhile_blk = new BasicBlock(this->currBB->varVals, false, 1, false);
    afterWhile_blk->parent = parent_blk;
    parent_blk->child2 = afterWhile_blk;
    #ifdef DEBUG
        std::cout << "created new BB after-While" << std::endl << afterWhile_blk->toString() << std::endl;
    #endif

    this->currBB = while_blk;
    #ifdef DEBUG
        std::cout << "right before while-statSeq, [this->currBB]'s child2 looks like (parent_blk): " << this->currBB->child2->toString() << std::endl;
    #endif
    SSA *while1 = p2_statSeq(); // DO: relation

    // this->SSA_instrs.push_back(while1); // returns first statement from statSeq in while
    #ifdef DEBUG
        if (while1) {
            std::cout << "[Parser::p2_whileStatement()] first statSeq instr returned: " << while1->toString() << std::endl;
        } else {
            std::cout << "[Parser::p2_whileStatement()] first statSeq instr returned: nullptr!" << std::endl;;
        }
    #endif

    // OD
    this->CheckFor(Result(2, 24)); // check `od`

    // [09/20/2024]: Do this before we return. so that the next SSA instr added will be the new jump from [unsuccessful relation: while-loop]
    this->prevJump = true;
    this->prevInstrs.push(jmp_instr);
    #ifdef DEBUG
        std::cout << "pushed jmp instr: " << jmp_instr->toString() << std::endl;
    #endif
    
    // [10.22.2024]: Somehow after [p2_statSeq()], this->currBB gets changed? So we just reset it here...is this ok/right/valid???
    // afterWhile_blk->varVals = this->currBB->varVals; // [12.30.2024]: update varVals here???
    this->currBB = afterWhile_blk;
    // this->currBB = this->currBB->child2; // [10.21.2024]: Same as above

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
    next(); // consume `return` token
    SSA *retVal = p_expr();

    #ifdef DEBUG
        std::cout << "retVal looks like: " << retVal->toString() << std::endl;
    #endif

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
        x = this->addSSA1(x, true);
        #ifdef DEBUG
            std::cout << "x has now been updated to: " << x->toString() << std::endl;
        #endif
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
        x = this->addSSA1(x, true);
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
        res = this->addSSA1(0, this->sym.get_value_literal(), true);
        next(); // [08/05/2024]: we can consume the [const-val]?
    } else if (this->sym.get_kind_literal() == 1) { // check [ident]
        // [11.25.2024]: modified to handle [func]'s ident if exists
        int ident;
        if (this->isFunc) {
            ident = this->generateFuncIdent();
        } else {
            ident = this->sym.get_value_literal();
        }

        if (this->currBB->varVals.find(ident) == this->currBB->varVals.end()) {
            std::cout << "Warning: p_factor(ident) expected a defined variable (in [varVals]), got (uninitialized): [" << this->sym.to_string() << "]! Default: setting to 0..." << std::endl;
            this->handleUninitVar(ident);
            #ifdef DEBUG
                std::cout << "uninitialized variable has now been set to 0. this->currBB looks like: " << std::endl << this->currBB->toString() << std::endl;
            #endif
            // exit(EXIT_FAILURE);
        }
        res = BasicBlock::ssa_table.at(this->currBB->varVals.at(ident));
        #ifdef DEBUG
            std::cout << "got ident with value: " << res->toString() << std::endl;
        #endif
        next(); // [08/08/2024]: must consume the [ident]?
    } else if (this->CheckFor(Result(2, 13), true)) { // check [`(` expression `)`]
        next(); // consume `(`
        res = p_expr();
        this->CheckFor(Result(2, 14)); // be sure to consume the `)`
    } else if (this->CheckFor(Result(2, 25), true)) { // check [funcCall]
        // next(); // consume `call`; [09/02/2024]: NOTE: we don't want to consume this token bc [p_funcCall] relies on it!
        if (this->block) { res = p2_funcCall(); } 
        else { res = p_funcCall(); }
        // [09/02/2024]: Assume funcCall (or it's respective function) will consume the closing [this->sym] Result token
    } else {
        std::cout << "Error: factor expected: [ident || number || `(` expression `)` || funcCall], got: [" << this->sym.to_string() << "]! exiting prematurely..." << std::endl;
        exit(EXIT_FAILURE);
    }

    #ifdef DEBUG
        std::cout << "[Parser::p_factor()]: returning: " << res->toString() << std::endl;
    #endif
    return res;
}