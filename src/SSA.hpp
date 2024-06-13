#ifndef SSA_DS_HPP
#define SSA_DS_HPP

#include <vector>
#include "SymbolTable.hpp"

static int curr_instr_num = 1; // for debugging)

// format: [#: debugging] [operation] [operand(s)]
class SSA {
public:
    // assume user passes in valid args to the curr [operator]
    // based on the op, find the proper operands(?) or wut do we js generate it?
    SSA(int op, std::vector<int> opnds) { 
        std::vector<int> instr;
        instr.push_back(curr_instr_num++);
        instr.push_back(op);
        for (const int &operand : opnds) { // [1 || 2]
            instr.push_back(operand);
        }
    }


    int get_instr_num() { return instr.at(0); }
    int get_operator() { return instr.at(1); }
    std::vector<int> get_operands() { 
        std::vector<int> operands = {};
        for (int i = 2; i < instr.size(); i++) {
            operands.push_back(instr.at(i));
        }
        return operands;
     }
    std::vector<int> get_instr() { return this->instr; }
private:
    std::vector<int> instr;
};

#endif