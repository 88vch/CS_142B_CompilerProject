#include "SSA.hpp"

class Node {
public:
    SSA *instr;
    Node *next;
    Node *prev;

    // Constructor
    // Node() : instr(nullptr), next(nullptr), prev(nullptr) {}
    Node() : instr(nullptr), next(nullptr), prev(nullptr) {}
    Node(SSA *in) : instr(in), next(nullptr), prev(nullptr) {}

    ~Node() {
        // [10/19/2024]: If we only have references (from [Parser::instrList]), then we shouldn't delete the SSA instr here, rather in [Parser] 
        // if (this->instr) {
        //     delete this->instr; 
        // }
        this->instr = nullptr;
        this->next = nullptr; 
        this->prev = nullptr; 
    }
};