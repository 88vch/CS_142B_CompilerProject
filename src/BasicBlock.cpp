// [10.22.2024]: is there even a need for the constList anymore?
// - just combine it all into the [newInstrs] and have the [prev] ptr's

#include "BasicBlock.hpp"

#define DEBUG

int BasicBlock::debugNum = 0;
std::unordered_map<int, SSA *> BasicBlock::ssa_table = {};
std::unordered_map<SSA *, int> BasicBlock::ssa_table_reversed = {};

std::unordered_map<int, LinkedList*, BasicBlock::SafeCustomHash, BasicBlock::SafeCustomEqual> BasicBlock::instrList = {};

// [10.22.2024]: Revised?
BasicBlock::BasicBlock(bool isConst, int blkType, bool mainWhile)
{
    #ifdef DEBUG
        std::cout << "in BasicBlock(isConst=" << isConst << ")" << std::endl;
    #endif

    this->blockNum = debugNum++;

    this->parent = nullptr;
    this->parent2 = nullptr;
    this->child = nullptr;
    this->child2 = nullptr;

    this->blkType = blkType;
    this->mainWhile = mainWhile;

    if (isConst) {
        // this->constList = new LinkedList();
        this->constList = BasicBlock::instrList.at(0); // [12.08.2024]: why create new when we can js use pre-existing? isnt' this more efficent and accurate?
        // this->constPtr = new Node();
        this->blkType = 0;
    } else {
        this->constList = nullptr;
        // this->constPtr = nullptr;
    }
    this->newInstrs = {};
    this->varVals = {};

    // #ifdef DEBUG
    //     std::cout << "new BasicBlock; got [instrList; size=" << this->instrList.size() << ", " << instrLst.size() << "] looks like:" << std::endl;
    //     this->printInstrList();
    // #endif
    #ifdef DEBUG
        std::cout << "new BasicBlock looks like: " << this->toString() << "varVals: {}" << std::endl;
    #endif
}

BasicBlock::BasicBlock(std::unordered_map<int, int> DOM_vv_map, bool isConst, int blkType, bool mainWhile)
{
    this->blockNum = debugNum++;

    this->parent = nullptr;
    this->parent2 = nullptr;
    this->child = nullptr;
    this->child2 = nullptr;
    this->varVals = DOM_vv_map;

    this->blkType = blkType;
    this->mainWhile = mainWhile;

    if (isConst) {
        // this->constList = new LinkedList();
        this->constList = BasicBlock::instrList.at(0); // [12.08.2024]: why create new when we can js use pre-existing? isnt' this more efficent and accurate?
        // this->constPtr = new Node();
        this->blkType = 0;
    } else {
        this->constList = nullptr;
        // this->constPtr = nullptr;
    }
    this->newInstrs = {};

    #ifdef DEBUG
        std::cout << "new BasicBlock; got [varVals: size=" << this->varVals.size() << "] looks like:" << std::endl;
        this->printVVs();
    #endif
    // #ifdef DEBUG
    //     std::cout << "new BasicBlock; got [instrList; size=" << this->instrList.size() << ", " << instrLst.size() << "] looks like:" << std::endl;
    //     this->printInstrList();
    // #endif
}
BasicBlock::BasicBlock(BasicBlock *p1, BasicBlock *p2, std::unordered_map<int, int> DOM_vv_map, int blkType, bool mainWhile)
{
    this->blockNum = debugNum++;

    this->parent = p1;
    this->parent2 = p2;
    this->varVals = DOM_vv_map;
    
    this->child = nullptr;
    this->child2 = nullptr;

    this->constList = nullptr;
    // this->constPtr = nullptr;
    
    this->blkType = blkType;
    this->mainWhile = mainWhile;

    this->newInstrs = {};

    #ifdef DEBUG
        std::cout << "new BasicBlock; got [varVals: size=" << this->varVals.size() << "] looks like:" << std::endl;
        this->printVVs();
    #endif
}

// update the [newInstrs] in this BB
// this function is called after we update this BB's varVal mapping with a new update
// - todo: search through each newInstr ( each {x, y} in the SSA )
void BasicBlock::updateInstructions(SSA *oldVal, SSA *newVal) {
    // #ifdef DEBUG
    //     std::cout << "BB before updateInstructions: " << std::endl << this->toString() << std::endl;
    // #endif
    
    if ((this->findSSA(oldVal) == false) || ((this->findSSA(oldVal)) && (oldVal->get_operator() == 0))) {
        for (size_t i = 0; i < this->newInstrs.size(); i++) {
            SSA *curr = this->newInstrs.at(i)->instr;
            if (curr->get_operator() != 6) { // do not update the phi functions!
                curr->updateIfHas(oldVal, newVal);
            }
        }
    }
    // #ifdef DEBUG
    //     std::cout << "BB after updateInstructions: " << std::endl << this->toString() << std::endl;
    // #endif
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

// returns the updated vector after remove
void BasicBlock::removeSSA(SSA *toRemove) { /* ,  std::vector<SSA*> &debugSSA_instrs) { */
    Node *curr = nullptr;

    for (auto it = this->newInstrs.begin(); it != this->newInstrs.end(); ++it) {
        curr = *it;
        if (curr->instr->get_debugNum() == toRemove->get_debugNum()) {
            int op = curr->instr->get_operator();
            #ifdef DEBUG
                std::cout << "found SSA match! curr: " << curr->instr->toString() << std::endl;
                std::cout << "linekd list before removal: " << std::endl;
                BasicBlock::instrList.at(op)->printList();
            #endif
            
            // Update links to maintain list integrity
            if (curr->prev) {
                #ifdef DEBUG
                    std::cout << "curr->prev exists" << std::endl;
                #endif
                curr->prev->next = curr->next;
            }
            if (curr->next) {
                #ifdef DEBUG
                    std::cout << "curr->next exists" << std::endl;
                #endif
                curr->next->prev = curr->prev;
            }

            if (curr == BasicBlock::instrList.at(op)->tail) {
                #ifdef DEBUG
                    std::cout << "curr was tail" << std::endl;
                #endif
                if (curr->prev) {
                    BasicBlock::instrList.at(op)->tail = curr->prev;
                } else {
                    BasicBlock::instrList.at(op)->tail = nullptr;
                }
            }

            // [12.09.2024]: don't free the [SSA::instr]'s mem tho for debugging purposes
            // - we also free all at the end
            // Free memory and erase the pointer from the vector
            // delete curr->instr;
            curr->instr = nullptr;
            delete curr;
            this->newInstrs.erase(it);
            // this->newInstrs.erase(std::remove(this->newInstrs.begin(), this->newInstrs.end(), curr), this->newInstrs.end());

            #ifdef DEBUG
                std::cout << "curr was deleted! newInstrs look like: " << this->printNewInstrs() << std::endl;
                std::cout << "instrList at ident looks like: " << std::endl;
                BasicBlock::instrList.at(op)->printList();
            #endif
            break;  // Exit after deletion to avoid invalid iterator usage
        }
    }

    // SSA *tmp = nullptr;
    // for (size_t i = 0; i < debugSSA_instrs.size(); i++) {
    //     tmp = debugSSA_instrs.at(i);

    //     if (tmp->get_debugNum() == toRemove->get_debugNum()) {
    //         debugSSA_instrs.erase(debugSSA_instrs.begin() + i);
    //         break;
    //     } 
    // }
}

// bool BasicBlock::identDefinedHere(SSA *prevVal) const {
//     for (const auto &n : this->newInstrs) {
//         if ((s->compare(n->instr)) && ()) {}
//     }

//     return false;
// }