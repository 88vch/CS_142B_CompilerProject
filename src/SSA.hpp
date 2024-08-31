#ifndef SSA_DS_HPP
#define SSA_DS_HPP

#include <vector>
#include "SymbolTable.hpp"

#define DEBUG

// format: [#instruction_no: debugging] [operation::operator_table] [operand(s)]
class SSA {
public:
    bool isWhile;
    
    SSA(int op, int opnd); // [07/26/2024]: const
    SSA(int op, SSA *retVal); // [07/28/2024]: (New) Pointer to result returned from [return: 16] statement
    SSA(int op, SSA *opnd1, SSA *opnd2); // [07/25/2024]: (New) Pointers to other SSA instructions

    ~SSA();
    // Copy constructor
    SSA(const SSA& other) : op(other.op), debug_num(other.debug_num) {
        if (other.constVal) { this->constVal = new int(*(other.get_constVal())); }
        if (other.get_operand1()) { this->x = new SSA(*(other.get_operand1())); }
        if (other.get_operand2()) { this->y = new SSA(*(other.get_operand2())); }
    }

    // Assignment operator
    SSA& operator=(const SSA& other) {
        if (this != &other) {
            if (this->constVal) { delete this->constVal; } // Free existing resource
            if (*(other.get_constVal())) {
                this->constVal = new int(*(other.get_constVal())); // Allocate and copy resource
            }
            delete this->x;
            delete this->y;
            this->x = other.get_operand1();
            this->y = other.get_operand2();
            this->debug_num = other.get_debugNum();
        }
        return *this;
    }


int get_debugNum() const { 
        // return instr.at(0); 
        return this->debug_num;
    }
    int get_operator() const { 
        // return instr.at(1); 
        return this->op;
    }
    int* get_constVal() const {
        if (this->constVal) {
            return this->constVal;
        } else { return nullptr; }
    }
    SSA* get_operand1() const {
        return (this->x) ? this->x : nullptr;
    }
    SSA* get_operand2() const {
        return (this->y) ? this->y : nullptr;
    }

    bool compare(int op, SSA *x, SSA *y) const {
        #ifdef DEBUG
            std::cout << "\t[SSA::compare(op=" << op << ")]; this->op=" << this->op << ", this->x=";
            if (this->x) {
                std::cout << this->x->toString() << ", this->y=";
            } else {
                std::cout << "nullptr, this->y=";
            }
            if (this->y) {
                std::cout << this->y->toString() << std::endl;
            } else {
                std::cout << "nullptr" << std::endl;
            }
        #endif
        if ((this->debug_num < 0) || (this->x == nullptr) || (this->y == nullptr)) { return false; }

        if ((this->op == op) && (this->x == x) && (this->y == y)) {
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

    bool compareConst(int val) const {
        if ((this->debug_num < 0) && (this->op == 0) && 
            (this->constVal != nullptr && (*(this->constVal) == val))) {
            return true;
        }
        return false;
    }
    bool compareConst(SSA *s) const {
        if ((this->debug_num < 0) && 
            (this->op == 0) && (this->op == s->get_operator()) && 
            (
                (this->constVal != nullptr && (*(this->constVal) == *(s->get_constVal()))) || 
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
        std::string x_val = (this->x) ? "[SSA]: " + std::to_string(this->x->get_debugNum()) : "[SSA]: null";
        std::string y_val = (this->y) ? "[SSA]: " + std::to_string(this->y->get_debugNum()) : "[SSA]: null";
        
        // [07/28/2024]; ToDo: add cases for [const] and finish this function (along with opToString())
        std::string res = "[" + std::to_string(this->debug_num) + "]: \t`" + this->opToString() + "`";
        std::string op = this->opToString();
        res += "\t | ";
        if (this->op == 0) {
            if (this->constVal) {
                res += "[CONST]: " + std::to_string(*(this->constVal));
            } else {
                res += "[CONST SSA]: " + x_val;
            }
        } else {
            res += x_val + ", \t\t" + y_val;
        }
        return res;
    }

    void set_operand2(SSA *s) {
        if (this->y == nullptr) {
            this->y = s;
        } else {
            std::cout << "Error: SSA setting [operand2] expected [operand2] to be [nullptr]!, got: [" << this->y->toString() << "]! exiting prematurely..." << std::endl;
            exit(EXIT_FAILURE);
        }
    }
private:
    // [Old Version]
    // std::vector<int> instr;
    
    // [07/25/2024]: (New) Pointers to other SSA instructions
    int op, debug_num, *constVal;
    SSA *x, *y;


    static int curr_instr_num; // for debugging
    static int curr_const_num; // for debugging
};

#endif