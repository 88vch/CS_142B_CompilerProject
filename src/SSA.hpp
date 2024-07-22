#ifndef SSA_DS_HPP
#define SSA_DS_HPP

#include <vector>
#include "SymbolTable.hpp"

// format: [#instruction_no: debugging] [operation] [operand(s)]
class SSA {
public:
    SSA(int op);
    SSA(int op, int opnd);
    SSA(int op, std::vector<int> opnds);


    int get_instr_num() { return instr.at(0); }
    int get_operator() { return instr.at(1); }
    std::vector<int> get_operands() { 
        std::vector<int> operands = {};
        for (size_t i = 2; i < instr.size(); i++) {
            operands.push_back(instr.at(i));
        }
        return operands;
    }

    // gets and returns the entire instruction [std::vector<int>]
    std::vector<int> get_instr() { return this->instr; }

    static bool compare(SSA a, SSA b) {
        if ((a.get_operator() == b.get_operator()) && 
            (a.get_operands() == b.get_operands())) {
            return true;
        }
        return false;
    }
private:
    std::vector<int> instr;
    static int curr_instr_num; // for debugging
    static int curr_const_num; // for debugging
};

int SSA::curr_instr_num = 1;
int SSA::curr_const_num = -1;

// [7] end (?)
SSA::SSA(int op) {
    if (op == 7) {
        // ToDo: create [end] instr
    } else {
        std::cout << "Error: expected SSA() operation to be 0 (i.e. const) got: [" << op << "]! exiting prematurely..." << std::endl;
        exit(EXIT_FAILURE);
    }
}

// [0] const
SSA::SSA(int op, int opnd) {
    if (op == 0) {
        std::vector<int> instr;
        instr.push_back(curr_const_num--);
        instr.push_back(op);
        instr.push_back(opnd);
    } else {
        std::cout << "Error: expected SSA() operation to be 0 (i.e. const) got: [" << op << "]! exiting prematurely..." << std::endl;
        exit(EXIT_FAILURE);
    }
}
    
// assume user passes in valid args to the curr [operator]
// based on the op, find the proper operands(?) or wut do we js generate it?
SSA::SSA(int op, std::vector<int> opnds) { 
    std::vector<int> instr;
    instr.push_back(curr_instr_num++);
    instr.push_back(op);
    for (const int &operand : opnds) { // [1 || 2]
        instr.push_back(operand);
    }
}

#endif