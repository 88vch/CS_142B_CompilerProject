#ifndef BASICBLOCK_HPP
#define BASICBLOCK_HPP

#include "SymbolTable.hpp"
#include "AST_Node.hpp"
#include <variant>
#include "SSA_DS.hpp"

// Remark: the [SubTree]'s should become our [BasicBlock]'s (i.e. we shouldn't need BasicBlock.hpp)

// using NodeVariant = std::variant<std::shared_ptr<main_Node>,
//                                  std::shared_ptr<funcBody_Node>,
//                                  std::shared_ptr<formalParam_Node>,
//                                  std::shared_ptr<funcDecl_Node>,
//                                  std::shared_ptr<varDecl_Node>,
//                                  std::shared_ptr<statSequence_Node>,
//                                  std::shared_ptr<statement_Node>,
//                                  std::shared_ptr<return_Node>,
//                                  std::shared_ptr<while_Node>,
//                                  std::shared_ptr<if_Node>,
//                                  std::shared_ptr<funcCall_Node>,
//                                  std::shared_ptr<assignment_Node>,
//                                  std::shared_ptr<relation_Node>,
//                                  std::shared_ptr<expression_Node>,
//                                  std::shared_ptr<term_Node>,
//                                  std::shared_ptr<factor_Node>,
//                                  std::shared_ptr<identNum_Node>>;

class BasicBlock {
public:
    BasicBlock(BasicBlock *parent) : ssa_instructions()
    {
        if (parent != nullptr) { // assume BB0!
            this->updated_sym_table = parent->get_sym_table();
            this->children = {};
        } else {
            this->updated_sym_table = {};
            this->children = {};
        }
    }

    // Custom destructor to properly manage shared pointers
    ~BasicBlock() {
        // Resetting the shared pointer will release the associated resource (if any)
        std::visit([](auto& ptr) { ptr.reset(); }, root);
    }


    std::unordered_map<std::string, int> get_sym_table() const noexcept { return this->updated_sym_table; }
    void add_child_blk(BasicBlock *child) { this->children.push_back(child); }
    // BasicBlock MakeTree(NodeVariant base);
    // BasicBlock CombineTree(BasicBlock x, BasicBlock y, int op);

    // NodeVariant root;

private:
    std::vector<BasicBlock *> children;
    // an ssa instruction: std::vector<int>
    // [#: debugging] [operation] [operand(s)]
    std::vector<std::vector<int>> ssa_instructions; 
    std::unordered_map<std::string, int> updated_sym_table; // symbol table int
};

#endif