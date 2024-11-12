// [10.22.2024]: is there even a need for the constList anymore?
// - just combine it all into the [newInstrs] and have the [prev] ptr's

#include "BasicBlock.hpp"

#define DEBUG

int BasicBlock::debugNum = 0;
std::unordered_map<int, SSA *> BasicBlock::ssa_table = {};
std::unordered_map<SSA *, int> BasicBlock::ssa_table_reversed = {};


// [10.22.2024]: Revised?
BasicBlock::BasicBlock(bool isConst, bool isJoin)
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
    this->join = isJoin;

    // #ifdef DEBUG
    //     std::cout << "new BasicBlock; got [instrList; size=" << this->instrList.size() << ", " << instrLst.size() << "] looks like:" << std::endl;
    //     this->printInstrList();
    // #endif
    #ifdef DEBUG
        std::cout << "new BasicBlock looks like: " << this->toString() << "varVals: {}" << std::endl;
    #endif
}

BasicBlock::BasicBlock(std::unordered_map<int, int> DOM_vv_map, bool isConst, bool isJoin)
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
    this->join = isJoin;

    #ifdef DEBUG
        std::cout << "new BasicBlock; got [varVals: size=" << this->varVals.size() << "] looks like:" << std::endl;
        this->printVVs();
    #endif
    // #ifdef DEBUG
    //     std::cout << "new BasicBlock; got [instrList; size=" << this->instrList.size() << ", " << instrLst.size() << "] looks like:" << std::endl;
    //     this->printInstrList();
    // #endif
}
BasicBlock::BasicBlock(BasicBlock *p1, BasicBlock *p2, std::unordered_map<int, int> DOM_vv_map, bool isJoin)
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
    this->join = isJoin;

    #ifdef DEBUG
        std::cout << "new BasicBlock; got [varVals: size=" << this->varVals.size() << "] looks like:" << std::endl;
        this->printVVs();
    #endif
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
        res.pop_back();
    } else {
        for (const auto &node : this->newInstrs) {
            if (node->instr->get_constVal() == nullptr) {
                res += node->instr->toDOT() + "|";
            }
        }
        res.pop_back();
    }
    res += "}";


    return res;
}

// update the [newInstrs] in this BB
// this function is called after we update this BB's varVal mapping with a new update
// - todo: search through each newInstr ( each {x, y} in the SSA )
void BasicBlock::updateInstructions(SSA *oldVal, SSA *newVal) {
    #ifdef DEBUG
        std::cout << "BB before updateInstructions: " << std::endl << this->toString() << std::endl;
    #endif
    for (size_t i = 0; i < this->newInstrs.size(); i++) {
        SSA *curr = this->newInstrs.at(i)->instr;
        curr->updateIfHas(oldVal, newVal);
    }
    #ifdef DEBUG
        std::cout << "BB after updateInstructions: " << std::endl << this->toString() << std::endl;
    #endif
}

SSA *BasicBlock::getConstSSA(int val) {
    SSA *res = nullptr;
    
    if (this->constList) {
        Node *curr = this->constList->tail;

        while (curr) {
            if (*(curr->instr->get_constVal()) == val) {
                res = curr->instr;
                break;
            }
            curr = curr->prev;
        }
    }

    return res;
}