#ifndef SSA_DS_HPP
#define SSA_DS_HPP

#include <vector>
#include "SymbolTable.hpp"

#define DEBUG

// format: [#instruction_no: debugging] [operation::operator_table] [operand(s)]
class SSA {
public:
    
    SSA(int op, int blkNum = -1); // [09/27/2024]: [read]
    SSA(int op, int opnd, int blkNum = -1); // [07/26/2024]: [const]
    SSA(int op, SSA *retVal, int blkNum = -1); // [07/28/2024]: (New) Pointer to result returned from [return: 16] statement
    SSA(int op, SSA *opnd1, SSA *opnd2, int blkNum = -1); // [07/25/2024]: (New) Pointers to other SSA instructions

    ~SSA();
    // Copy constructor
    SSA(const SSA& other) : op(other.op), debug_num((op > 0) ? curr_instr_num++ : curr_const_num--), blockNum(other.blockNum) {
        if (other.constVal) { this->constVal = new int(*(other.get_constVal())); } else { this->constVal = nullptr; }
        if (other.get_operand1()) { this->x = new SSA(*(other.get_operand1())); } else { this->x = nullptr; }
        if (other.get_operand2()) { this->y = new SSA(*(other.get_operand2())); } else { this->y = nullptr; }
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
        return this->constVal;
    }
    SSA* get_operand1() const {
        return (this->x) ? this->x : nullptr;
    }
    SSA* get_operand2() const {
        return (this->y) ? this->y : nullptr;
    }

    void updateIfHas(SSA *oldVal, SSA *newVal) {
        if (this->x == oldVal) {
            #ifdef DEBUG
                std::cout << "updated [SSA->x]" << std::endl;
            #endif
            this->x = newVal;
        }
        if (this->y == oldVal) {
            #ifdef DEBUG
                std::cout << "updated [SSA->y]" << std::endl;
            #endif
            this->y = newVal;
        }
    }

    bool compare(int op, SSA *x, SSA *y) const {
        #ifdef DEBUG
            std::cout << "[SSA::compare(op=" << op << ")]; this->op=" << this->op << ", this->x=";
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

        // return false because we have [compareConst()]
        if (this->debug_num < 0) { return false; }

        if ((this->op == op) && (this->x == x) && (this->y == y)) {
            #ifdef DEBUG
                std::cout << "\treturned true!" << std::endl;
            #endif
            return true;
        }
        #ifdef DEBUG
            std::cout << "\treturned false!" << std::endl;
        #endif
        return false;
    }
    bool compare(SSA *s) const {
        if (!s) { 
            #ifdef DEBUG 
                std::cout << "[SSA::compare(s); found s == nullptr]" << std::endl; 
            #endif 
            return false; 
        }

        if (
            (
                (this->debug_num > 0) && (this->op == s->get_operator()) && 
                (
                    ((this->x != nullptr && (this->x == s->get_operand1())) && 
                    (this->y != nullptr && (this->y == s->get_operand2()))) 
                    || 
                    (((this->x == nullptr) && (s->get_operand1() == nullptr)) && 
                    ((this->y == nullptr) && (s->get_operand2() == nullptr)))
                    || 
                    (((this->x != nullptr) && (this->x == s->get_operand1())) && 
                    ((this->y == nullptr) && (s->get_operand2() == nullptr)))
                    || 
                    (((this->x == nullptr) && (s->get_operand1() == nullptr)) && 
                    ((this->y != nullptr) && (this->y == s->get_operand2())))
                )
            ) 
            ||
            (
                (this->debug_num < 0) && 
                (this->constVal != nullptr && (this->constVal == s->get_constVal()))
            )) {
            return true;
        }
        return false;
    }

    bool compareConst(int val) const {
        #ifdef DEBUG
            std::cout << "this->val=" << *(this->constVal) << ", comparing=" << val << std::endl;
        #endif
        if ((this->debug_num < 0) && (this->op == 0) && 
            (this->constVal != nullptr && (*(this->constVal) == val))) {
            #ifdef DEBUG
                std::cout << "\treturning true!" << std::endl;
            #endif
            return true;
        }
        #ifdef DEBUG
            std::cout << "\treturning false!" << std::endl;
        #endif
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

    // [10/18/2024]: What's wrong here??? Why is it segfaulting？
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

    std::string toDOT() const {
        std::string res = "";

        res += std::to_string(this->debug_num) + ":" + this->opToString();

        if (this->x) {
            res += " (" + std::to_string(this->x->get_debugNum()) + ")";
        }

        if (this->y) {
            res += " (" + std::to_string(this->y->get_debugNum()) + ")";
        }

        if (this->constVal) {
            res += " (" + std::to_string(*(this->constVal)) + ")";
        }

        return res;
    }

    void set_operand1(SSA *s) {
        if (this->x == nullptr) {
            this->x = s;
        } else {
            std::cout << "Warning: SSA setting [operand1] expected [operand1] to be [nullptr]!, got: [" << this->x->toString() << "]! continuing update..." << std::endl;
            // exit(EXIT_FAILURE);
            this->x = s;
        }
    }

    void set_operand2(SSA *s) {
        if (this->y == nullptr) {
            this->y = s;
        } else {
            std::cout << "Warning: SSA setting [operand2] expected [operand2] to be [nullptr]!, got: [" << this->y->toString() << "]! continuing update..." << std::endl;
            // exit(EXIT_FAILURE);
            this->y = s;
        }
    }

    void updateDebugNum(int num = -1) {
        if (this->debug_num > curr_instr_num) {
            this->debug_num = curr_instr_num - 1; // [11.18.2024]: minus 1 since [this] is part of the [curr_instr_num]; we update us to get teh proper value
            
            #ifdef DEBUG
                std::cout << "updated Debug Number for ssa!" << std::endl << this->toString() << std::endl;
            #endif
        } else {
            // assume const
            this->debug_num = num;
        }
    }

    void setBlkNum(int blkNum) {
        this->blockNum = blkNum;
    }

    int getBlkNum() const {
        return this->blockNum;
    }

    static void resetDebug() {
        curr_instr_num = 1;
        curr_const_num = -1;
    }
private:
    // [Old Version]
    // std::vector<int> instr;
    
    // [07/25/2024]: (New) Pointers to other SSA instructions
    int op, debug_num, *constVal;
    SSA *x, *y;
    int blockNum; // [12.16.2024]; the bb blockNum that this SSA belongs to (only used for p2)


    static int curr_instr_num; // for debugging
    static int curr_const_num; // for debugging
};

#endif