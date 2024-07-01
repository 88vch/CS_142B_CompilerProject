// #ifndef BASICBLOCK_HPP
// #define BASICBLOCK_HPP

// #include "SymbolTable.hpp"
// // #include "AST_Node.hpp"
// #include <variant>
// #include "SSA.hpp"
// #include "LinkedList.hpp"

// // Remark: the [SubTree]'s should become our [BasicBlock]'s (i.e. we shouldn't need BasicBlock.hpp)


// class BasicBlock {
// public:
//     BasicBlock(BasicBlock *p, std::unordered_map<int, LinkedList> curr_instr_lst) //: ssa_instructions()
//     {
//         if (p != nullptr) { 
//             this->parent = p;
//             this->parent2 = nullptr;
//             this->updated_varval_map = this->parent->updated_varval_map;
//         } else { // assume BB0!
//             this->parent = nullptr;
//             this->parent2 = nullptr;
//             this->updated_varval_map = {};
//         }
//         this->instruction_list = curr_instr_lst;
//         this->children = {};
//     }
//     // special block: [join]
//     BasicBlock(BasicBlock *p1, BasicBlock *p2, std::unordered_map<int, int> DOM_vv_map, std::unordered_map<int, LinkedList> curr_instr_lst)
//     {
//         this->parent = p1;
//         this->parent2 = p2;
//         this->updated_varval_map = DOM_vv_map;
//         this->instruction_list = curr_instr_lst;
//         this->children = {};
//     }

//     BasicBlock *parent, *parent2;
//     std::vector<BasicBlock *> children;
//     std::unordered_map<int, int> updated_varval_map; // [variable (sym_table val) : value]
//     // an ssa instruction: std::vector<int>
//     // [#: debugging] [operation] [operand(s)]
//     std::vector<SSA> ssa_instructions; // not really used
//     std::unordered_map<int, LinkedList> instruction_list; 
// };

// #endif