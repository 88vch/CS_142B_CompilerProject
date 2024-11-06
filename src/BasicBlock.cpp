// [10.22.2024]: is there even a need for the constList anymore?
// - just combine it all into the [newInstrs] and have the [prev] ptr's

#include "BasicBlock.hpp"

#define DEBUG

int BasicBlock::debugNum = 0;
std::unordered_map<int, SSA *> BasicBlock::ssa_table = {};
std::unordered_map<SSA *, int> BasicBlock::ssa_table_reversed = {};


// [10.22.2024]: Revised?
BasicBlock::BasicBlock(bool isConst)
{
    #ifdef DEBUG
        std::cout << "in BasicBlock(isConst=" << isConst << ")" << std::endl;
    #endif

    this->blockNum = debugNum++;

    this->parent = nullptr;
    this->parent2 = nullptr;
    this->child = nullptr;
    this->child2 = nullptr;

    if (isConst) {
        this->constList = new LinkedList();
        this->constPtr = new Node();
    } else {
        this->constList = nullptr;
        this->constPtr = nullptr;
    }
    this->newInstrs = {};
    this->varVals = {};

    // #ifdef DEBUG
    //     std::cout << "new BasicBlock; got [instrList; size=" << this->instrList.size() << ", " << instrLst.size() << "] looks like:" << std::endl;
    //     this->printInstrList();
    // #endif
    #ifdef DEBUG
        std::cout << "new BasicBlock looks like: " << this->toString() << std::endl;
    #endif
}

BasicBlock::BasicBlock(std::unordered_map<int, int> DOM_vv_map, bool isConst)
{
    this->blockNum = debugNum++;

    this->parent = nullptr;
    this->parent2 = nullptr;
    this->child = nullptr;
    this->child2 = nullptr;
    this->varVals = DOM_vv_map;

    if (isConst) {
        this->constList = new LinkedList();
        this->constPtr = new Node();
    } else {
        this->constList = nullptr;
        this->constPtr = nullptr;
    }
    this->newInstrs = {};

    // #ifdef DEBUG
    //     std::cout << "new BasicBlock; got [instrList; size=" << this->instrList.size() << ", " << instrLst.size() << "] looks like:" << std::endl;
    //     this->printInstrList();
    // #endif
}
BasicBlock::BasicBlock(BasicBlock *p1, BasicBlock *p2, std::unordered_map<int, int> DOM_vv_map)
{
    this->blockNum = debugNum++;

    this->parent = p1;
    this->parent2 = p2;
    this->varVals = DOM_vv_map;
    
    this->child = nullptr;
    this->child2 = nullptr;

    this->constList = nullptr;
    this->constPtr = nullptr;

    this->newInstrs = {};

    // #ifdef DEBUG
    //     std::cout << "new BasicBlock; got [instrList; size=" << this->instrList.size() << ", " << instrLst.size() << "] looks like:" << std::endl;
    //     this->printInstrList();
    // #endif
}

std::string BasicBlock::toDOT() const {
    std::string res = "";
    // std::string res = "<b>";

    res += "BB" + std::to_string(this->blockNum) + " | {";

    if (this->constPtr) {
        Node *curr = this->constList->tail;

        while (curr) {
            res += curr->instr->toDOT() + "|";
            curr = curr->prev;
        }
    } else {
        for (const auto &node : this->newInstrs) {
            if (node->instr->get_constVal() == nullptr) {
                res += node->instr->toDOT() + "|";
            }
        }
    }
    res.pop_back();
    res += "}";


    return res;
}

// update the [newInstrs] in this BB
// this function is called after we update this BB's varVal mapping with a new update
// - todo: search through each newInstr ( each {x, y} in the SSA )
void updateInstructions(SSA *oldVal, SSA *newVal) {

}