#ifndef SSA_DS_HPP
#define SSA_DS_HPP

#include <vector>
#include "SymbolTable.hpp"

class SSA_DS {
public:
    SSA_DS() { this->curr_instr_num = 1;  }

    static generate_ssa_instr(int op, std::vector<int> operands) {
        // the instr to be returned
        // size depends on the operator
        std::vector<int> instr;

        // based on the op, find the proper operands(?) or wut do we js generate it?
    }


private:
    int curr_instr_num; // for debugging)
    std::vector<int> operands;
    int op;
};

#endif