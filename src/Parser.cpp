#include "Parser.hpp"

// Top-Down Recursive Descent LL(1) Parsing to generate BB IR-representation
// [0ld Version]: assumes we've generated all SSA Instructions & Have done error checking
// [09/03/2024]: Unable to pre-generate all SSA Instructions (bc some branch locations may be unknown still)
// - need BasicBlock's (IR) to get a better idea of control-flow (-> CFG's)
BasicBlock Parser::parse() {
    this->BB0 = new BasicBlock();
    // this->BB_start = this->p2_start();

    this->BB0->child = this->BB_start;
    this->BB_start->parent = this->BB0;
    
    
    return this->BB0; // stub
}


// does error checking here
void Parser::parse_generate_SSA() {
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


// BasicBlock* Parser::p2_start() {
//     #ifdef DEBUG
//         std::cout << "[Parser::p2_start(" << this->sym.to_string() << ")]" << std::endl;
//     #endif
//     this->CheckFor(Result(2, 29)); // consumes `main` token

//     if (this->CheckFor(Result(2, 5), true)) { // check for `var` token
//         #ifdef DEBUG
//             std::cout << "[Parser::parse()]: next token is [varDecl]" << std::endl;
//         #endif
//         next(); // consumes `var` token
//         p2_varDecl(); // ends with `;` = end of varDecl (consume it in the func)
//         #ifdef DEBUG
//             std::cout << "[Parser::parse()]: done parsing [varDecl]'s" << std::endl;
//         #endif
//     }

//     // [09/05/2024]: ToDo - check for [function start] token
//     this->CheckFor(Result(2, 15)); // check for `{`

//     BasicBlock *statSeq = p2_statSeq();

//     this->CheckFor(Result(2, 16)); // check for `}`
//     this->CheckFor(Result(2, 30)); // check for `.`

//     return statSeq;
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
        
        // if ((first->isWhile) || 
        //     (curr_stmt != nullptr && curr_stmt->isWhile)) {
        //     #ifdef DEBUG
        //         std::cout << "in while stmt!" << std::endl;
        //     #endif
        //     SSA *tmp = curr_stmt;
        //     curr_stmt = p_statement();
        //     tmp->set_operand2(curr_stmt); // [08/08/2024]: update [jmp_instr] if previous statement was [while_statement()]
        // } else {
        //     curr_stmt = p_statement();
        //     if (curr_stmt == nullptr) { // [curr_stmt] = nullptr indicates that this was the last stmt and it js had a `;` too (last stmt `;` is optional)
        //         #ifdef DEBUG
        //             std::cout << "curr_stmt returned nullptr!" << std::endl;
        //         #endif
        //         break;
        //     }
        // }
        
        
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
// BasicBlock* Parser::p2_statSeq() {
//     #ifdef DEBUG
//         std::cout << "[Parser::p2_statSeq(" << this->sym.to_string() << ")]: found a statement to parse" << std::endl;
//     #endif
//     SSA *curr_stmt;
//     SSA *first = p_statement(); // ends with `;` = end of statement
//     while (this->CheckFor(Result(2, 4), true)) { // if [optional] next token is ';', then we still have statements
//         next();

        // curr_stmt = p_statement();
        // if (curr_stmt == nullptr) { // [curr_stmt] = nullptr indicates that this was the last stmt and it js had a `;` too (last stmt `;` is optional)
        //     #ifdef DEBUG
        //         std::cout << "curr_stmt returned nullptr!" << std::endl;
        //     #endif
        //     break;
        // }

// //         if ((first->isWhile) || 
// //             (curr_stmt != nullptr && curr_stmt->isWhile)) {
// //             #ifdef DEBUG
// //                 std::cout << "in while stmt!" << std::endl;
// //             #endif
// //             SSA *tmp = curr_stmt;
// //             curr_stmt = p_statement();
// //             tmp->set_operand2(curr_stmt); // [08/08/2024]: update [jmp_instr] if previous statement was [while_statement()]
// //         } else {
// //             curr_stmt = p_statement();
// //             if (curr_stmt == nullptr) { // [curr_stmt] = nullptr indicates that this was the last stmt and it js had a `;` too (last stmt `;` is optional)
// //                 #ifdef DEBUG
// //                     std::cout << "curr_stmt returned nullptr!" << std::endl;
// //                 #endif
// //                 break;
// //             }
// //         }

//         // [07/28/2024]: Should be doing something with [curr_stmt] here.
//         #ifdef DEBUG
//             std::cout << "[curr_stmt] returned: " << curr_stmt->toString() << std::endl;
//         #endif
//     }
//     #ifdef DEBUG
//         std::cout << "[Parser::p_statSeq()] returning: " << first->toString() << std::endl;
//     #endif
//     return first; // returning first [SSA_instr] in [p_statSeq()] so we know control flow
// }

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
    SSA *value = p_expr();
    // [08/07/2024]: This still holds true (below); 
    // - [ident] should correspond to [SymbolTable::symbol_table] key with the value being the [value]; stoi(value)
    // - Should not need to return any SSA value for this; will probably need more complexity when BB's are introduced
    // [07/28/2024]: Add [ident : value] mapping into [symbol_table], that's it.
    #ifdef DEBUG
        std::cout << "in [Parser::p_assignment]: key=" << SymbolTable::symbol_table.at(ident) << ", current varVal value=";
        if (this->varVals.find(SymbolTable::symbol_table.at(ident)) != this->varVals.end()) {
            std::cout << this->varVals.at(SymbolTable::symbol_table.at(ident))->toString() << std::endl;
        } else {
            std::cout << "none!" << std::endl;
        }
    #endif
    this->varVals.insert_or_assign(SymbolTable::symbol_table.at(ident), value);
    this->printVVs();

    if (value->get_constVal()) {
        if (this->CheckConstExistence(*(value->get_constVal())) == nullptr) {
            this->addSSA(value);
        }
    } else {
        if (this->CheckExistence(value->get_operator(), value->get_operand1(), value->get_operand2()) == nullptr) {
            this->addSSA(value);
        }
    }

    // [08/07/2024]: Revised; wtf was i trying to say down there???
    #ifdef DEBUG
        std::cout << "inserted new var-val mapping: {" << SymbolTable::symbol_table.at(ident) << ", " << value->toString() << "}" << std::endl;
        std::cout << "value addr: " << &value << "; value toString(): " << value->toString() << std::endl;
    #endif
    // this->SSA_instrs.push_back(value);
    #ifdef DEBUG
        std::cout << "[Parser::p_assignment()]: returning " << value->toString() << std::endl;
    #endif
    return value;

    // [07/28/2024]: this will always be a new [SSA instr] bc it contains a path not simply a value (i.e. if the path has multiple ways to get there [think if/while-statements], the value will change depending on which route the path takes)
    // SSA *constVal = nullptr;
    // SSA tmp = SSA(0, value);
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

// ToDo: after generating the Dot & graph the first time
SSA* Parser::p_funcCall() {
    SSA *res = nullptr;
    #ifdef DEBUG
        std::cout << "[Parser::p_funcCall(" << this->sym.to_string() << ")]" << std::endl;
    #endif

    // check for `call`
    if (this->CheckFor(Result(2, 25), true)) {
        next(); 

        // #ifdef DEBUG
        //     for (int i = 0; i < 2; i++) {
        //         std::cout << this->sym.to_string() << std::endl;
        //         next();
        //     }
        //     std::cout << "testing [funcCall]" << std::endl;
        //     exit(EXIT_SUCCESS);
        // #endif

        // check UDF's that return void
        Func f(this->sym.get_value());
        next();
        #ifdef DEBUG
            std::cout << "\tcreated func f: [" << f.getName() << "]" << std::endl;
        #endif
        if (f.name == "InputNum") {
            this->CheckFor_udf_optional_paren();

            int in;
            std::cout << "Please enter a number: ";
            std::cin >> in;
            #ifdef DEBUG
                std::cout << "\tGot: [" << in << "]" << std::endl;
            #endif

            // ToDo; 
            // [08/27/2024]: What's expected behavior? repeatedly ping until received a num? or exit fail if not num?
            // [08/31/2024]: Check if [const SSA] of constVal [in] already exists, (if not then create it)
            // 1) check if value already exists as a const SSA
            // 2) if exists, use and return; else, create and return new-SSA const
            res = this->CheckConstExistence(in);

            if (res == nullptr) { // if dne, create and return new-SSA const
                // int val = this->sym.get_value_literal();
                #ifdef DEBUG
                    std::cout << "\t[Parser::funcCall()] created new [tmp] SSA const instr" << std::endl;
                #endif
                res = new SSA(0, in);
                // this->SSA_instrs.push_back(res);
                this->addSSA(res);
                #ifdef DEBUG
                    std::cout << "\t[const] val dne in [this->instrList], created new-SSA instruction: [" << res->toString() << "]" << std::endl;
                #endif
            } else {
                #ifdef DEBUG
                    std::cout << "\t[const] val exists in [this->instrList], looks like: " << res->toString() << std::endl;
                #endif
            }
            // [08/05/2024]: we can consume the [const-val]?
            // [09/02/2024]: No lmao...we can't consume it bc it's a user-input, not a token generated by code
            // next(); // [09/02/2024]: commented out to prevent accidentally consuming the next token
        } else if (f.name == "OutputNum") {
            std::string num;
            // [09/22/2024]: But what are we supposed to do with the args?
            // [08/31/2024]: ToDo: handle args for this to work
            this->CheckFor(Result(2, 13)); // check for `(`
            num = this->sym.get_value();

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
            if (this->varVals.find(num) != this->varVals.end()) {
                res = this->varVals.at(num);
                std::cout << res->toString() << std::endl;
            } else {
                try {
                    std::cout << std::stoi(num) << std::endl;
                } catch(...) {
                    std::cout << "could not recognize identifier corresponding to: [" << num << "]; exiting prematurely..." << std::endl;
                    exit(EXIT_FAILURE);
                }
            }
        } else if (f.name == "OutputNewLine") {
            this->CheckFor_udf_optional_paren();
            std::cout << std::endl; // new line (?)
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
    return res; // [08/31]2024]: stub (?)
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

    SSA *if1 = p_statSeq(); // returns the 1st SSA instruction 

    // [Optional] ELSE
    // BasicBlock *ifStat_else = nullptr; // should use some similar logic to check for existence of [else block]
    SSA *if2 = nullptr, *jmp1 = nullptr;
    if (this->CheckFor(Result(2, 20), true)) { // check `else`
        // [09/20/2024]: if `else` exists, we need a jump at the end of `if` right?
        jmp1 = new SSA(8, nullptr);
    
        next();

        // // previously: do something start here
        // BasicBlock if_else = BasicBlock(blk, this->instruction_list);
        // ifStat_else = &if_else;
        // blk->children.push_back(ifStat_else);
        // // previously: do something end here

        if2 = p_statSeq(); // returns the 1st SSA instruction in the case which we jump to [case 2: if () == false]
        jmp_instr->set_operand2(if2);
    } else {
    // 【09/23/2024】： This is not the condition that properly checks whether or not we have an `else` condition
    // - it's possible to have a [statSeq] that doesn't use any SSA (right? Yes. Ex. OutputNum(1) OutputNewLine())
    // [07/28/2024]: Are we not just returning the [SSA *relation] either way?
    // if (if2 == nullptr) {   
        // FI
        this->CheckFor(Result(2, 21)); // check `fi`
    
        // [09/20/2024]: if `else` condition DNE, then we want to set the jump to be the next SSA instr after the `if-condition instr's`
        this->prevJump = true;
        this->prevInstrs.push(jmp_instr);

        // [07/28/2024]: might need this here(?)
        return if1; // [08/08/2024]: Good call; yes we should
    }

    // FI
    this->CheckFor(Result(2, 21)); // check `fi`

    // [Special Instruction]: phi() goes here
    SSA *phi_instr = new SSA(6, if1, if2); // gives us location of where to continue [SSA instr's] based on outcome of [p_relation()]
    // this->SSA_instrs.push_back(phi_instr);
    SSA *addedInstr = this->addSSA(phi_instr);
    
    // [09/23/2024]: do something like this 
    // - bc we made a change in [LinkedList::contains()] && [Parser::addSSA()]
    {
        if (addedInstr != phi_instr) {
            // implies our new phi-SSA is a dupe of a [phi] in our LinkedList,
            // - so we didn't actually insert the new SSA we created
            delete phi_instr; // so we should do this(?)
        }
    }

    // [09/20/2024]: Add the [jmp1] here (if an `else` condition exists, we jump here after if-statement)
    this->prevJump = true;
    this->prevInstrs.push(jmp1);

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

// ToDo;
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

    SSA *ret = new SSA(16, retVal); // [SSA constructor] should handle checking of [retVal](nullptr)
    // this->SSA_instrs.push_back(ret);
    this->addSSA(ret);
    // return ret; // [07/28/2024]: might need this here(?)


    // // [07/19/2024] This probably needs revising
    // if (value == -1) { // no expr (optional)
    //     // previously: do something here (add [return] SSA?)
    //     int op = SymbolTable::operator_table.at("ret");
    //     this->instruction_list.at(op).InsertAtHead(SSA(op, {value}));
    //     blk->instruction_list.at(op).InsertAtHead(SSA(op, {value}));
    // }

    return ret; // [08/31/2024]: compilation stub
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
    SSA *cmp_instr = new SSA(5, x, y); // [SymbolTable::operator_table `cmp`: 5]
    // this->SSA_instrs.push_back(cmp_instr);
    SSA *addedInstr = this->addSSA(cmp_instr);

    // [09/23/2024]: Similar idea as if-statement
    // - bc we made a change in [LinkedList::contains()] && [Parser::addSSA()]
    if (cmp_instr != addedInstr) {
        delete cmp_instr;
        cmp_instr = addedInstr;
        
        #ifdef DEBUG
            std::cout << "cmp_instr dupe existed already; using dupe: " << cmp_instr->toString() << std::endl;
        #endif
    } else {
        #ifdef DEBUG
            std::cout << "created new cmp_instr: " << cmp_instr->toString() << std::endl;
        #endif
    }


    // return cmp_instr; // [07/28/2024]: might need this here(?)
    SSA *jmp_instr = new SSA(op, cmp_instr, nullptr); // [nullptr bc we don't know where to jump to yet]
    // this->SSA_instrs.push_back(jmp_instr);
    this->addSSA(jmp_instr);
    // [09/23/2024]: For now we shouldn't be pushing it in the relation
    // this->prevJump = true;
    // this->prevInstrs.push(jmp_instr);
    // [09/20/2024]: Nah this is wrong we don't want to add the VERY NEXT SSA as the jmp
    // [09/20/2024]: Added AFTER [this->addSSA()] bc we check [prevJump && prevInstr] in [this->addSSA()]
    
    #ifdef DEBUG
        std::cout << "returning new jmp_instr: " << jmp_instr->toString() << std::endl;
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
        // [07/26/2024]: return the SSA for the const value
        // 1) check if value already exists as a const SSA
        // 2) if exists, use and return; else, create and return new-SSA const
        res = this->CheckConstExistence();

        if (res == nullptr) { // if dne, create and return new-SSA const
            // int val = this->sym.get_value_literal();
            #ifdef DEBUG
                std::cout << "\t[Parser::p_factor()] created new [tmp] SSA const instr" << std::endl;
            #endif
            res = new SSA(0, this->sym.get_value_literal());
            // this->SSA_instrs.push_back(res);
            this->addSSA(res);
            #ifdef DEBUG
                std::cout << "\t[const] val dne in [this->instrList], created new-SSA instruction: [" << res->toString() << "]" << std::endl;
            #endif
        }
        next(); // [08/05/2024]: we can consume the [const-val]?
    } else if (this->sym.get_kind_literal() == 1) { // check [ident]
        if (this->varVals.find(this->sym.get_value()) == this->varVals.end()) {
            std::cout << "Error: p_factor(ident) expected a defined variable (in [varVals]), got: [" << this->sym.to_string() << "]! exiting prematurely..." << std::endl;
            exit(EXIT_FAILURE);
        }
        res = this->varVals.at(this->sym.get_value());
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