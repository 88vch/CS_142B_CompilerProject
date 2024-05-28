#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

// Token type enum
enum TOKEN_TYPE {
    IDENTIFIER, // [var/func names]
    NUMBER,     // [DIGIT | {DIGIT}]
    PLUS,       // [+]
    MINUS,      // [-]
    MULTIPLY,   // [*]
    DIVIDE,     // [/]
    SEMICOLON,  // [;]
    PRINT,      // self-defined
    VAR,        // [VAR]
    LET,        // [LET]
    ASSIGNMENT, // [<-]
    EXPRESSION, // [math]
    OPEN_PAREN, // [for expr's]
    CLOSE_PAREN,// [for expr's]
    OPEN_CURLY, // [for statSequence & functions]
    CLOSE_CURLY,// [for statSequence & functions]
    IF,         // [IF-statement]
    THEN,       // [IF-statement clause 2]
    ELSE,       // [IF-statement branch 2]
    FI,         // [end IF-statement]
    WHILE,      // [WHILE-statement]
    DO,         // [WHILE-statement body start]
    OD,         // [WHILE-statement body end]
    FUNCTION,   // [FUNCTION-statement] (might include VOID later)
    RETURN,     // [RETURN-statement]
    MAIN,       // [start of file]
    END_OF_FILE // [end of file indicated by "."]
};

// Token struct
struct TOKEN {
    TOKEN_TYPE type;
    std::string lexeme;
};

std::string TOKEN_TYPE_toString(TOKEN_TYPE token);
const std::string TOKEN_TYPE_toStringLower(TOKEN_TYPE token);

#endif