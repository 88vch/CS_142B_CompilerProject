#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

#undef TOKENS
#define RESULT
#define MAX_FREE_REGS 100

enum r_type {
    CONSTANT,
    VARIABLE,
    REGISTER,
    CONDITION
};

struct Result {
    r_type kind;
    union Data {
        int value; // for const
        int address; // for var
        int regno; // for reg
    } data;
};

// [1]=allocated, [0]=free
// bool free_reg[MAX_FREE_REGS];
// Result regs[MAX_FREE_REGS]; // temp, not sure if needed; "actual" reg's to store data?
// int last_checked_idx = 0;

// int AllocateReg() {
//     do {
//         if (last_checked_idx > (MAX_FREE_REGS - 1)) { last_checked_idx = 0; } 
//         else { last_checked_idx++; }
//     } while (free_reg[last_checked_idx]);
    
//     // assumes we exit while() when we encounter a reg that's free [0]
//     free_reg[last_checked_idx] = 1;
//     return last_checked_idx++; // and increment it after so we don't check this next time we allocate!
// }

// void DeAllocateReg(size_t idx) {
//     free_reg[idx] = 0;
// }

// // no matter what [x] was, we want to force the result into a register
// void load(Result *x) {
//     // if [x] is already a register we don't do anything
//     if ((*x).kind == r_type::REGISTER) { return; } 
//     else if ((*x).kind == r_type::VARIABLE) {
//         (*x).data.regno = AllocateReg();
//         // load it (same way we did w basic compiler onto stack)???
//         // PutF1(LDW, target=x.regno, reg=BASE, x.address);
//         (*x).kind = r_type::REGISTER;
//     } else if ((*x).kind == r_type::CONSTANT) {
//         (*x).data.regno = AllocateReg();
//         // transport the constant into the register
//         // special case: if constant == 0 can js use register 0
//         // PutF1(ADDI, x.regno, reg=0, x.value); 
//         (*x).kind = r_type::REGISTER;
//     }
// }

// Result Compute(op, Result *x, Result *y) {
//     // make modifications on [Result x] and return this modified [Result]
//     if (((*x).kind == r_type::CONSTANT) && (y.kind == r_type::CONSTANT)) { // [a]
//         if (op == ADD) { (*x).value += y.value; } 
//         else if (op == SUB) { (*x).value -= y.value; }
//         else if (op == MULT) { (*x).value *= y.value; }
//         else if (op == DIV) { (*x).value /= y.value; }
//     } else {
//         // only going to combine a const if it's on the right [y]-side
//         load(x); // force [x] into a register
//         // what can be the right side of things? can be const
//         if (y.kind == r_type::CONSTANT) { // [c1]
//             // want to do op with IMM
//             // PutF1(opIMM(op), x.regno, x.regno, y.value);
//         } else { // [b]
//             load(y); // move [y] into a register if it's not already in one
//             PutF1(op, x.regno, x.regno, y.value);
//             // no longer need register [y]; deallocate it
//             DeAllocateReg(y);
//         }

//         // TODO: finish this!
//     }

//     return x;
// }


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
    REL_OP,     // [relational operator (for comparison in statements)(if, while, etc...)]
    OPEN_PAREN, // [for expr's]
    CLOSE_PAREN,// [for expr's]
    OPEN_CURLY, // [for statSequence & functions]
    CLOSE_CURLY,// [for statSequence & functions]
    COMMA,      // [for varDecl / formalParam / funcCall]
    IF,         // [IF-statement]
    THEN,       // [IF-statement clause 2]
    ELSE,       // [IF-statement branch 2]
    FI,         // [end IF-statement]
    WHILE,      // [WHILE-statement]
    DO,         // [WHILE-statement body start]
    OD,         // [WHILE-statement body end]
    FUNCTION,   // [FUNCTION-statement] (might include VOID later)
    CALL,       // [FUNCTION-CALL]
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