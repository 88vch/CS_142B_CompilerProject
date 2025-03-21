#include "SSA.hpp"

// format: [#instruction_no: debugging] [operation::operator_table] [operand(s)]
int SSA::curr_instr_num = 1;
int SSA::curr_const_num = -1;

SSA::~SSA() {
    // #ifdef DEBUG
    //     std::cout << "SSA to-delete: " << this->toString() << std::endl;
    // #endif

    // [12.02.2024]: not decrementing number since that would imply we shift the debugNum of every SSA after this one to reflect the update properly for future SSA instructions
    // [12.11.2024]: decrementing, we'll handle BB0 update in Parser
    if (this->op == 0) {
        curr_const_num++;
    } else {
        curr_instr_num--;
    }
    
    if (this->constVal) { delete this->constVal; }
    // [09/17/2024]: Maybe we don't need to do this and just delete individually
    // [Old Version]: delete sub-ssa's if they're not [const]
    // if (this->x && (this->x->get_constVal() == nullptr)) { delete this->x; }
    // if (this->y && (this->y->get_constVal() == nullptr)) { delete this->y; }
}

SSA::SSA(int op, int blkNum) {
    #ifdef DEBUG
        std::cout << "in SSA(int op) constructor" << std::endl;
    #endif

    if (op == 8 || op == 23 || op == 25) { // [read]
        this->debug_num = curr_instr_num++;
        this->op = op;
        
        this->x = nullptr;
        this->y = nullptr;
        this->constVal = nullptr;
    } else {
        std::cout << "Error: expected SSA() operation to be [8: bra, 23: read, or 25: writeNL] got: [" << op << "]! exiting prematurely..." << std::endl;
        exit(EXIT_FAILURE);
    }

    this->blockNum = blkNum;
    this->xIdent = -1;
    this->yIdent = -1;
    this->ident = -1;

    #ifdef DEBUG
        std::cout << "created SSA instruction: " << this->toString() << std::endl;
    #endif
}

SSA::SSA(int op, int opnd, int blkNum) {
    #ifdef DEBUG
        std::cout << "in SSA(int op=" << op << ", int opnd=" << opnd << ") constructor" << std::endl;
    #endif


    if (op == 0) {
        this->debug_num = curr_const_num--;
        this->op = op;
        this->constVal = new int(opnd); // the [SymbolTable::symbol_table] value representing the constVal being stored
    
        this->x = nullptr;
        this->y = nullptr;
    } else {
        std::cout << "Error: expected SSA() operation to be [0: const] got: [" << op << "]! exiting prematurely..." << std::endl;
        exit(EXIT_FAILURE);
    }

    this->blockNum = blkNum;
    this->xIdent = -1;
    this->yIdent = -1;
    this->ident = -1;

    #ifdef DEBUG
        std::cout << "created SSA instruction: " << this->toString() << std::endl;
    #endif
}

SSA::SSA(int op, SSA *retVal, int blkNum) {
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
    } else if (op == 8 || op == 16 || op == 24) {
        this->debug_num = curr_instr_num++;
        this->op = op;
        this->x = retVal;

        this->y = nullptr;
        this->constVal = nullptr;
    } else {
        std::cout << "Error: expected SSA() operation to be [16] got: [" << op << "]! exiting prematurely..." << std::endl;
        exit(EXIT_FAILURE);
    }

    this->blockNum = blkNum;
    this->xIdent = -1;
    this->yIdent = -1;
    this->ident = -1;
}

SSA::SSA(int op, SSA *opnd1, SSA *opnd2, int blkNum) {
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

    this->blockNum = blkNum;
    this->xIdent = -1;
    this->yIdent = -1;
    this->ident = -1;
};
