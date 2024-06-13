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
    // node::main *s; // expected start: [main]
    BasicBlock *main = BasicBlock(nullptr, this->instruction_list);
    this->root = main;

    this->CheckFor(SymbolTable::symbol_table.at("main")); // consumes `main` token

    if (this->CheckForOptional(SymbolTable::symbol_table.at("var"))) {
        next();
        // s->varDecl = parse_varDecl(); // ends with `;` = end of varDecl (consume it in the func)
        parse_varDecl();
    }
    
    if (this->CheckForMultipleOptionals(this->func_startTokens, FUNCTION_ST_SIZE)) {
        next(); // might change (bc func retType)
        // s->funcDecl = parse_funcDecl(); // ends with `;` = end of funcDecl (consume it in the func)
        parse_funcDecl();
    }
    
    // if we cared about the parse tree, we'd keep the `{` and `}` like tokens, but we don't
    // more so trying to go as fast as we can: direct Abstract Syntax Tree (AST)
    this->CheckFor(SymbolTable::symbol_table.at("{")); // consumes `{`
    // s->statSeq = parse_statSeq();
    parse_statSeq(main);
    // next(); // do we need this?
    this->CheckFor(SymbolTable::symbol_table.at(".")); // consumes `.`
    
    // this->root = s; // head-node of AST
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
    parse_statement(blk); // ends with `;` = end of varDecl (consume it in the func)
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
        parse_ifStatement(blk);
    } else if (this->sym == this->statement_startTokens[3]) { // `while`
        parse_whileStatement(blk);
    } else if (this->sym == this->statement_startTokens[4]) { // `return`
        parse_return();
    }
    next(); // do we need this?
    if (this->CheckForOptional(SymbolTable::symbol_table.at(";"))) { // if [optional] next token is ';', then we still have statements
        next();
        // if we see any of these tokens then it means the next thing is another statement
        if (this->CheckForMultipleOptionals(this->statement_startTokens, STATEMENT_ST_SIZE)) {
            parse_statement();
        }
    }
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
    int value = parse_expr();
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
    BasicBlock *ifStat_then = BasicBlock(blk, this->instruction_list);
    blk->add_child_blk(ifStat_then);
    parse_statSeq(ifStat_then);

    // [Optional] ELSE
    BasicBLock *ifStat_else = nullptr;
    if (this->CheckForOptional(SymbolTable::symbol_table.at("else"))) {
        next();
        ifStat_else = BasicBlock(blk, this->instruction_list);
        blk->add_child_blk(ifStat_else);
        parse_statSeq(ifStat_else);
    }

    // FI
    this->CheckFor(SymbolTable::symbol_table.at("fi"));

    // no need for phi since only one path! just return the [ifStat_then] block (i think)
    if (ifStat_else == nullptr) { return ifStat_then; }

    BasicBlock *join = BasicBlock(ifStat_then, ifStat_else, this->instruction_list);
    ifStat_then->add_child_blk(join);
    ifStat_else->add_child_blk(join);
}


node::whileStat* Parser::parse_whileStatement() { // this->curr->type == TOKEN_TYPE::WHILE
    node::whileStat *s;

    // WHILE
    s->relation = parse_relation(); // WHILE: relation

    // DO
    this->CheckFor(SymbolTable::symbol_table.at("do"));
    s->do_statSeq = parse_statSeq(); // DO: relation

    // OD
    this->CheckFor(SymbolTable::symbol_table.at("od"));

    return s;
}

node::returnStat* Parser::parse_return() { // this->curr->type == TOKEN_TYPE::RETURN
    node::returnStat *s;
    s->expr = parse_expr(); // TODO: how to determine optional expr?
    return s;
}

node::relation* Parser::parse_relation() {
    node::relation *s;
    s->exprA = parse_expr();
    
    if (CheckForMultipleOptionals(this->relational_operations, RELATIONAL_OP_SIZE)) {
        s->relOp = this->sym;
        next();
    }
    s->exprB = parse_expr();
    return s;
}

// maintain root node and then return that root node 
node::expr* Parser::parse_expr() { // this->sym = first token in expr
    node::expr *s;

    s->A = parse_term();
    if (s->A == nullptr) { return nullptr; }

    // OLD COMMENT
    // if an [OP] exists, then [termB] also exists! put it as [termA] of new [expr]!
    if (this->CheckForMultipleOptionals(this->expression_operations, EXPRESSION_OP_SIZE)) {
        s->op = this->sym;
        next();
        s->B->e = parse_expr();
    } else {
        s->op = -1;
        s->B = nullptr;
    }
    return s;
}

node::term* Parser::parse_term() {
    node::term *s;

    s->A = parse_factor();
    if (s->A == nullptr) { return nullptr; }

    if (this->CheckForMultipleOptionals(this->term_operations, TERM_OP_SIZE)) {
        s->op = this->sym;
        next();
        s->B->f = parse_factor();
    } else {
        s->op = -1;
        s->B = nullptr;
    }
    return s;
}

node::factor* Parser::parse_factor() {
    node::factor *s;

    if (this->CheckForOptional(SymbolTable::symbol_table.at("("))) { // expr
        s->data_type = factor_data::EXPR_F;
        next();
        s->data->expr->e = parse_expr();
        this->CheckFor(SymbolTable::symbol_table.at(")"));
    } else if (this->CheckForOptional(SymbolTable::symbol_table.at("call"))) { // funcCall
        s->data_type = factor_data::FUNCCALL_F;
        s->data->funcCall = parse_funcCall();
    } else {
        // either [ident] or [number]
        if (this->CheckForIdentifier()) { // ident
            s->data_type = factor_data::IDENT_F;
            s->data->ident = this->sym;
            next();
        } else if (this->CheckForNumber()) { // number
            s->data_type = factor_data::NUM_F;
            s->data->num = this->sym;
            next();
        } else { return nullptr; } // else it's not a factor!
    }
    return s;
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
    parse_vars();
    this->CheckFor(SymbolTable::symbol_table.at(";"));
    // return s;
}

void Parser::parse_vars() { 
    // node::var *s;
    // s->ident = this->sym;
    this->varDeclarations.push_back(this->sym);
    next();

    // existence of a `,` indicates a next variable exists
    if (this->CheckForOptional(SymbolTable::symbol_table.at(","))) {
        next(); // consuming the `,`
        // s->next = parse_vars();
        parse_vars();
    }
    // return s;
}


/*
whether or not this will actually work is another story
expected action: consume and return [node::ident]
node::ident* Parser::parse_ident() { // this->curr->type == TOKEN_TYPE::IDENTIFIER
    node::ident *s = new node::ident(this->curr);
    consume();
    return s;
}
*/











// // Statement parsing functions
// void Parser::parseStatement() {
//     if (curr.type == LET) {
//         Parser::parseDeclaration();
//     } else if (curr.type == IDENTIFIER) {
//         Parser::parseAssignment();
//     } else if (curr.type == PRINT) {
//         Parser::parsePrintStatement();
//     } else {
//         std::cerr << "Syntax error: Unexpected token " << curr.lexeme << std::endl;
//         curr = Parser::getNextToken(); // Skip token
//     }
// }

// // Declaration parsing function
// void Parser::parseDeclaration() {
//     consume(LET); // Consume 'let' keyword
//     std::string identifier = curr.lexeme;
//     consume(IDENTIFIER); // Consume identifier
//     // consume('='); // Consume '='
//     std::string expressionResult = parseExpression();
//     symbolTable[identifier] = expressionResult; // Store result in symbol table for copy propagation
//     consume(SEMICOLON); // Consume ';'
// }

// // Assignment parsing function
// void Parser::parseAssignment() {
//     std::string identifier = curr.lexeme;
//     consume(IDENTIFIER); // Consume identifier
//     // consume('='); // Consume '='
//     std::string expressionResult = parseExpression();
//     symbolTable[identifier] = expressionResult; // Store result in symbol table for copy propagation
//     consume(SEMICOLON); // Consume ';'
// }

// // Print statement parsing function
// void Parser::parsePrintStatement() {
//     consume(PRINT); // Consume 'print' keyword
//     std::string expressionResult = parseExpression();
//     ir.push_back({"", "print", expressionResult, ""}); // Generate IR for print statement
//     consume(SEMICOLON); // Consume ';'
// }

// // Expression parsing function
// std::string Parser::parseExpression() {
//     std::string left = parseTerm();
//     while (curr.type == PLUS || curr.type == MINUS) {
//         TOKEN_TYPE op = curr.type;
//         consume(op); // Consume operator
//         std::string right = parseTerm();
//         // Generate IR for addition or subtraction
//         std::string result = generateIRBinaryOperation(left, right, op);
//         // Perform copy propagation
//         left = propagateCopy(result);
//     }
//     return left;
// }

// // Term parsing function
// std::string Parser::parseTerm() {
//     std::string left = parseFactor();
//     while (curr.type == MULTIPLY || curr.type == DIVIDE) {
//         TOKEN_TYPE op = curr.type;
//         consume(op); // Consume operator
//         std::string right = parseFactor();
//         // Generate IR for multiplication or division
//         std::string result = generateIRBinaryOperation(left, right, op);
//         // Perform copy propagation
//         left = propagateCopy(result);
//     }
//     return left;
// }

// // Factor parsing function
// std::string Parser::parseFactor() {
//     if (curr.type == IDENTIFIER || curr.type == NUMBER) {
//         std::string factorResult = curr.lexeme;
//         consume(curr.type); // Consume identifier or number
//         return factorResult;
//     // } else if (curr.type == '(') {
//         // consume('('); // Consume '('
//         std::string expressionResult = parseExpression();
//         // consume(')'); // Consume ')'
//         // return expressionResult;
//     } else {
//         std::cerr << "Syntax error: Unexpected token " << curr.lexeme << std::endl;
//         curr = getNextToken(); // Skip token
//         return ""; // Return empty string for error recovery
//     }
// }

// Helper function to generate IR for binary operations
std::string Parser::generateIRBinaryOperation(const std::string& left, const std::string& right, TOKEN_TYPE op) {
    std::string result = "t" + std::to_string(ir.size()); // Temporary variable name
    ir.push_back({result, getTokenString(op), left, right}); // Generate IR for binary operation
    return result;
}

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
std::string Parser::getTokenString(TOKEN_TYPE type) {
    switch (type) {
        case PLUS: return "+";
        case MINUS: return "-";
        case MULTIPLY: return "*";
        case DIVIDE: return "/";
        case SEMICOLON: return ";";
        case PRINT: return "PRINT"; // TODO: fix this
        default: return "";
    }
}




// int main() {
//     std::string source = "let a = 3 + 5 * (4 - 2); print a;";
//     Parser parser(source);
//     parser.parse_FIRSTPASS();
//     std::vector<IRNode> ir = parser.getIR();

//     // Perform common subexpression elimination (CSE)
//     performCSE(ir);

//     // Print IR
//     printIR(ir);

//     return 0;
// }