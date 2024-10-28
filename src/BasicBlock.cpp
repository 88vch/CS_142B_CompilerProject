// [10.22.2024]: is there even a need for the constList anymore?
// - just combine it all into the [newInstrs] and have the [prev] ptr's

#include "BasicBlock.hpp"

#define DEBUG

int BasicBlock::debugNum = 0;

// // [09/03/2024]: How do we determine whether we have the special [const BB0] or not?
// BasicBlock::BasicBlock(std::unordered_map<int, LinkedList*> instrLst, bool isConst) // [09/02/2024]: Note - took away ssa_instructions[curr_instr_list]
// {
//     this->blockNum = debugNum++;

//     this->parent = nullptr;
//     this->parent2 = nullptr;
//     this->child = nullptr;
//     this->child2 = nullptr;

//     if (isConst) {
//         this->constList = new LinkedList();
//     } else {
//         this->constList = nullptr;

//         for (const auto &instr : instrLst) {
//             this->instrList.insert({instr.first, new LinkedList(*(instr.second))});
//             // this->instrList.insert(instr);
//         }
//         // this->instrList = instrLst;
//     }
//     this->newInstrs = {};

//     #ifdef DEBUG
//         std::cout << "new BasicBlock; got [instrList; size=" << this->instrList.size() << ", " << instrLst.size() << "] looks like:" << std::endl;
//         this->printInstrList();
//     #endif
// }

// // [09/27/2024]: Copied from above, probs use this cause we need to inherit the map (&& SSA DLL)
// // [09/02/2024]: Note - took away ssa_instructions[curr_instr_list]
// BasicBlock::BasicBlock(std::unordered_map<int, int> DOM_vv_map, std::unordered_map<int, LinkedList*> instrLst, bool isConst)
// {
//     this->blockNum = debugNum++;

//     this->parent = nullptr;
//     this->parent2 = nullptr;
//     this->child = nullptr;
//     this->child2 = nullptr;
//     this->updated_varval_map = DOM_vv_map;

//     if (isConst) {
//         this->constList = new LinkedList();
//     } else {
//         for (const auto &instr : instrLst) {
//             this->instrList.insert({instr.first, new LinkedList(*(instr.second))});
//             // this->instrList.insert(instr);
//         }
//         // this->instrList = instrLst;
        
//         this->constList = nullptr;
//     }
//     this->newInstrs = {};

//     #ifdef DEBUG
//         std::cout << "new BasicBlock; got [instrList; size=" << this->instrList.size() << ", " << instrLst.size() << "] looks like:" << std::endl;
//         this->printInstrList();
//     #endif
// }
// // special block: [join]; need phi function here
// BasicBlock::BasicBlock(BasicBlock *p1, BasicBlock *p2, std::unordered_map<int, int> DOM_vv_map, std::unordered_map<int, LinkedList*> instrLst)
// {
//     this->blockNum = debugNum++;

//     this->parent = p1;
//     this->parent2 = p2;
//     this->updated_varval_map = DOM_vv_map;
    
//     for (const auto &instr : instrLst) {
//         this->instrList.insert({instr.first, new LinkedList(*(instr.second))});
//         // this->instrList.insert(instr);
//     }
//     // this->instrList = instrLst;
    
//     this->child = nullptr;
//     this->child2 = nullptr;

//     this->newInstrs = {};

//     #ifdef DEBUG
//         std::cout << "new BasicBlock; got [instrList; size=" << this->instrList.size() << ", " << instrLst.size() << "] looks like:" << std::endl;
//         this->printInstrList();
//     #endif
// }


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
    this->updated_varval_map = DOM_vv_map;

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
    this->updated_varval_map = DOM_vv_map;
    
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