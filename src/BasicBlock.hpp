#ifndef BASICBLOCK_HPP
#define BASICBLOCK_HPP

#include "SymbolTable.hpp"
// #include "AST_Node.hpp"
#include <variant>
#include "SSA.hpp"
#include "LinkedList.hpp"

// Remark: the [SubTree]'s should become our [BasicBlock]'s (i.e. we shouldn't need BasicBlock.hpp)


class BasicBlock {
public:
    BasicBlock(BasicBlock *p, std::unordered_map<int, LinkedList> curr_instr_lst) //: ssa_instructions()
    {
        if (p != nullptr) { 
            this->parent = p;
            this->updated_varval_map = this->parent->get_curr_vv_map();
        } else { // assume BB0!
            this->parent = nullptr;
            this->updated_varval_map = {};
        }
        this->instruction_list = curr_instr_lst;
        this->children = {};
    }

    // std::unordered_map<int, LinkedList> get_instr_list() const noexcept { return instruction_list; }
    // void set_instr_list(std::unordered_map<int, LinkedList> lst) { instruction_list = lst; }
    std::unordered_map<int, int> get_curr_vv_map() const noexcept { return this->updated_varval_map; }
    void add_child_blk(BasicBlock *child) { this->children.push_back(child); }
private:
    BasicBlock *parent;
    std::vector<BasicBlock *> children;
    std::unordered_map<int, int> updated_varval_map; // [variable (sym_table val) : value]
    // an ssa instruction: std::vector<int>
    // [#: debugging] [operation] [operand(s)]
    std::vector<SSA> ssa_instructions; 
    std::unordered_map<int, LinkedList> instruction_list; 
};

class BB0 : BasicBlock {
public:
    BasicBlock(std::unordered_map<int, LinkedList> curr_instr_lst) //: ssa_instructions()
    {
        // assume BB0! [no parent BasicBlock]
        this->parent = nullptr;
        this->updated_varval_map = {};
        this->instruction_list = curr_instr_lst;
        this->children = {};
    }
private:
    // TODO: copy from [BasicBlock]
};

class BB_Join : BasicBlock {
public:
    // special block: [join]
    BasicBlock(BasicBlock *p1, BasicBlock *p2, std::unordered_map<int, int> DOM_vv_map, std::unordered_map<int, LinkedList> curr_instr_lst)
    {
        this->parent = nullptr;
        this->updated_varval_map = DOM_vv_map;
        this->instruction_list = curr_instr_lst;
        this->children = {};
    }
private:
    // TODO: copy from [BasicBlock]
};

#endif