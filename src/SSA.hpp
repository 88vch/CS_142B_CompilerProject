#ifndef SSA_DS_HPP
#define SSA_DS_HPP

#include <vector>
#include "SymbolTable.hpp"

// format: [#instruction_no: debugging] [operation::operator_table] [operand(s)]
class SSA {
public:
    SSA(int op, int opnd); // [07/26/2024]: const
    SSA(int op, SSA *retVal); // [07/28/2024]: (New) Pointer to result returned from [return: 16] statement
    SSA(int op, SSA *opnd1, SSA *opnd2); // [07/25/2024]: (New) Pointers to other SSA instructions


    int get_instrNum() { 
        // return instr.at(0); 
        return this->debug_num;
    }
    int get_operator() { 
        // return instr.at(1); 
        return this->op;
    }
    int get_constVal() const {
        return *(this->constVal);
    }
    SSA* get_operand1() {
        return (this->x) ? this->x : nullptr;
    }
    SSA* get_operand2() {
        return (this->y) ? this->y : nullptr;
    }

    bool compare(int op, SSA *x, SSA *y) const {
        if ((this->debug_num > 0) && (this->op == op) && 
            (this->x != nullptr && (this->x == x)) && 
            (this->y != nullptr && (this->y == y))) {
            return true;
        }
        return false;
    }
    bool compare(SSA *s) const {
        if ((this->debug_num > 0) && (this->op == s->get_operator()) && 
            (this->x != nullptr && (this->x == s->get_operand1())) && 
            (this->y != nullptr && (this->y == s->get_operand2()))) {
            return true;
        }
        return false;
    }

    bool compareConst(int op, int opnd) const {
        if ((this->debug_num < 0) && 
            (this->op == 0) && (this->op == op) && 
            (this->constVal != nullptr && (*(this->constVal) == opnd))) {
            return true;
        }
        return false;
    }
    bool compareConst(SSA *s) const {
        if ((this->debug_num < 0) && 
            (this->op == 0) && (this->op == s->get_operator()) && 
            (
                (this->constVal != nullptr && (*(this->constVal) == s->get_constVal())) || 
                (this->x != nullptr && (this->x == s->get_operand1()))
            )) {
            return true;
        }
        return false;
    }

    std::string opToString() const {
        return SymbolTable::operator_table_reversed.at(this->op);
    }

    std::string toString() const {
        std::string x_val = (this->x) ? "[SSA]: " + std::to_string(this->x->get_instrNum()) : "[SSA]: null";
        std::string y_val = (this->y) ? "[SSA]: " + std::to_string(this->y->get_instrNum()) : "[SSA]: null";
        
        // [07/28/2024]; ToDo: add cases for [const] and finish this function (along with opToString())
        std::string res = "[" + std::to_string(this->debug_num) + "]: " + "`" + this->opToString() + "` | ";
        if (this->op == 0) {
            if (this->constVal) {
                res += "[CONST]: " + std::to_string(*(this->constVal));
            } else {
                res += "[CONST SSA]: " + x_val;
            }
        } else {
            res += x_val + ", " + y_val;
        }
        return res;
    }

    void set_operand2(SSA *s) {
        if (this->y == nullptr) {
            this->y = s;
        } else {
            std::cout << "Error: SSA setting [operand2] expected [operand2] to be [nullptr]!, got: [" << this->y << "]! exiting prematurely..." << std::endl;
            exit(EXIT_FAILURE);
        }
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

#endif