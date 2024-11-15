#ifndef BASICBLOCK_HPP
#define BASICBLOCK_HPP

#include "SymbolTable.hpp"
#include "LinkedList.hpp"
#include "Result.hpp"
#include <algorithm>

class BasicBlock {
public:
    // // [09/03/2024]: How do we determine whether we have the special [const BB0] or not?
    // BasicBlock(std::unordered_map<int, LinkedList*> instrLst, bool isConst = false);
    // // [09/27/2024]: Copied from above, probs use this cause we need to inherit the map (&& SSA DLL)
    // // [09/02/2024]: Note - took away ssa_instructions[curr_instr_list]
    // BasicBlock(std::unordered_map<int, int> DOM_vv_map, std::unordered_map<int, LinkedList*> instrLst, bool isConst = false);
    // // special block: [join]; need phi function here
    // BasicBlock(BasicBlock *p1, BasicBlock *p2, std::unordered_map<int, int> DOM_vv_map, std::unordered_map<int, LinkedList*> instrLst);


    // [10.22.2024]: Revised slightly to try without [instrLis]?
    BasicBlock(bool isConst = false, bool isJoin = false);
    BasicBlock(std::unordered_map<int, int> DOM_vv_map, bool isConst = false, bool isJoin = false);
    BasicBlock(BasicBlock *p1, BasicBlock *p2, std::unordered_map<int, int> DOM_vv_map, bool isJoin = false);

    ~BasicBlock() {
        #ifdef DEBUG
            std::cout << "in ~BasicBlock()" << std::endl;
        #endif

        if (this->constList) {
            delete this->constList;
            this->constList = nullptr;
        }

        if (this->constPtr) {
            delete this->constPtr;
            this->constPtr = nullptr;
        }
    }

    std::string printNewInstrs() const {
        std::string res = "\nnewInstrs: ";
        if (!this->newInstrs.empty()) {
            for (const auto &node : this->newInstrs) {
                res += "\n\t" + node->instr->toString() + ", ";
            }
        } else {
            res += "none!";
        }
        return res;
    }

    std::string toString() const {
        std::string res = " ------------------------------------------------------------------------------------------------------------ \n| BB" + std::to_string(this->blockNum) + ": ";

        if (this->constList) 
        {
            res += "[const]: \n| ";
            res += this->constList->listToString();
        } 
        // else 
        // {
        //     res += this->instrListToString();
        // }
        
        res += "|\n| newInstrs: ";
        if (!this->newInstrs.empty()) {
            for (const auto &node : this->newInstrs) {
                res += "|\n| \t" + node->instr->toString() + ", ";
            }
        } else {
            res += "none!";
        }

        res += "|\n| varVals[" + std::to_string(this->varVals.size()) + "]: ((string) SymbolTable::symbol_table.at(key), (SSA *) BasicBlock::ssa_table.at(value)): ";
        if (!this->varVals.empty()) {
            for (const auto &pair : this->varVals) {
                res += "|\n| \tident: " + SymbolTable::symbol_table.at(pair.first) + ", value: " + BasicBlock::ssa_table.at(pair.second)->toString();
            }
        } else {
            res += "none!";
        }

        if (this->child) {
            res +="|\n| child1: " + std::to_string(this->child->debugNum) + "";
        } else {
            res +="|\n| child1: nullptr";
        }
         
        if (this->child2) {
            res +="|\n| child2: " + std::to_string(this->child2->debugNum) + "\n";
        } else {
            res +="|\n| child2: nullptr\n";
        }
        res += " ------------------------------------------------------------------------------------------------------------ ";
        return res;
    }

    inline bool compare(BasicBlock *b) const {
        #ifdef DEBUG
            std::cout << "in BB compare: this=[" << this->blockNum << "], b=[" << b->blockNum << "]" << std::endl;
        #endif
        return (this->blockNum == b->blockNum); 
    }

    // checks for SSA existence in [this->newInstrs]
    inline bool findSSA(SSA *s) {
        for (const auto &n : this->newInstrs) {
            if (s->compare(n->instr)) {
                return true;
            }
        }
        return false;
    }

    inline void printVVs() { // print varVals
        std::unordered_map<int, int> res = this->varVals;

        std::cout << "printing [varVals(size=" << res.size() << ")]:" << std::endl;
        for (const auto &p : res) {
            std::cout << "[" << SymbolTable::symbol_table.at(p.first) << "], [" << BasicBlock::ssa_table.at(p.second)->toString() << "]" << std::endl;
        }
        std::cout << std::endl;
    }

    std::string toDOT() const;
    void updateInstructions(SSA *oldVal, SSA *newVal);
    SSA *getConstSSA(int constVal);
    void removeSSA(SSA *toRemove, std::vector<SSA*> &debugSSA_instrs);

    BasicBlock *parent, *parent2;
    BasicBlock *child, *child2;
    std::unordered_map<int, int> varVals; // [variable (sym_table val) : value]
    // an ssa instruction: std::vector<int>
    // [#: debugging] [operation] [operand(s)]
    std::vector<Node*> newInstrs;
    LinkedList *constList;
    // std::unordered_map<int, LinkedList*> instrList; // [10.22.2024]: May not need this 

    Node *constPtr;

    bool join;
    int blockNum;
    static int debugNum;

    // [11.04.2024]: made public && moved to BB (from pParser)
    // [09/30/2024]: Though this may be for a legitimate reason
    static std::unordered_map<int, SSA *> ssa_table; // key=[value's in this->VVs] : value=SSA-corresponding to int-val
    static std::unordered_map<SSA *, int> ssa_table_reversed;

    struct SafeCustomEqual {
        bool operator()(const int& lhs, const int& rhs) const {
            return lhs == rhs; // Only compare the keys.
        }
    };

    struct SafeCustomHash {
        std::size_t operator()(const int& key) const {
            return std::hash<int>()(key); // Hash the key only.
        }
    };

    static std::unordered_map<int, LinkedList*, SafeCustomHash, SafeCustomEqual> instrList; // 11.14.2024: moved from [Parser]
};

#endif   