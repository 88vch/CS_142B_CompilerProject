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
    BasicBlock() // [09/02/2024]: Note - took away ssa_instructions[curr_instr_list]
    {
        this->parent = nullptr;
        this->parent2 = nullptr;
        this->instruction_list = {};
        this->children = {};
    }
    // special block: [join]; need phi function here
    BasicBlock(BasicBlock *p1, BasicBlock *p2, std::unordered_map<int, int> DOM_vv_map)
    {
        this->parent = p1;
        this->parent2 = p2;
        this->updated_varval_map = DOM_vv_map;
        this->instruction_list = {};
        this->children = {};
    }

    void setInstructionList(const std::unordered_map<int, LinkedList> &curr_instr_lst) {
        // [09/02/2024]: VALIDATE that this makes a shallow copy (we only care abt the values, since we're going to continue to modify this [curr_instr_list])
        this->instruction_list = curr_instr_lst;
    }

    BasicBlock *parent, *parent2;
    std::vector<BasicBlock *> children;
    std::unordered_map<int, int> updated_varval_map; // [variable (sym_table val) : value]
    // an ssa instruction: std::vector<int>
    // [#: debugging] [operation] [operand(s)]
    std::vector<SSA> ssa_instructions; // not really used
    std::unordered_map<int, LinkedList> instruction_list; 
};

#endif