#ifndef BASICBLOCK_HPP
#define BASICBLOCK_HPP

#include "SymbolTable.hpp"
#include <variant>
#include "SSA.hpp"
#include "LinkedList.hpp"
#include "Result.hpp"

// Remark: the [SubTree]'s should become our [BasicBlock]'s (i.e. we shouldn't need BasicBlock.hpp)


class BasicBlock {
public:
    // [09/03/2024]: How do we determine whether we have the special [const BB0] or not?
    BasicBlock(bool isConst = false) // [09/02/2024]: Note - took away ssa_instructions[curr_instr_list]
    {
        this->parent = nullptr;
        this->parent2 = nullptr;
        this->child = nullptr;
        this->child2 = nullptr;

        if (isConst) {
            this->constList = new LinkedList();
        } else {
            this->instrList = {};
            this->constList = nullptr;
        }
    }

    // [09/27/2024]: Copied from above, probs use this cause we need to inherit the map (&& SSA DLL)
    // [09/02/2024]: Note - took away ssa_instructions[curr_instr_list]
    BasicBlock(std::unordered_map<int, int> DOM_vv_map, std::unordered_map<int, LinkedList*> instrLst, bool isConst = false)
    {
        this->parent = nullptr;
        this->parent2 = nullptr;
        this->child = nullptr;
        this->child2 = nullptr;
        this->updated_varval_map = DOM_vv_map;

        if (isConst) {
            this->constList = new LinkedList();
        } else {
            this->instrList = instrLst;
            this->constList = nullptr;
        }
    }
    // special block: [join]; need phi function here
    BasicBlock(BasicBlock *p1, BasicBlock *p2, std::unordered_map<int, int> DOM_vv_map)
    {
        this->parent = p1;
        this->parent2 = p2;
        this->updated_varval_map = DOM_vv_map;
        this->instrList = {};
        this->child = nullptr;
        this->child2 = nullptr;
    }

    ~BasicBlock() {
        #ifdef DEBUG
            std::cout << "in ~BasicBlock(this->instrList.size=" << this->instrList.size() << ")" << std::endl;
        #endif

        if (this->constList) {
            delete this->constList;
            this->constList = nullptr;
        }

        for (unsigned int i = 1; i < this->instrList.size() - 1; i++) {
            if (this->instrList.find(i) != this->instrList.end()) {
                delete this->instrList.at(i);
            }
            this->instrList.insert(std::pair<int, LinkedList*>(i, nullptr));
        }

        // for (SSA* instr : this->instrs) {
        //     delete instr;
        // }
    }

    void setInstructionList(const std::unordered_map<int, LinkedList*> &curr_instr_lst) {
        // [09/20/2024]: We have to make deep copies lol
        // [09/02/2024]: VALIDATE that this makes a shallow copy (we only care abt the values, since we're going to continue to modify this [curr_instr_list])
        this->instrList = curr_instr_lst;
    }

    std::string instrListToString() const {
        std::string lst = "[instrA]:\n\tinstrA1, instrA2, instrA3, ...\n";
        
        if (this->constList) {
            lst += "[const]: \n\t" + this->constList->listToString();
        } else {
            for (unsigned int i = 1; i < SymbolTable::operator_table.size() - 1; i++) {
                lst += "[" + SymbolTable::operator_table_reversed.at(i) + "]: \n\t" + this->instrList.at(i)->listToString();
            }
        }
        return lst;
    }

    std::string toString() const {
        std::string res = "";

        if (this->constList) {
            res += "[const]: \n";
            res += this->constList->listToString();
        } else {
            res += this->instrListToString();
        }

        return res;
    }

    BasicBlock *parent, *parent2;
    BasicBlock *child, *child2;
    std::unordered_map<int, int> updated_varval_map; // [variable (sym_table val) : value]
    // an ssa instruction: std::vector<int>
    // [#: debugging] [operation] [operand(s)]
    // std::vector<SSA*> instrs;
    LinkedList *constList;
    std::unordered_map<int, LinkedList*> instrList; 
};

#endif   