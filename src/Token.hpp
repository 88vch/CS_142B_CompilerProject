#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

// struct node {
//     TOKEN *data;
//     std::vector<node *>children;
// };

// Token type enum: do we need a [VALUE] type (number, string, idk)
enum TOKEN_TYPE {
    // IDENTIFIER, // [var/func names]
    // NUMBER,     // [DIGIT | {DIGIT}]
    SEMICOLON,  // [;]
    PRINT,      // self-defined
    VAR,        // [KEYWORD: VAR]
    LET,        // [KEYWORD: LET]
    ASSIGNMENT, // [KEYWORD: <-]
    // EXPRESSION, // [math]
    REL_OP_GT,     // [relational operator (for comparison in statements)(if, while, etc...)]
    REL_OP_LT,     // [relational operator (for comparison in statements)(if, while, etc...)]
    REL_OP_EQ,     // [relational operator (for comparison in statements)(if, while, etc...)]
    REL_OP_GEQ,     // [relational operator (for comparison in statements)(if, while, etc...)]
    REL_OP_LEQ,     // [relational operator (for comparison in statements)(if, while, etc...)]
    OPEN_PAREN, // [KEYWORD: for expr's]
    CLOSE_PAREN,// [KEYWORD: for expr's]
    OPEN_CURLY, // [KEYWORD: for statSequence & functions]
    CLOSE_CURLY,// [KEYWORD: for statSequence & functions]
    COMMA,      // [KEYWORD: for varDecl / formalParam / funcCall]
    IF,         // [KEYWORD: IF-statement]
    THEN,       // [KEYWORD: IF-statement clause 2]
    ELSE,       // [KEYWORD: IF-statement branch 2]
    FI,         // [KEYWORD: end IF-statement]
    WHILE,      // [KEYWORD: WHILE-statement]
    DO,         // [KEYWORD: WHILE-statement body start]
    OD,         // [KEYWORD: WHILE-statement body end]
    CALL,       // [KEYWORD: VOID-function-statement] (to call a function)
    FUNCTION,   // [KEYWORD: FUNCTION-statement] (might include VOID later)
    VOID,       // [KEYWORD: VOID-function-statement] (indeed included VOID later :)
    RETURN,     // [KEYWORD: RETURN-statement]
    MAIN,       // [KEYWORD: MAIN]
    END_OF_FILE, // [end of file indicated by "."]
    PLUS=-1,    // [+]
    MINUS=-2,   // [-]
    MULTIPLY=-3,// [*]
    DIVIDE=-4  // [/]
};

// Token struct
struct TOKEN {
    TOKEN_TYPE type;
    std::string lexeme;
};


std::string TOKEN_TYPE_toString(TOKEN_TYPE token);
const std::string TOKEN_TYPE_toStringLower(TOKEN_TYPE token);

#endif