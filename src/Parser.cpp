#include "Parser.hpp"



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


// assume input file has no errors
void Parser::parse_FIRSTPASS() {
    node::computation *s;
    this->curr = next(); // expected start: [main]

    if (this->curr->type == TOKEN_TYPE::MAIN) {
        s->terminal_sym_main = this->curr;
        consume();
        this->curr = next();
    } else {
        fprintf(stderr, "expected computation to start with [MAIN]!\n");
        exit(EXIT_FAILURE);
    }

    if (this->curr->type == TOKEN_TYPE::VAR) {
        s->varDecl = parse_varDecl(); // ends with `;` = end of varDecl (consume it in the func)
        this->curr = next();
    }
    if (this->curr->type == TOKEN_TYPE::FUNCTION || 
        this->curr->type == TOKEN_TYPE::VOID) {
        s->funcDecl = parse_funcDecl(); // ends with `;` = end of funcDecl (consume it in the func)
        this->curr = next();
    }
    
    // if we cared about the parse tree, we'd keep the `{` and `}` like tokens, but we don't
    // more so trying to go as fast as we can: direct Abstract Syntax Tree (AST)
    if (this->curr->type == TOKEN_TYPE::OPEN_CURLY) {        
        consume(); // `{`
        s->statSeq = parse_statSeq();
        consume(); // `}`
    }
    this->curr = next();
    if (this->curr->type == TOKEN_TYPE::END_OF_FILE) {
        s->terminal_sym_eof = this->curr;
    }
    
    this->root = s; // head-node of AST
}

node::statSeq* Parser::parse_statSeq() {
    node::statSeq *s;
    s->head = parse_statement(); // ends with `;` = end of varDecl (consume it in the func)
    return s;
}

node::statement* Parser::parse_statement() {
    node::statement *s;

    // statement starting tokens: [let, call, if, while, return]
    // assume after every statement has a semicolon
    switch (this->curr->type) {
        case TOKEN_TYPE::LET:
            s->data->assignment_S = parse_assignment();
            break;
        case TOKEN_TYPE::CALL:
            s->data->funcCall_S = parse_funcCall();
            break;
        case TOKEN_TYPE::IF:
            s->data->ifStat_S = parse_ifStatement();
            break;
        case TOKEN_TYPE::WHILE:
            s->data->whileStat_S = parse_whileStatement();
            break;
        case TOKEN_TYPE::RETURN:
            s->data->ret_S = parse_return();
            consume();
            this->curr = next();
            break;
        default:
            break;
    };
    s->terminal_sym_semi = next(); // assume `;` comes after all statements
    consume();
    this->curr = next();
    if (this->curr->type == TOKEN_TYPE::LET || this->curr->type == TOKEN_TYPE::CALL ||
        this->curr->type == TOKEN_TYPE::IF || this->curr->type == TOKEN_TYPE::WHILE || 
        this->curr->type == TOKEN_TYPE::RETURN) {
            // if we see any of these tokens then it means the next thing is another statement
            s->next = parse_statement();
    }
    return s;
}

node::assignment* Parser::parse_assignment() { // this->curr->type == TOKEN_TYPE::LET
    node::assignment *s;

    // LET
    s->terminal_sym_let = this->curr;
    consume();
    this->curr = next();

    // IDENT
    s->ident = this->curr;
    consume();
    this->curr = next();

    // ASSIGNMENT
    s->terminal_sym_assignment = this->curr;
    consume();
    this->curr = next();

    // EXPRESSION
    s->expr = parse_expr();
}

node::funcCall* Parser::parse_funcCall() { // this->curr->type == TOKEN_TYPE::CALL

}


node::ifStat* Parser::parse_ifStatement() { // this->curr->type == TOKEN_TYPE::IF

}


node::whileStat* Parser::parse_whileStatement() { // this->curr->type == TOKEN_TYPE::WHILE

}


node::returnStat* Parser::parse_return() { // this->curr->type == TOKEN_TYPE::RETURN

}

// maintain root node and then return that root node 
node::expr* Parser::parse_expr() { // this->curr = first token in expr
    node::expr *s;

    s->A->t = parse_term();
    this->curr = next();

    // if an [OP] exists, then [termB] also exists! put it as [termA] of new [expr]!
    if (this->curr->type < 0) { // TOKEN_TYPE::[OP] < 0 (i.e. PLUS, MINUS, MULTIPLY, DIVIDE)
        node::expr *ss;
        ss->A->t = s->A->t;
        s->A->t = nullptr;
        ss->op = this->curr;
        consume();

        s->A->e = ss;
        this->curr = next();
        ss->B->e = parse_expr();
    } else {
        s->op = nullptr;
    }
}


node::funcDecl* Parser::parse_funcDecl() {}

node::varDecl* Parser::parse_varDecl() { // this->curr->type == TOKEN_TYPE::VAR
    node::varDecl *s;
    s->terminal_sym_var = this->curr;
    consume(); // `VAR`
    this->curr = next();
    s->head = parse_vars();
    consume(); // `;` = end of varDecl

    return s;
}

node::var* Parser::parse_vars() { // this->curr->type == TOKEN_TYPE::IDENTIFIER
    node::var *s;

    s->ident = this->curr;
    consume();
    this->curr = next();

    // existence of a `,` indicates a next variable exists
    if (this->curr->type == TOKEN_TYPE::COMMA) {
        consume();
        if (this->curr->type == TOKEN_TYPE::IDENTIFIER) {
            s->next = parse_vars();
        }
    }
    return s;
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





















// Statement parsing functions
void Parser::parseStatement() {
    if (curr.type == LET) {
        Parser::parseDeclaration();
    } else if (curr.type == IDENTIFIER) {
        Parser::parseAssignment();
    } else if (curr.type == PRINT) {
        Parser::parsePrintStatement();
    } else {
        std::cerr << "Syntax error: Unexpected token " << curr.lexeme << std::endl;
        curr = Parser::getNextToken(); // Skip token
    }
}

// Declaration parsing function
void Parser::parseDeclaration() {
    consume(LET); // Consume 'let' keyword
    std::string identifier = curr.lexeme;
    consume(IDENTIFIER); // Consume identifier
    // consume('='); // Consume '='
    std::string expressionResult = parseExpression();
    symbolTable[identifier] = expressionResult; // Store result in symbol table for copy propagation
    consume(SEMICOLON); // Consume ';'
}

// Assignment parsing function
void Parser::parseAssignment() {
    std::string identifier = curr.lexeme;
    consume(IDENTIFIER); // Consume identifier
    // consume('='); // Consume '='
    std::string expressionResult = parseExpression();
    symbolTable[identifier] = expressionResult; // Store result in symbol table for copy propagation
    consume(SEMICOLON); // Consume ';'
}

// Print statement parsing function
void Parser::parsePrintStatement() {
    consume(PRINT); // Consume 'print' keyword
    std::string expressionResult = parseExpression();
    ir.push_back({"", "print", expressionResult, ""}); // Generate IR for print statement
    consume(SEMICOLON); // Consume ';'
}

// Expression parsing function
std::string Parser::parseExpression() {
    std::string left = parseTerm();
    while (curr.type == PLUS || curr.type == MINUS) {
        TOKEN_TYPE op = curr.type;
        consume(op); // Consume operator
        std::string right = parseTerm();
        // Generate IR for addition or subtraction
        std::string result = generateIRBinaryOperation(left, right, op);
        // Perform copy propagation
        left = propagateCopy(result);
    }
    return left;
}

// Term parsing function
std::string Parser::parseTerm() {
    std::string left = parseFactor();
    while (curr.type == MULTIPLY || curr.type == DIVIDE) {
        TOKEN_TYPE op = curr.type;
        consume(op); // Consume operator
        std::string right = parseFactor();
        // Generate IR for multiplication or division
        std::string result = generateIRBinaryOperation(left, right, op);
        // Perform copy propagation
        left = propagateCopy(result);
    }
    return left;
}

// Factor parsing function
std::string Parser::parseFactor() {
    if (curr.type == IDENTIFIER || curr.type == NUMBER) {
        std::string factorResult = curr.lexeme;
        consume(curr.type); // Consume identifier or number
        return factorResult;
    // } else if (curr.type == '(') {
        // consume('('); // Consume '('
        std::string expressionResult = parseExpression();
        // consume(')'); // Consume ')'
        // return expressionResult;
    } else {
        std::cerr << "Syntax error: Unexpected token " << curr.lexeme << std::endl;
        curr = getNextToken(); // Skip token
        return ""; // Return empty string for error recovery
    }
}

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