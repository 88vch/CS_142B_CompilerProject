#ifndef SSA_DS_HPP
#define SSA_DS_HPP

#include <vector>
#include "SymbolTable.hpp"

// format: [#instruction_no: debugging] [operation::operator_table] [operand(s)]
class SSA {
public:
    // [Old Version]
    // SSA();
    // SSA(int op);
    // SSA(int op, int opnd);
    // SSA(int op, int opnd1, int opnd2);


    SSA(int op, int opnd); // [07/26/2024]: const
    // [07/28/2024]: (New) Pointer to result returned from [return: 16] statement
    SSA(int op, SSA *retVal);
    // [07/25/2024]: (New) Pointers to other SSA instructions
    SSA(int op, SSA *opnd1, SSA *opnd2);


    int get_instrNum() { 
        // return instr.at(0); 
        return this->debug_num;
    }
    int get_operator() { 
        // return instr.at(1); 
        return this->op;
    }

    // [Old Version]
    // std::vector<int> get_operands() { 
    //     std::vector<int> operands = {};
    //     for (size_t i = 2; i < instr.size(); i++) {
    //         operands.push_back(instr.at(i));
    //     }
    //     return operands;
    // }
    SSA* get_operand1() {
        return (this->x) ? this->x : nullptr;
    }
    SSA* get_operand2() {
        return (this->y) ? this->y : nullptr;
    }

    // [Old Version]
    // gets and returns the entire instruction [std::vector<int>]
    // std::vector<int> get_instr() { return this->instr; }

    bool compare(int op, SSA *x, SSA *y) {
        if ((this->debug_num > 0) && (this->op == op) && 
            (this->x != nullptr && (this->x == x)) && 
            (this->y != nullptr && (this->y == y))) {
            return true;
        }
        return false;
    }

    bool compareConst(int op, int opnd) {
        if ((this->debug_num < 0) && 
            (this->op == 0) && (this->op == op) && 
            (this->constVal != nullptr && (*(this->constVal) == opnd))) {
            return true;
        }
        return false;
    }

    std::string opToString() const {
        std::string res;

        // [07/28/2024];
        // Generate [operator_table] sorted by values (store as DS in [SymbolTable]?)
        // Iterate through sorted [operator_table] or if can index the std::pair's, return the one at [this->op]
        
        return res;
    }

    std::string toString() const {
        std::string x_val = (this->x) ? std::to_string(this->x->get_instrNum()) : "null";
        std::string y_val = (this->y) ? std::to_string(this->y->get_instrNum()) : "null";
        
        std::string res = "[" + std::to_string(this->debug_num) + "]: " + "`" + this->opToString() + "` | " + x_val + ", " + y_val;

        // [07/28/2024]; ToDo: add cases for [const] and finish this function (along with opToString())
    }
private:
    // [Old Version]
    std::vector<int> instr;
    
    // [07/25/2024]: (New) Pointers to other SSA instructions
    int debug_num, op, *constVal;
    SSA *x, *y;


    static int curr_instr_num; // for debugging
    static int curr_const_num; // for debugging
};

int SSA::curr_instr_num = 1;
int SSA::curr_const_num = -1;

// // [Old Version]: placeholder for default-constructor
// SSA::SSA() {};

// // [Old Version]: [7] end (?)
// SSA::SSA(int op) {
//     if (op == 7) {
//         // ToDo: create [end] instr
//         this->instr = {};
//         this->instr.push_back(curr_instr_num++);
//         this->instr.push_back(op);
//     } else {
//         std::cout << "Error: expected SSA() operation to be 7 (i.e. end) got: [" << op << "]! exiting prematurely..." << std::endl;
//         exit(EXIT_FAILURE);
//     }
// };

// // [Old Version]: [0] const
// SSA::SSA(int op, int opnd) {
//     if (op == 0) {
//         this->instr = {};
//         this->instr.push_back(curr_const_num--);
//         this->instr.push_back(op);
//         this->instr.push_back(opnd);
//     } else {
//         std::cout << "Error: expected SSA() operation to be 0 (i.e. const) got: [" << op << "]! exiting prematurely..." << std::endl;
//         exit(EXIT_FAILURE);
//     }
// };
    
// // [Old Version]: assume user passes in valid args to the curr [operator]
// // based on the op, find the proper operands(?) or wut do we js generate it?
// SSA::SSA(int op, int opnd1, int opnd2) { 
//     if (op > 0 && op <= 25) {
//         this->instr = {};
//         this->instr.push_back(curr_instr_num++);
//         this->instr.push_back(op);
//         this->instr.push_back(opnd1);
//         this->instr.push_back(opnd2);
//     } else {
//         std::cout << "Error: expected SSA() operation to be [1, 25] got: [" << op << "]! exiting prematurely..." << std::endl;
//         exit(EXIT_FAILURE);
//     }
// };

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

#endif