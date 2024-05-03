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
    LET,        // [LET]
    ASSIGNMENT, // [<-]
    EXPRESSION, // [math]
    OPEN_PAREN, // [for expr's]
    CLOSE_PAREN,// [for expr's]
    MAIN,       // [start of file]
    END_OF_FILE // [end of file]
};

// Token struct
struct TOKEN {
    TOKEN_TYPE type;
    std::string lexeme;
};

#endif