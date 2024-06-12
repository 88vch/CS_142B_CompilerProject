#ifndef SUBTREE_HPP
#define SUBTREE_HPP

#include "SymbolTable.hpp"
#include "AST_Node.hpp"
#include <variant>
#include "SSA_DS.hpp"

// Remark: the [SubTree]'s should become our [BasicBlock]'s (i.e. we shouldn't need BasicBlock.hpp)

using NodeVariant = std::variant<std::shared_ptr<main_Node>,
                                 std::shared_ptr<funcBody_Node>,
                                 std::shared_ptr<formalParam_Node>,
                                 std::shared_ptr<funcDecl_Node>,
                                 std::shared_ptr<varDecl_Node>,
                                 std::shared_ptr<statSequence_Node>,
                                 std::shared_ptr<statement_Node>,
                                 std::shared_ptr<return_Node>,
                                 std::shared_ptr<while_Node>,
                                 std::shared_ptr<if_Node>,
                                 std::shared_ptr<funcCall_Node>,
                                 std::shared_ptr<assignment_Node>,
                                 std::shared_ptr<relation_Node>,
                                 std::shared_ptr<expression_Node>,
                                 std::shared_ptr<term_Node>,
                                 std::shared_ptr<factor_Node>,
                                 std::shared_ptr<identNum_Node>>;

class SubTree {
public:
    SubTree(NodeVariant node) : root(node), ssa_instructions()
    {
    }

    // Custom destructor to properly manage shared pointers
    ~SubTree() {
        // Resetting the shared pointer will release the associated resource (if any)
        std::visit([](auto& ptr) { ptr.reset(); }, root);
    }

    // SubTree MakeTree(NodeVariant base);
    SubTree CombineTree(SubTree x, SubTree y, int op);
    NodeVariant root;

private:
    std::vector<SSA_DS> ssa_instructions;
};

#endif