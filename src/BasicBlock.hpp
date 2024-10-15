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
    BasicBlock(std::unordered_map<int, LinkedList*> instrLst, bool isConst = false);
    // [09/27/2024]: Copied from above, probs use this cause we need to inherit the map (&& SSA DLL)
    // [09/02/2024]: Note - took away ssa_instructions[curr_instr_list]
    BasicBlock(std::unordered_map<int, int> DOM_vv_map, std::unordered_map<int, LinkedList*> instrLst, bool isConst = false);
    // special block: [join]; need phi function here
    BasicBlock(BasicBlock *p1, BasicBlock *p2, std::unordered_map<int, int> DOM_vv_map, std::unordered_map<int, LinkedList*> instrLst);

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

    // [10/01/2024]: We don't actually need to do this bc we can do smtn else instead
    // - [Parser::this->addSSA()] already adds the SSA instr to the LinkedList, 
    // - right before we create a new [BasicBlock] each time, we should ONLY THEN assign the [Parser::SSA_LL] to [this->instrList]!!!
    void insertInstr(SSA *instr) {
        this->instrList.at(instr->get_operator())->InsertAtTail(instr);
    }

    // [10/14/2024]: BLEHHHH; ugh do we need to do this?
    void setInstructionList(const std::unordered_map<int, LinkedList*> &curr_instr_lst) {
        #ifdef DEBUG
            std::cout << "\tin setInstructionList( lol ) pre-clear" << std::endl;
        #endif
        
        // [09/20/2024]: We have to make deep copies lol
        for (unsigned int i = 1; i < this->instrList.size(); i++) {
            delete this->instrList.at(i);
            // this->instrList.insert(std::pair<int, LinkedList*>(i, nullptr));
        }
        
        #ifdef DEBUG
            std::cout << "\tin setInstructionList( lol ) post-erase" << std::endl;
        #endif

        this->instrList.clear();
        
        for (const auto &pair : curr_instr_lst) {
            this->instrList.insert(pair);
        }
        // [09/02/2024]: VALIDATE that this makes a shallow copy (we only care abt the values, since we're going to continue to modify this [curr_instr_list])
        // this->instrList = curr_instr_lst;
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

    std::string toDOT() const;

    BasicBlock *parent, *parent2;
    BasicBlock *child, *child2;
    std::unordered_map<int, int> updated_varval_map; // [variable (sym_table val) : value]
    // an ssa instruction: std::vector<int>
    // [#: debugging] [operation] [operand(s)]
    std::vector<SSA*> newInstrs;
    LinkedList *constList;
    std::unordered_map<int, LinkedList*> instrList; 

    int blockNum;
    static int debugNum;
};

#endif   