#include "SSA.hpp"

// format: [#instruction_no: debugging] [operation::operator_table] [operand(s)]
int SSA::curr_instr_num = 1;
int SSA::curr_const_num = -1;

SSA::SSA(int op, int opnd) {
    if (op == 0) {
        this->debug_num = curr_const_num--;
        this->op = op;
        *(this->constVal) = opnd; // the [SymbolTable::symbol_table] value representing the constVal being stored
    
        this->x = nullptr;
        this->y = nullptr;
    } else {
        std::cout << "Error: expected SSA() operation to be [0: const] got: [" << op << "]! exiting prematurely..." << std::endl;
        exit(EXIT_FAILURE);
    }
}

SSA::SSA(int op, SSA *retVal) {
    if (op == 0) {
        // if [op == 0 == const], and [this->constVal == nullptr], we should check if [this->x == nullptr]
        // - a constant can either be an integer literal (given by the [SymbolTable::symbol_table] value representing the constVal being stored)
        // - OR, an [SSA instruction] where the result is deduced to be a const (i.e. const + const = const, const + ident = (potentially new) const)
        //      Note: when I say `deduced to be a const`, I mean the SSA instruction is a (or a series of) instruction(s) that trace back to a const number
        this->debug_num = curr_const_num--;
        this->op = op;  
        this->x = retVal;

        this->y = nullptr;
        this->constVal = nullptr;
    } else if (op == 16) {
        this->debug_num = curr_instr_num++;
        this->op = op;
        this->x = retVal;

        this->y = nullptr;
        this->constVal = nullptr;
    } else {
        std::cout << "Error: expected SSA() operation to be [16] got: [" << op << "]! exiting prematurely..." << std::endl;
        exit(EXIT_FAILURE);
    }
}

SSA::SSA(int op, SSA *opnd1, SSA *opnd2) {
    if (op > 0 && op <= 25) {
        this->debug_num = curr_instr_num++;
        this->op = op;
        this->x = opnd1;
        this->y = opnd2;

        this->constVal = nullptr;
    } else {
        std::cout << "Error: expected SSA() operation to be [1, 25] got: [" << op << "]! exiting prematurely..." << std::endl;
        exit(EXIT_FAILURE);
    }
};
