#include "BasicBlock.hpp"

int BasicBlock::debugNum = 1;

// [09/03/2024]: How do we determine whether we have the special [const BB0] or not?
BasicBlock::BasicBlock(std::unordered_map<int, LinkedList*> instrLst, bool isConst) // [09/02/2024]: Note - took away ssa_instructions[curr_instr_list]
{
    this->blockNum = debugNum++;

    this->parent = nullptr;
    this->parent2 = nullptr;
    this->child = nullptr;
    this->child2 = nullptr;

    if (isConst) {
        this->constList = new LinkedList();
    } else {
        this->instrList = instrList;
        this->constList = nullptr;
    }
    this->newInstrs = {};
}

// [09/27/2024]: Copied from above, probs use this cause we need to inherit the map (&& SSA DLL)
// [09/02/2024]: Note - took away ssa_instructions[curr_instr_list]
BasicBlock::BasicBlock(std::unordered_map<int, int> DOM_vv_map, std::unordered_map<int, LinkedList*> instrLst, bool isConst)
{
    this->blockNum = debugNum++;

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
    this->newInstrs = {};
}
// special block: [join]; need phi function here
BasicBlock::BasicBlock(BasicBlock *p1, BasicBlock *p2, std::unordered_map<int, int> DOM_vv_map, std::unordered_map<int, LinkedList*> instrLst)
{
    this->blockNum = debugNum++;

    this->parent = p1;
    this->parent2 = p2;
    this->updated_varval_map = DOM_vv_map;
    this->instrList = instrList;
    this->child = nullptr;
    this->child2 = nullptr;

    this->newInstrs = {};
}

std::string BasicBlock::toDOT() const {
    std::string res = "";
    // std::string res = "<b>";

    res += "BB" + std::to_string(this->blockNum) + " | {";

    for (const auto &instr : this->newInstrs) {
        res += instr->toDOT() + "|";
    }
    res.pop_back();
    res += "}";


    return res;
}