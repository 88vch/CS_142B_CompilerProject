#ifndef SSA_DS_HPP
#define SSA_DS_HPP

#include <vector>
#include "SymbolTable.hpp"

class SSA {
public:
    SSA() { this->curr_instr_num = 1;  }

    // assume user passes in valid args to the curr [operator]
    static std::vector<int> generate_ssa_instr(int op, std::vector<int> opnds) {
        // the instr to be returned
        // size depends on the operator
        std::vector<int> instr;
        instr.push_back(curr_instr_num++);
        instr.push_back(op);
        for (const int &operand : opnds) {
            instr.push_back(operand);
        }
        return instr;
        // based on the op, find the proper operands(?) or wut do we js generate it?
    }


    static int curr_instr_num; // for debugging)
private:
    // std::vector<int> operands;
    // int op;
};

#endif