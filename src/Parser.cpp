#include "Parser.hpp"

// Recursive Descent LL(1) Parsing to generate BB IR-representation
// - assume we've generated all SSA Instructions & Have done error checking
BasicBlock Parser::parse() {
    BasicBlock r(this->instrList);
    return r; // Stub
}


// does error checking here
void Parser::generate_SSA() {
    // concerned with first 15 operators first
    // ToDo: this is where we will use Recursive Descent [OldParser.*]
    // - [RESUME HERE]!!!!!!
}

