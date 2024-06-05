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


// we assume lexing found errors; iff we've reached this step, assume this program is syntactically accurate
node::computation* Parser::parse() {
    TOKEN *curr = next();
    node::computation *c;

    if (curr == nullptr) { exit(EXIT_FAILURE); }

    // ASSUMPTION: if (curr->type == TOKEN_TYPE::MAIN) {
    terminal_n *main_t = terminal_n{ curr->lexme; };
    c->terminal_sym_main = *main_t;
    consume();

    c->varDecl = parse_varDecl();
    c->funcDecl = parse_funcDecl();
    c->statSeq = parse_statSeq();
    
    curr = next();
    terminal_n *eof_t = terminal_n{ curr->lexme; };
    c->terminal_sym_eof = eof_t;
    
    return c;
}

node::var* Parser::parse_var() {
    TOKEN *curr = next();
    node::var *v;

    if (curr == nullptr) { exit(EXIT_FAILURE); }

    // ASSUMPTION: if (curr->type == TOKEN_TYPE::IDENT) {
    ident *ident_t = ident{ terminal_n{ curr->lexme; } };
    v->ident_VD  = ident_t;
    consume();
    curr = next();

    if (curr->type == TOKEN_TYPE::COMMA) {
        v->terminal_sym_comma = terminal_n{ curr->lexme; };
        consume();
        v->next = parse_var();
    } else if (curr->type == TOKEN_TYPE::SEMICOLON) {
        v->terminal_sym_comma = terminal_n{ curr->lexme; };
        v->next = nullptr;
        consume();
    }
    return v;
}

// if there's a [VAR] token we'll parse the variables
node::varDecl* Parser::parse_varDecl() {
    TOKEN *curr = next();
    node::varDecl *v;

    if (curr == nullptr) { exit(EXIT_FAILURE); }

    if (curr->type == TOKEN_TYPE::VAR) {
        terminal_n *var_t = terminal_n{ curr->lexme; };
        v->terminal_sym_var = var_t;
        consume();

        v->head = parse_var();
    }
    return v;
}

// TODO: functions later
// [ASSUMPTION]: [void && non-void] func ret types will have the same order (retType, `function`, funcName)
node::funcDecl* Parser::parse_funcDecl() {
    TOKEN *curr = next();
    node::funcDecl *fd;
    
    if (curr == nullptr) { exit(EXIT_FAILURE); }

    // if (curr-type == ) {

    // } else { 
    //     exit(EXIT_FAILURE); 
    // }

    return fd;
}

node::statement* Parser::parse_statement() {
    TOKEN *curr = next();
    node::statement *s;
    
    if (curr == nullptr) { exit(EXIT_FAILURE); }

    if (statement_start_KEYWORDS.contains(curr->type)) { // if it's a [statement]
        // determine [data] type
        switch (curr->type) {
            case TOKEN_TYPE::LET:
                s->data->assignment_S = parse_assignment();
                break;
            case TOKEN_TYPE::CALL:
                s->data->funcCall_S = parse_funcCall();
                break;
            case TOKEN_TYPE::IF:
                s->data->ifStat_S = parse_ifStat();
                break;
            case TOKEN_TYPE::WHILE:
                s->data->whileStat = parse_whileStat();
                break;
            case TOKEN_TYPE::RETURN:
                s->data->ret_S = parse_ret();
                break;
            default:
                exit(EXIT_FAILURE);
                break;
        };

        if ((curr = next())->type == TOKEN_TYPE::SEMICOLON) { // having a semicolon indicates a guarantee that we have a next statement (i.e. we can continue parsing THIS [statSeq])
            terminal_n *semi_t = terminal_n{ curr->lexme; };
            s->terminal_sym_semi = *semi_t;
            consume();
            s->next = parse_statement();
        } else {
            s->terminal_sym_semi = nullptr;
            s->next = nullptr;
        }
    } else {
        return nullptr;
    }
    // else, no statements left to parse
    return s;
}

node::statSeq* Parser::parse_statSeq() {
    node::statSeq *ss;
    ss->head = parse_statement();
    return ss;
}



// // Get intermediate representation (IR)
// std::vector<IRNode> Parser::getIR() {
//     return ir;
// }


// // Statement parsing functions
// void Parser::parseStatement() {
//     if (currentToken.type == LET) {
//         Parser::parseDeclaration();
//     } else if (currentToken.type == IDENTIFIER) {
//         Parser::parseAssignment();
//     } else if (currentToken.type == PRINT) {
//         Parser::parsePrintStatement();
//     } else {
//         std::cerr << "Syntax error: Unexpected token " << currentToken.lexeme << std::endl;
//         currentToken = Parser::getNextToken(); // Skip token
//     }
// }

// // Declaration parsing function
// void Parser::parseDeclaration() {
//     consume(LET); // Consume 'let' keyword
//     std::string identifier = currentToken.lexeme;
//     consume(IDENTIFIER); // Consume identifier
//     // consume('='); // Consume '='
//     std::string expressionResult = parseExpression();
//     symbolTable[identifier] = expressionResult; // Store result in symbol table for copy propagation
//     consume(SEMICOLON); // Consume ';'
// }

// // Assignment parsing function
// void Parser::parseAssignment() {
//     std::string identifier = currentToken.lexeme;
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
//     while (currentToken.type == PLUS || currentToken.type == MINUS) {
//         TOKEN_TYPE op = currentToken.type;
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
//     while (currentToken.type == MULTIPLY || currentToken.type == DIVIDE) {
//         TOKEN_TYPE op = currentToken.type;
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
//     if (currentToken.type == IDENTIFIER || currentToken.type == NUMBER) {
//         std::string factorResult = currentToken.lexeme;
//         consume(currentToken.type); // Consume identifier or number
//         return factorResult;
//     // } else if (currentToken.type == '(') {
//         // consume('('); // Consume '('
//         std::string expressionResult = parseExpression();
//         // consume(')'); // Consume ')'
//         // return expressionResult;
//     } else {
//         std::cerr << "Syntax error: Unexpected token " << currentToken.lexeme << std::endl;
//         currentToken = getNextToken(); // Skip token
//         return ""; // Return empty string for error recovery
//     }
// }

// // Helper function to generate IR for binary operations
// std::string Parser::generateIRBinaryOperation(const std::string& left, const std::string& right, TOKEN_TYPE op) {
//     std::string result = "t" + std::to_string(ir.size()); // Temporary variable name
//     ir.push_back({result, getTokenString(op), left, right}); // Generate IR for binary operation
//     return result;
// }

// // Helper function to perform copy propagation
// std::string Parser::propagateCopy(const std::string& result) {
//     for (auto& entry : symbolTable) {
//         std::string& value = entry.second;
//         if (value == result) {
//             return entry.first; // Replace use with value
//         }
//     }
//     return result;
// }

// // Helper function to get string representation of token
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