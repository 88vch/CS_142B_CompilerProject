#include "Parser.hpp"

// Question: do we need the terminals??? (i.e. struct->terminal_sym_*)
// Question: how to determine returnStatement [optional expression]?
// Remark: we should be generating AST (don't care abt terminals ["syntactic sugar"])

// Function to perform common subexpression elimination (CSE)
void performCSE(std::vector<IRNode>& ir) {
    std::unordered_map<std::string, std::string> expressions;
    for (auto it = ir.begin(); it != ir.end(); ++it) {
        if (it->operation != "print") {
            // Check if expression already exists
            std::string expression = it->operation + " " + it->operand1 + " " + it->operand2;
            if (expressions.find(expression) != expressions.end()) {
                // Replace current result with existing result
                it->result = expressions[expression];
            } else {
                // Store new expression
                expressions[expression] = it->result;
            }
        }
    }
}

// Function to print IR
void printIR(const std::vector<IRNode>& ir) {
    for (const auto& node : ir) {
        std::cout << node.result << " = " << node.operation << " " << node.operand1 << " " << node.operand2 << std::endl;
    }
}

// Get intermediate representation (IR)
std::vector<IRNode> Parser::getIR() {
    return ir;
}


// if error [CheckFor() handles it]
void Parser::parse() {
    #ifdef DEBUG
        std::cout << "\t[Parser::parse()]: this->instruction_list for [main] looks like: " << std::endl;
    #endif
    // node::main *s; // expected start: [main]
    BasicBlock main = BasicBlock(nullptr, this->instruction_list);
    this->root = &main;

    this->CheckFor(SymbolTable::symbol_table.at("main")); // consumes `main` token

    if (this->CheckForOptional(SymbolTable::symbol_table.at("var"))) {
        #ifdef DEBUG
            std::cout << "\t[Parser::parse()]: next token is [varDecl]" << std::endl;
        #endif
        next(); // consumes `var` token
        // s->varDecl = parse_varDecl(); // ends with `;` = end of varDecl (consume it in the func)
        parse_varDecl();
        #ifdef DEBUG
            std::cout << "\t[Parser::parse()]: done parsing initial [varDecl]'s" << std::endl;
        #endif
    }
    
    if (this->CheckForMultipleOptionals(this->func_startTokens, FUNCTION_ST_SIZE)) {
        #ifdef DEBUG
            std::cout << "\t[Parser::parse()]: next token is [funcDecl]" << std::endl;
        #endif
        next(); // might change (bc func retType)
        // s->funcDecl = parse_funcDecl(); // ends with `;` = end of funcDecl (consume it in the func)
        parse_funcDecl();
    }
    
    #ifdef DEBUG
        std::cout << "\t[Parser::parse()]: next token is [statSeq]" << std::endl;
    #endif
    // if we cared about the parse tree, we'd keep the `{` and `}` like tokens, but we don't
    // more so trying to go as fast as we can: direct Abstract Syntax Tree (AST)
    this->CheckFor(SymbolTable::symbol_table.at("{")); // consumes `{`
    // s->statSeq = parse_statSeq();
    parse_statSeq(this->root);
    // next(); // do we need this?
    this->CheckFor(SymbolTable::symbol_table.at(".")); // consumes `.`
    #ifdef DEBUG
        std::cout << "\t[Parser::parse()]: Done; about to exit" << std::endl;
    #endif
}

// node::statSeq* Parser::parse_statSeq() {
//     node::statSeq *s;
//     s->head = parse_statement(); // ends with `;` = end of varDecl (consume it in the func)
//     return s;
// }

// node::statement* Parser::parse_statement() {
//     node::statement *s;

//     // statement starting tokens: [let, call, if, while, return]
//     // assume after every statement has a semicolon
//     this->CheckForMultiple(this->statement_startTokens, STATEMENT_ST_SIZE);
//     if (this->sym == this->statement_startTokens[0]) { // `let`
//         s->data->assignment_S = parse_assignment();
//     } else if (this->sym == this->statement_startTokens[1]) { // `call`
//         s->data->funcCall_S = parse_funcCall();
//     } else if (this->sym == this->statement_startTokens[2]) { // `if`
//         s->data->ifStat_S = parse_ifStatement();
//     } else if (this->sym == this->statement_startTokens[3]) { // `while`
//         s->data->whileStat_S = parse_whileStatement();
//     } else if (this->sym == this->statement_startTokens[4]) { // `return`
//         s->data->ret_S = parse_return();
//     }
//     next(); // do we need this?
//     if (this->CheckForOptional(SymbolTable::symbol_table.at(";"))) { // if [optional] next token is ';', then we still have statements
//         next();
//         // if we see any of these tokens then it means the next thing is another statement
//         if (this->CheckForMultipleOptionals(this->statement_startTokens, STATEMENT_ST_SIZE)) {
//             s->next = parse_statement();
//         }
//     } else {
//         s->next = nullptr;
//     }
//     return s;
// }

void Parser::parse_statSeq(BasicBlock *blk) {
    #ifdef DEBUG
        std::cout << "\t\t[Parser::parse_statSeq()]: found a statement to parse" << std::endl;
    #endif
    parse_statement(blk); // ends with `;` = end of varDecl (consume it in the func)


    next(); // do we need this?
    if (this->CheckForOptional(SymbolTable::symbol_table.at(";"))) { // if [optional] next token is ';', then we still have statements
        next();
        // if we see any of these tokens then it means the next thing is another statement
        if (this->CheckForMultipleOptionals(this->statement_startTokens, STATEMENT_ST_SIZE)) {
            parse_statSeq(blk);
        }
    }
}

void Parser::parse_statement(BasicBlock *blk) {
    // statement starting tokens: [let, call, if, while, return]
    // assume after every statement has a semicolon
    this->CheckForMultiple(this->statement_startTokens, STATEMENT_ST_SIZE);
    if (this->sym == this->statement_startTokens[0]) { // `let`
        parse_assignment(blk);
    } else if (this->sym == this->statement_startTokens[1]) { // `call`
        parse_funcCall(blk);
    } else if (this->sym == this->statement_startTokens[2]) { // `if`
        BasicBlock *join = parse_ifStatement(blk);
        if (join->parent2 == nullptr) { // means that [ifStat_then] was returned (no else)
            blk->children.push_back(join);
        }
    } else if (this->sym == this->statement_startTokens[3]) { // `while`
        BasicBlock while_blk = BasicBlock(blk, this->instruction_list);
        blk->children.push_back(&while_blk);
        parse_whileStatement(&while_blk);
    } else if (this->sym == this->statement_startTokens[4]) { // `return`
        parse_return(blk);
    }
    // next(); // do we need this?
    // if (this->CheckForOptional(SymbolTable::symbol_table.at(";"))) { // if [optional] next token is ';', then we still have statements
    //     next();
    //     // if we see any of these tokens then it means the next thing is another statement
    //     if (this->CheckForMultipleOptionals(this->statement_startTokens, STATEMENT_ST_SIZE)) {
    //         parse_statement(blk);
    //     }
    // }
}


// node::assignment* Parser::parse_assignment() { // this->curr->type == TOKEN_TYPE::LET
//     node::assignment *s;

//     // LET
//     this->CheckFor(SymbolTable::symbol_table.at("let"));
//     next();

//     // IDENT: not checking for a specific since this could be [variable]
//     s->ident = this->sym; 
//     next();

//     // ASSIGNMENT
//     this->CheckFor(SymbolTable::symbol_table.at("<-"));
//     next();

//     // EXPRESSION
//     s->expr = parse_expr();
    
//     // this->ssa_instructions.push_back(SSA::generate_ssa_instr());
//     return s;
// }

// node::funcCall* Parser::parse_funcCall() { // this->curr->type == TOKEN_TYPE::CALL
//     node::funcCall *s;
//     return s; // stub
// }


// node::ifStat* Parser::parse_ifStatement() { // this->curr->type == TOKEN_TYPE::IF
//     node::ifStat *s;
    
//     // IF
//     s->relation = parse_relation(); // IF: relation

//     // THEN
//     this->CheckFor(SymbolTable::symbol_table.at("then"));
//     s->then_statSeq = parse_statSeq();

//     // [Optional] ELSE
//     if (this->CheckForOptional(SymbolTable::symbol_table.at("else"))) {
//         next();
//         s->else_statSeq = parse_statSeq();
//     } else {
//         s->else_statSeq = nullptr;
//     }

//     // FI
//     this->CheckFor(SymbolTable::symbol_table.at("fi"));

//     return s;
// }

// node::whileStat* Parser::parse_whileStatement() { // this->curr->type == TOKEN_TYPE::WHILE
//     node::whileStat *s;

//     // WHILE
//     s->relation = parse_relation(); // WHILE: relation

//     // DO
//     this->CheckFor(SymbolTable::symbol_table.at("do"));
//     s->do_statSeq = parse_statSeq(); // DO: relation

//     // OD
//     this->CheckFor(SymbolTable::symbol_table.at("od"));

//     return s;
// }

// node::returnStat* Parser::parse_return() { // this->curr->type == TOKEN_TYPE::RETURN
//     node::returnStat *s;
//     s->expr = parse_expr(); // TODO: how to determine optional expr?
//     return s;
// }

void Parser::parse_assignment(BasicBlock *blk) { 
    // LET(TUCE GO)
    this->CheckFor(SymbolTable::symbol_table.at("let"));
    next();

    // IDENT: not checking for a specific since this could be [variable]
    // - validate that [variable] has been declared
    if (this->varDeclarations.find(this->sym) == this->varDeclarations.end()) {
        std::cout << "var [sym_table id: " << this->sym << "] doesn't exist! exiting prematurely..." << std::endl;
        exit(EXIT_FAILURE);
    }
    int ident = this->sym;
    next();

    // ASSIGNMENT
    this->CheckFor(SymbolTable::symbol_table.at("<-"));
    next();

    // EXPRESSION
    int value = parse_expr(blk);
    if (blk->updated_varval_map.find(ident) == blk->updated_varval_map.end()) {
        blk->updated_varval_map.insert({ident, value});
    } else {
        blk->updated_varval_map[ident] = value;
    }
}

void Parser::parse_funcCall(BasicBlock *blk) { 
    // TODO: 
}

// [BasicBlock *blk] gets modified & returned [BasicBlock] is the [join-blk]
BasicBlock* Parser::parse_ifStatement(BasicBlock *blk) {     
    // IF
    parse_relation(blk); // IF: relation

    // THEN
    this->CheckFor(SymbolTable::symbol_table.at("then"));
    BasicBlock *ifStat_then;
    BasicBlock if_then = BasicBlock(blk, this->instruction_list);
    ifStat_then = &if_then;
    blk->children.push_back(ifStat_then);
    parse_statSeq(ifStat_then);

    // [Optional] ELSE
    BasicBlock *ifStat_else = nullptr;
    if (this->CheckForOptional(SymbolTable::symbol_table.at("else"))) {
        next();
        BasicBlock if_else = BasicBlock(blk, this->instruction_list);
        ifStat_else = &if_else;
        blk->children.push_back(ifStat_else);
        parse_statSeq(ifStat_else);
    }

    // FI
    this->CheckFor(SymbolTable::symbol_table.at("fi"));

    // no need for phi since only one path! just return the [ifStat_then] block (i think)
    if (ifStat_else == nullptr) { return ifStat_then; }

    BasicBlock *join_ptr;
    BasicBlock join = BasicBlock(ifStat_then, ifStat_else, blk->updated_varval_map, this->instruction_list);
    join_ptr = &join;
    ifStat_then->children.push_back(join_ptr);
    ifStat_else->children.push_back(join_ptr);

    return join_ptr;
}


void Parser::parse_whileStatement(BasicBlock *blk) {
    // WHILE
    parse_relation(blk); // WHILE: relation

    // DO
    this->CheckFor(SymbolTable::symbol_table.at("do"));
    BasicBlock *while_body;
    BasicBlock body = BasicBlock(blk, this->instruction_list);
    while_body = &body;
    blk->children.push_back(while_body);
    while_body->children.push_back(blk); // technically a "child" since we loop back to it
    parse_statSeq(while_body); // DO: relation

    // OD
    this->CheckFor(SymbolTable::symbol_table.at("od"));
}

// what we supposed to do with this??? RAHHHHHH
void Parser::parse_return(BasicBlock *blk) {
    int val = parse_expr(blk);

    if (val == -1) { // no expr (optional)
        int op = SymbolTable::operator_table.at("ret");
        this->instruction_list.at(op).InsertAtHead(SSA(op, {val}));
        blk->instruction_list.at(op).InsertAtHead(SSA(op, {val}));
    }
}

// node::relation* Parser::parse_relation() {
//     node::relation *s;
//     s->exprA = parse_expr();
    
//     if (CheckForMultipleOptionals(this->relational_operations, RELATIONAL_OP_SIZE)) {
//         s->relOp = this->sym;
//         next();
//     }
//     s->exprB = parse_expr();
//     return s;
// }

void Parser::parse_relation(BasicBlock *blk) {
    std::vector<int> operands;

    int val1 = parse_expr(blk);
    this->CheckForMultiple(this->relational_operations, RELATIONAL_OP_SIZE);
    int relOp = this->sym;
    next(); // is this necessary?
    int val2 = parse_expr(blk);
    operands = {val1, val2};
    this->add_ssa_entry(SSA(5, operands));

    int op;
    switch (relOp) {
        case relational_operations_arr[0]:
            op = SymbolTable::operator_table.at("blt");
            break;
        case relational_operations_arr[1]:
            op = SymbolTable::operator_table.at("bgt");
            break;
        case relational_operations_arr[2]:
            op = SymbolTable::operator_table.at("ble");
            break;
        case relational_operations_arr[3]:
            op = SymbolTable::operator_table.at("bge");
            break;
        case relational_operations_arr[4]:
            op = SymbolTable::operator_table.at("beq");
            break;
        default:
            std::cout << "relation op [sym_table id: " << this->sym << "] doesn't exist! exiting prematurely..." << std::endl;
            exit(EXIT_FAILURE);
            break;
    }
    operands = {curr_instr_num, -1}; // [-1] bc we don't know the specific instr yet
    this->add_ssa_entry(SSA(op, operands));
}
// return: int
// maintain root node and then return that root node 
// node::expr* Parser::parse_expr() { // this->sym = first token in expr
//     node::expr *s;

//     s->A = parse_term();
//     if (s->A == nullptr) { return nullptr; }

//     // OLD COMMENT
//     // if an [OP] exists, then [termB] also exists! put it as [termA] of new [expr]!
//     if (this->CheckForMultipleOptionals(this->expression_operations, EXPRESSION_OP_SIZE)) {
//         s->op = this->sym;
//         next();
//         s->B->e = parse_expr();
//     } else {
//         s->op = -1;
//         s->B = nullptr;
//     }
//     return s;
// }

int Parser::parse_expr(BasicBlock *blk) { 
    int val1 = parse_term(blk);
    if (this->CheckForIdentifier(val1)) {
        if (blk->updated_varval_map.find(val1) != blk->updated_varval_map.end()) {
            val1 = blk->updated_varval_map.at(val1);
        } else {
            // std::cout << "expr [sym_table id: " << val1 << "] doesn't exist! exiting prematurely..." << std::endl;
            // std::cout << "\tblk's [updated_varval_map] looks like: " << std::endl;
            // for (const auto& pair : blk->updated_varval_map) {
            //     std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
            // }
            // exit(EXIT_FAILURE);
            return -1;
        }
    }

    // OLD COMMENT
    // if an [OP] exists, then [termB] also exists! put it as [termA] of new [expr]!
    if (this->CheckForMultipleOptionals(this->expression_operations, EXPRESSION_OP_SIZE)) {
        int op = this->sym;
        next();
        int val2 = parse_expr(blk);
        if (this->CheckForIdentifier(val2)) {
            // if it's not a number then it's an ident
            if (blk->updated_varval_map.find(val2) != blk->updated_varval_map.end()) {
                return val1 + blk->updated_varval_map.at(val2);
            } else {
                std::cout << "expr [sym_table id: " << val2 << "] doesn't exist! exiting prematurely..." << std::endl;
                std::cout << "\tblk's [updated_varval_map] looks like: " << std::endl;
                for (const auto& pair : blk->updated_varval_map) {
                    std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
                }
                exit(EXIT_FAILURE);
            }
        }

        if (SymbolTable::symbol_table.at("+") == op) {
            int operatorr = SymbolTable::operator_table.at("add");
            this->instruction_list.at(operatorr).InsertAtHead(SSA(operatorr, {val1, val2}));
            blk->instruction_list.at(operatorr).InsertAtHead(SSA(operatorr, {val1, val2}));
            
            return val1 + val2;
        } else if (SymbolTable::symbol_table.at("-") == op) {
            int operatorr = SymbolTable::operator_table.at("sub");
            this->instruction_list.at(operatorr).InsertAtHead(SSA(operatorr, {val1, val2}));
            blk->instruction_list.at(operatorr).InsertAtHead(SSA(operatorr, {val1, val2}));
            
            return val1 - val2;
        }
    }
    return val1;
}

// node::term* Parser::parse_term() {
//     node::term *s;

//     s->A = parse_factor();
//     if (s->A == nullptr) { return nullptr; }

//     if (this->CheckForMultipleOptionals(this->term_operations, TERM_OP_SIZE)) {
//         s->op = this->sym;
//         next();
//         s->B->f = parse_factor();
//     } else {
//         s->op = -1;
//         s->B = nullptr;
//     }
//     return s;
// }

int Parser::parse_term(BasicBlock *blk) {
    int val1 = parse_factor(blk);
    if (this->CheckForIdentifier(val1)) {
        if (blk->updated_varval_map.find(val1) != blk->updated_varval_map.end()) {
            val1 = blk->updated_varval_map.at(val1);
        } else {
            std::cout << "term [sym_table id: " << val1 << "] doesn't exist! exiting prematurely..." << std::endl;
            std::cout << "\tblk's [updated_varval_map] looks like: " << std::endl;
            for (const auto& pair : blk->updated_varval_map) {
                std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
            }
            exit(EXIT_FAILURE);
        }
    }

    if (this->CheckForMultipleOptionals(this->term_operations, TERM_OP_SIZE)) {
        int op = this->sym;
        next();
        int val2 = parse_factor(blk);
        if (this->CheckForIdentifier(val2)) {
            if (blk->updated_varval_map.find(val2) != blk->updated_varval_map.end()) {
                val2 = blk->updated_varval_map.at(val2);
            } else {
                std::cout << "term [sym_table id: " << val2 << "] doesn't exist! exiting prematurely..." << std::endl;
                std::cout << "\tblk's [updated_varval_map] looks like: " << std::endl;
                for (const auto& pair : blk->updated_varval_map) {
                    std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
                }
                exit(EXIT_FAILURE);
            }
        }

        if (SymbolTable::symbol_table.at("*") == op) {
            this->instruction_list.at(SymbolTable::operator_table.at("mul")).InsertAtHead(SSA(SymbolTable::operator_table.at("mul"), {val1, val2}));
            blk->instruction_list.at(SymbolTable::operator_table.at("mul")).InsertAtHead(SSA(SymbolTable::operator_table.at("mul"), {val1, val2}));
            
            return val1 + val2;
        } else if (SymbolTable::symbol_table.at("/") == op) {
            this->instruction_list.at(SymbolTable::operator_table.at("div")).InsertAtHead(SSA(SymbolTable::operator_table.at("div"), {val1, val2}));
            blk->instruction_list.at(SymbolTable::operator_table.at("div")).InsertAtHead(SSA(SymbolTable::operator_table.at("div"), {val1, val2}));
            
            return val1 - val2;
        }
    }
    return val1;
}

// node::factor* Parser::parse_factor() {
//     node::factor *s;

//     if (this->CheckForOptional(SymbolTable::symbol_table.at("("))) { // expr
//         s->data_type = factor_data::EXPR_F;
//         next();
//         s->data->expr->e = parse_expr();
//         this->CheckFor(SymbolTable::symbol_table.at(")"));
//     } else if (this->CheckForOptional(SymbolTable::symbol_table.at("call"))) { // funcCall
//         s->data_type = factor_data::FUNCCALL_F;
//         s->data->funcCall = parse_funcCall();
//     } else {
//         // either [ident] or [number]
//         if (this->CheckForIdentifier()) { // ident
//             s->data_type = factor_data::IDENT_F;
//             s->data->ident = this->sym;
//             next();
//         } else if (this->CheckForNumber()) { // number
//             s->data_type = factor_data::NUM_F;
//             s->data->num = this->sym;
//             next();
//         } else { return nullptr; } // else it's not a factor!
//     }
//     return s;
// }

int Parser::parse_factor(BasicBlock *blk) {
    if (this->CheckForOptional(SymbolTable::symbol_table.at("("))) { // expr
        next();
        int val1 = parse_expr(blk);
        this->CheckFor(SymbolTable::symbol_table.at(")"));
        return val1;
    } else if (this->CheckForOptional(SymbolTable::symbol_table.at("call"))) { // funcCall
        parse_funcCall(blk);
    } else {
        // either [ident] or [number]
        int res = this->sym;
        if (this->CheckForIdentifier()) { // ident
            next();
            return blk->updated_varval_map.at(res);
        } else if (this->CheckForNumber()) { // number
            next();
            return res;
        } else { 
            std::cout << "term [sym_table id: " << res << "] doesn't exist! exiting prematurely..." << std::endl;
            exit(EXIT_FAILURE);    
        } // else it's not a factor!
    }
}

// node::funcDecl* Parser::parse_funcDecl() {
//     node::funcDecl *s;
//     return s; // stub
// }

void Parser::parse_funcDecl() {
    // TODO: add into symbol_table (?) & [this->funcDeclarations]

    // node::funcDecl *s;
    // return s; // stub
}

// node::varDecl* Parser::parse_varDecl() { // this->curr->type == TOKEN_TYPE::VAR
//     node::varDecl *s;
//     s->head = parse_vars();
//     this->CheckFor(SymbolTable::symbol_table.at(";"));
//     return s;
// }

// node::var* Parser::parse_vars() { // this->curr->type == TOKEN_TYPE::IDENTIFIER
//     node::var *s;
//     s->ident = this->sym;
//     this->varDeclarations.push_back(this->sym);
//     next();

//     // existence of a `,` indicates a next variable exists
//     if (this->CheckForOptional(SymbolTable::symbol_table.at(","))) {
//         next(); // consuming the `,`
//         s->next = parse_vars();
//     }
//     return s;
// }


void Parser::parse_varDecl() { 
    // node::varDecl *s;
    // s->head = parse_vars();
    #ifdef DEBUG
        std::cout << "\t\t[Parser::parse_varDecl()]: got new var: [" << this->sym << "]" << std::endl;
    #endif
    this->varDeclarations.insert(this->sym);
    next();

    // existence of a `,` indicates a next variable exists
    if (this->CheckForOptional(SymbolTable::symbol_table.at(","))) {
        next(); // consuming the `,`
        // s->next = parse_vars();
        parse_varDecl();
    }
    this->CheckFor(SymbolTable::symbol_table.at(";"));
    // return s;
}


// void Parser::parse_varDecl() { 
//     // node::varDecl *s;
//     // s->head = parse_vars();
//     parse_vars();
//     this->CheckFor(SymbolTable::symbol_table.at(";"));
//     // return s;
// }

// void Parser::parse_vars() { 
//     // node::var *s;
//     // s->ident = this->sym;
//     // node::varDecl *s;
//     this->varDeclarations.insert(this->sym);
//     next();

//     // existence of a `,` indicates a next variable exists
//     if (this->CheckForOptional(SymbolTable::symbol_table.at(","))) {
//         next(); // consuming the `,`
//         // s->next = parse_vars();
//         parse_vars();
//     }
//     // return s;
// }


/*
whether or not this will actually work is another story
expected action: consume and return [node::ident]
node::ident* Parser::parse_ident() { // this->curr->type == TOKEN_TYPE::IDENTIFIER
    node::ident *s = new node::ident(this->curr);
    consume();
    return s;
}
*/






// Helper function to generate IR for binary operations
// std::string Parser::generateIRBinaryOperation(const std::string& left, const std::string& right, TOKEN_TYPE op) {
//     std::string result = "t" + std::to_string(ir.size()); // Temporary variable name
//     ir.push_back({result, getTokenString(op), left, right}); // Generate IR for binary operation
//     return result;
// }

// Helper function to perform copy propagation
std::string Parser::propagateCopy(const std::string& result) {
    for (auto& entry : symbolTable) {
        std::string& value = entry.second;
        if (value == result) {
            return entry.first; // Replace use with value
        }
    }
    return result;
}

// Helper function to get string representation of token
// std::string Parser::getTokenString(TOKEN_TYPE type) {
//     switch (type) {
//         case PLUS: return "+";
//         case MINUS: return "-";
//         case MULTIPLY: return "*";
//         case DIVIDE: return "/";
//         case SEMICOLON: return ";";
//         case PRINT: return "PRINT"; // TODO: fix this
//         default: return "";
//     }
// }
