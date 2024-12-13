#ifndef PARSER_HPP
#define PARSER_HPP

#include <stack>
#include <queue>
#include <vector>
#include <sstream>
#include <cstring>
#include <fstream>
#include <algorithm>
// #include <stdio.h>
// #include <cstdio>
#include "Result.hpp"
#include "BasicBlock.hpp"
#include "SSA.hpp"
#include "LinkedList.hpp"
#include "TinyExceptions.hpp"

#define DEBUG



// parse(): IR through BasicBlocks

// Note: all nodes will be inherited from BasicBlock
// Note: shouldn't need keywords bc we only care about computational code
class Parser {
public:
    static std::unordered_map<Func*, Parser*> funcMap; // [11.22.2024]: the map of [func:Parser*] since each [funcDecl] will define it's own [funcBody]

    Parser(const std::vector<Result> &tkns, bool blk = false, bool isFunc = false, Func *f = nullptr)
    {
        this->source = tkns;

        this->SSA_start = nullptr;
        this->SSA_parent = nullptr;

        this->s_index = 0;
        this->source_len = tkns.size();
        this->SSA_instrs = {};

        // [12.08.2024]: changed from int i = [0] to [1]
        // - since we modified [constBB] to use [BB::instrList]'s LL
        for (unsigned int i = 0; i < SymbolTable::operator_table.size(); i++) {
            BasicBlock::instrList.insert({i, new LinkedList()});
        }

        this->currBB = nullptr;
        // this->BB0 = new BasicBlock(this->instrList, true);
        this->BB0 = new BasicBlock(true);
        if (blk) {
            this->VVs = this->BB0->varVals;
        }
        // this->startBB = nullptr;
        // this->parentBB = this->BB0;

        this->prevInstrs = std::stack<SSA *>();
        this->prevJump = false;
        this->prevInstr = nullptr;

        this->block = blk;
        this->blksSeen = {};

        this->func = nullptr;
        this->isFunc = false;
        if (isFunc) {
            this->func = f;
            this->isFunc = true;
        }

        next(); // [10/14/2024]: Why do we do this?
        // - load's first token into [this->sym]
    }

    ~Parser() {
        #ifdef DEBUG
            std::cout << "Start ~Parser()" << std::endl;
        #endif
        delete this->BB0;
        
        for (unsigned int i = 1; i < BasicBlock::instrList.size(); i++) {
            // #ifdef DEBUG
            //     std::cout << "deleting linked list [" << SymbolTable::operator_table_reversed.at(i) << "];" << std::endl;
            // #endif
            delete BasicBlock::instrList.at(i);
            BasicBlock::instrList[i] = nullptr;
        }

        // Then delete the SSA instructions themselves (note ~LinkedList() doesn't do this bc we delete BB LinkedLists too there...)
        for (SSA *instr : this->SSA_instrs) {
            delete instr;
            instr = nullptr;
        }
        this->SSA_instrs.clear();

        #ifdef DEBUG
            std::cout << "Done ~Parser()!!!" << std::endl;
        #endif
    }

    // [07/25/2024]: ToDo
    inline int getOp() const {
        #ifdef DEBUG
            std::cout << "\t[Parser::getOp()]; this->sym.kind=" << this->sym.get_kind_literal() << ", this->sym.value=" << this->sym.get_value_literal() << std::endl;
        #endif
        switch (this->sym.get_kind_literal()) {
            case 0: // const
                return SymbolTable::operator_table.at("const");
                break;
            case 1: // ident
                break;
            case 2: // keyword
                switch (this->sym.get_value_literal()) {
                    case 0: // `+`
                        return SymbolTable::operator_table.at("add");
                    case 1: // `-`
                        return SymbolTable::operator_table.at("sub");
                    case 2: // `*`
                        return SymbolTable::operator_table.at("mul");
                    case 3: // `/`
                        return SymbolTable::operator_table.at("div");
                }
                break;
        }
        return -1; // stub
    }


    inline SSA* CheckExistence(const int op, SSA *x, SSA *y) const {
        #ifdef DEBUG
            std::cout << "\t[Parser::CheckExistence(op=[" << op << "], x=";
            if (x) {
                std::cout << x->toString();
            } else {
                std::cout << "nullptr";
            }
            std::cout << ",y=";
            if (y) { 
                std::cout << y->toString();
            } else {
                std::cout << "nullptr" << std::endl;
            }
            std::cout << "), this->SSA_instrs.size()=" << this->SSA_instrs.size() << "]" << std::endl;
        #endif
        SSA *ret = nullptr;
        
        if (BasicBlock::instrList.find(op) == BasicBlock::instrList.end()) {
            return nullptr;
        }

        // [09/19/2024]: Replaced Below (previous=this->SSA_instrs; current=this->instrList)
        Node *curr = BasicBlock::instrList.at(op)->tail;
        #ifdef DEBUG
            std::cout << "got tail with ssa: ";
            if (curr) {
                std::cout << curr->instr->toString() << std::endl;
            } else {
                std::cout << "nullptr" << std::endl;
            }
        #endif


        while (curr) {
            #ifdef DEBUG
                std::cout << "\t\tcomparing instr=[" << curr->instr->toString() << "]" << std::endl;
            #endif
            if (curr->instr->compare(op, x, y)) {
                #ifdef DEBUG
                    std::cout << "\t\tgot true!!!" << std::endl;
                #endif
                ret = curr->instr;
                break;
            } else {
                #ifdef DEBUG
                    std::cout << "\t\tgot false" << std::endl;
                #endif
            }
            curr = curr->prev;
        }

        #ifdef DEBUG
            if (ret != nullptr) {
                std::cout << "\t\treturning: [" << ret->toString() << "]" << std::endl;
            } else { 
                std::cout << "\t\treturning: nullptr!" <<std::endl;
            }
        #endif
        return ret;
    }

    // [09/20/2024]: Const is not found in instrList, rather in BB0
    // [08/22/2024]: might need to revise (like CheckExistence()) to include op and curr sym's values
    inline SSA* CheckConstExistence(int val = -1) const {
        // #ifdef DEBUG
        //     std::cout << "in Parser::CheckConstExistence(val=" << val << ")" << std::endl;
        // #endif
        int comparisonVal;
        if (val == -1) {
            comparisonVal = this->sym.get_value_literal();
            // #ifdef DEBUG
            //     // std::cout << "\t[Parser::CheckExistence(op=" << op << "), this->sym.get_value_literal()=" << this->sym.get_value_literal() << "]" << std::endl;
            //     std::cout << "\tcomparing value: [" << comparisonVal << "], [this->SSA_instrs].size() =" << this->SSA_instrs.size() << std::endl;
            // #endif
        } else {
            comparisonVal = val;
        }
        SSA *ret = nullptr;

        if (this->BB0->constList->tail == nullptr) {
            return nullptr;
        }

        // [09/20/2024]: Replaced from using [this->instrList] to [this->BB0->constList]
        // [09/19/2024]: Replaced Below (previous=this->SSA_instrs; current=this->instrList)
        Node *curr = this->BB0->constList->tail;
        while (curr) {
            // #ifdef DEBUG
            //     std::cout << "\tthis instr: [" << curr->instr->toString() << "]" << std::endl;
            // #endif
            if (curr->instr->compareConst(comparisonVal)) {
                ret = curr->instr;
                // #ifdef DEBUG
                //     std::cout << "ret is now: " << ret->toString() << std::endl;
                // #endif
                break;
            }
            curr = curr->prev;
        }
        return ret;
    }

    // [07/25/2024]: ToDo
    inline SSA* BuildIR(int op, SSA *x, SSA *y) {
        #ifdef DEBUG
            std::cout << "\t[Parser::BuildIR(op=" << op << ", this->sym=" << SymbolTable::symbol_table.at(op) << ")]: SSA x = " << x->toString() << ", SSA y = " << y->toString() << std::endl;
        #endif
        SSA *ret = CheckExistence(op, x, y); // [08/12/2024]: Note we might have to include the `op` here to check diff types (i.e. `+` vs `*`)
        // if we don't get [nullptr] from CheckExistence(), that implies there was a previous SSA of the exact same type; so rather than creating a dupe, we can simply use the previously existing one (Copy Propagation)
        //      Note: check that all previous instructions will DOM this SSA instr
        #ifdef DEBUG
            std::cout << "\treturn value from [this->CheckExistence]: ";
        #endif
        if (ret) { 
            #ifdef DEBUG
                std::cout << ret->toString() << std::endl;
            #endif
            return ret; 
        } else {
            #ifdef DEBUG
                std::cout << "nullptr!" << std::endl;
            #endif
        }

        // assume op represents the keyword on the [SymbolTable::symbol_table]
        switch (op) {
                case 0: // `+`
                    // ret = new SSA(SymbolTable::operator_table.at("add"), x, y);
                    #ifdef DEBUG
                        std::cout << "\t\tcase 0 (+): " << std::endl;
                    #endif
                    ret = new SSA(SymbolTable::operator_table.at("add"), x, y);
                    #ifdef DEBUG
                        std::cout << "\t\tgot ret: " << ret->toString() << std::endl;
                    #endif
                    break;
                case 1: // `-`
                    ret = new SSA(SymbolTable::operator_table.at("sub"), x, y);
                    break;
                case 2: // `*`
                    ret = new SSA(SymbolTable::operator_table.at("mul"), x, y);
                    break;
                case 3: // `/`
                    ret = new SSA(SymbolTable::operator_table.at("div"), x, y);
                    break;
            }
        // this->SSA_instrs.push_back(ret);
        return ret;
    }

    // [10.21.2024]: Create a deep copy of the instrList for now
    // - ...wait, then we should just create deep copy of [instrList] for each BB
    // - , which implies that our BB-destructor should be changed accordingly
    // - , and brings up the possibility for the removal of [this->instrList] in [Parser]
    //      - if we could just keep it between the BB's wouldn't that be even better (?)
    // [10.22.2024]: ok so revised; we'll keep it only in Parser (remove from BB's)
    // - and only keep a vector<Node*> in BB but those Node* point to prev-SSA-instrs
    // -, so it's kinda like we have an artificial LinkedList but not really
    
    // -, concern is placed on the [BasicBlock::constList] since we don't know how we will map / check instructions?...ignoring for now :(
    
    // inline std::unordered_map<int, LinkedList*> copyInstrList() const {
    //     std::unordered_map<int, LinkedList*> res = {};

    //     for (const auto &pair : this->instrList) {
    //         res.insert({pair.first, new LinkedList(*pair.second)});
    //     }

    //     return res;
    // }

    inline void checkPrevJmp(SSA *instr) {
        if (this->prevJump) {
            // [09/22/2024]: Assumption - [this->prevInstr] is alr in the instrList
            this->prevInstr = this->prevInstrs.top();
            
            #ifdef DEBUG
                std::cout << "\tprevJump exists with prevInstr: [" << this->prevInstr->toString() << "]!" << std::endl;
            #endif
            
            if (this->prevInstr->get_operator() == 8) {
                this->prevInstr->set_operand1(instr);
            } else {
                this->prevInstr->set_operand2(instr);
            }

            // this->prevJump = false;
            this->prevInstr = nullptr;
            this->prevInstrs.pop();

            // [10.24.2024]: trying something out...
            if (this->prevInstrs.empty()) {
                this->prevJump = false;
            }
        }
    }

    // [11.29.2024]: created [addSSA1()] for const
    inline SSA* addSSA1(int op, int val, bool check = false) {
        // [10.23.2024]: Assume addSSA1() handles deletion properly...
        if (op != 0) {
            std::cout << "addSSA1() overload expected constVal" << std::endl;
            exit(EXIT_FAILURE);
        }
        SSA *tmp = new SSA(op, val);
        SSA *res = this->addSSA1(tmp, check);
        
        // #ifdef DEBUG
        //     std::cout << "in addSSA1(const) returned from [addSSA1()] with val: " << res->toString() << std::endl;
        //     std::cout << "tmp looks like: ";
        //     if (tmp) {
        //         std::cout << tmp->toString() << std::endl;
        //     } else {
        //         std::cout << "nullptr!" << std::endl;
        //     }
        // #endif
        // if (tmp && tmp->compare(res) == false) {
        //     delete tmp;
        // }

        tmp = nullptr;
        return res;
    }

    inline SSA* addSSA1(int op, SSA *x = nullptr, SSA *y = nullptr, bool check = false) {
        // [10.23.2024]: Assume addSSA1() handles deletion properly...
        SSA *tmp = new SSA(op, x, y);
        SSA *res = this->addSSA1(tmp, check);

        // if (tmp && tmp->compare(res) == false) {
        //     delete tmp;
        // }

        tmp = nullptr;
        return res;
    }
        
    // [10/06/2024]: When wouldn't we want to check (A: phi)
    // [10/09/2024]: If the SSA already exists, we don't need to check for it and just add it like normally
    inline SSA* addSSA1(SSA *instr, bool check = false) {
        #ifdef DEBUG
            std::cout << "in addSSA1(instr=" << instr->toString() << ", check=" << check << ")" << std::endl;
        #endif
        // [10/05/2024]: todo refactor addSSA() with additional prameter to determine if need to check for existence
        if (instr->get_constVal()) {
            // #ifdef DEBUG
            //     std::cout << "\tinstr is const" << std::endl;
            // #endif
            if (check) {
                SSA *tmp = this->CheckConstExistence(*(instr->get_constVal())); 
                if (tmp == nullptr) {
                    instr = this->addSSA(instr);
                } else {
                    // #ifdef DEBUG
                    //     std::cout << "instr(" << instr->toString() << ") exists already: " << tmp->toString() << std::endl;
                    // #endif
                    if (instr->compare(tmp) == false) {
                        // #ifdef DEBUG
                        //     std::cout << "deleting instr!" << std::endl;
                        // #endif
                        // [10.23.2024]: CAN WE DO THIS???
                        delete instr;
                        this->BB0->updateConstInstrs();
                        instr = tmp;
                    }
                }
            } else {
                // #ifdef DEBUG
                //     std::cout << "not checking! instr=" << instr->toString() << std::endl;
                // #endif
                instr = this->addSSA(instr);
            }
        } else {
            // #ifdef DEBUG
            //     std::cout << "\tinstr is NOT const" << std::endl;
            // #endif
            if (check) {
                SSA *tmp = this->CheckExistence(instr->get_operator(), instr->get_operand1(), instr->get_operand2());
                if (tmp == nullptr) {
                    instr = this->addSSA(instr);
                } else {
                    // #ifdef DEBUG
                    //     std::cout << "instr(" << instr->toString() << ") exists already: " << tmp->toString() << std::endl;
                    // #endif
                    // [10.23.2024]: CAN WE DO THIS???
                    delete instr;
                    instr = tmp;

                    // #ifdef DEBUG
                    //     std::cout << "instr is now: " << instr->toString() << std::endl;
                    // #endif
                }
            } else {
                // #ifdef DEBUG
                //     std::cout << "not checking! instr=" << instr->toString() << std::endl;
                // #endif
                instr = this->addSSA(instr);
            }
        }

        #ifdef DEBUG
            std::cout << "returning this->addSSA1() instr=" << instr->toString() << std::endl;
        #endif
        return instr;
    }

    // [11.24.2024]: segfault here when [getparX] new [X] since LL wasn't added into [BB::instrList]
    inline SSA* addSSA(SSA *instr) {
        #ifdef DEBUG
            std::cout << "\t[Parser::addSSA(instr=" << instr->toString() << ")]" << std::endl;
        #endif
        if (instr->get_operator() == 0) {
            #ifdef DEBUG
                std::cout << "\tinstr op == 0 (const)" << std::endl;
            #endif
            // [09/20/2024]: new constList specifically for [this->BB0]
            // [10.23.2024]: [LinkedList::InsertAtTail()] returns [Node*]
            if (this->block) {
                this->currBB->newInstrs.push_back(this->BB0->constList->InsertAtTail(instr));
                #ifdef DEBUG
                    std::cout << "done pushing into [currBB]'s [newInstrs]" << std::endl;
                #endif
            } else {
                instr = this->BB0->constList->InsertAtTail(instr)->instr;
            }
        } else {
            #ifdef DEBUG
                std::cout << "\tinstr op == " << instr->get_operator() << std::endl;
            #endif
            
            if (this->block) {
                // #ifdef DEBUG
                //     std::cout << "currBB looks like: ";
                //     if (this->currBB) {
                //         std::cout << this->currBB->toString() << std::endl;
                //     } else {
                //         std::cout << "nullptr!" << std::endl;
                //     }
                // #endif
                if (instr->get_operator() == 6) {
                    // [11.01.2024]: phi instructions go ontop of [this->newInstrs]
                    // - insert into initial position
                    this->currBB->newInstrs.insert(this->currBB->newInstrs.begin(), BasicBlock::instrList.at(instr->get_operator())->InsertAtTail(instr));
                } else {
                    // [10.23.2024]: [LinkedList::InsertAtTail()] returns [Node*]
                    Node *retVal = BasicBlock::instrList.at(instr->get_operator())->InsertAtTail(instr);

                    // #ifdef DEBUG
                    //     std::cout << "successfully inserted Node* into BB's :: LL ; instr looks like: " << retVal->instr->toString() << std::endl;
                    // #endif

                    this->currBB->newInstrs.push_back(retVal);
                }
                #ifdef DEBUG
                    std::cout << "done pushing into [currBB]'s [newInstrs]" << std::endl;
                #endif
            } else {
                instr = BasicBlock::instrList.at(instr->get_operator())->InsertAtTail(instr)->instr;
            }
        }

        // [10.20.2024]: Modification
        this->checkPrevJmp(instr);

        // this->currBB->setInstructionList(this->instrList);

        this->SSA_instrs.push_back(instr);
        return instr;
    }

    // [10/09/2024]: ToDo - should check if [SSA] already exists in [this->BasicBlock::ssa_table] 
    // - specifically for [this->VVs]
    inline int add_SSA_table(SSA *toAdd) {
        BasicBlock::ssa_table.insert(std::pair<int, SSA *>(BasicBlock::ssa_table.size(), toAdd));
        BasicBlock::ssa_table_reversed.insert(std::pair<SSA *, int>(toAdd, BasicBlock::ssa_table.size() - 1));

        return BasicBlock::ssa_table.size() - 1; // [09/30/2024]: return the int that is associated with SSA-instr
    }

    // // [12.02.2024]: assumes the SSA was created in this->currBB
    // inline void removeSSA(SSA *s) {
    //     for (auto &ssa : this->currBB->newInstrs) {
    //         if (s->compare(ssa->instr)) {
    //             if (ssa->next) {
    //                 ssa->next->prev = ssa->prev;
    //             }
    //             if (ssa->prev) {
    //                 ssa->prev->next = ssa->next;
    //             }
    //             this->currBB->newInstrs.erase(std::remove(this->currBB->newInstrs.begin(), this->currBB->newInstrs.end(), ssa), this->currBB->newInstrs.end());
    //             break;
    //         }
    //     }
    //     // we remove all SSA from [BasicBlock::ssa_table] && [reversed] at the end of parsing...
    // }

    // recursive; maintains [this->currBB]
    inline void propagateDown(BasicBlock *curr, int ident, SSA* oldVal, int ident_val, bool first = false, std::vector<BasicBlock *> seen = {}) {    
        #ifdef DEBUG
            std::cout << "in propagateDown(ident=" << SymbolTable::symbol_table.at(ident) << ", phi_ident=" << BasicBlock::ssa_table.at(ident_val)->toString() << ", oldVal (SSA) = " << oldVal->toString() << ", )" << std::endl;
        #endif
        seen.push_back(curr);

        if (!first && this->currBB->compare(curr)) {
            #ifdef DEBUG
                std::cout << "returning: in a loop! found startBB looks like: " << std::endl << this->currBB->toString() << std::endl;
            #endif
            return;
        }

        if (curr->varVals.find(ident) != curr->varVals.end()) {
            if (curr->varVals.at(ident) != ident_val) {
                #ifdef DEBUG
                    std::cout << "currBB's varVals contains the following pair: {ident=" << ident << ", val=" << curr->varVals.at(ident) << "}; updating with [ident_val]" << std::endl;
                #endif
                SSA *prevVal = BasicBlock::ssa_table.at(curr->varVals.at(ident));
                // [12.06.2024]: new updated to include (mainWhile unique update)
                if (curr->blkType == 1 && curr->mainWhile) {
                    // [12.06.2024]: should update the phi-SSA not the SSA associated with the ident here
                    #ifdef DEBUG
                        std::cout << "in mainWhile-blk looks like: " << std::endl << curr->toString() << std::endl;
                    #endif
                    if (BasicBlock::ssa_table.at(curr->varVals.at(ident))->get_operator() == 6) {
                        prevVal->set_operand2(BasicBlock::ssa_table.at(ident_val));
                        curr->updateInstructions(prevVal, BasicBlock::ssa_table.at(ident_val));
                        curr->varVals.insert_or_assign(ident, ident_val);
                    } else {
                        if (prevVal->compare(oldVal)) {            
                            curr->updateInstructions(prevVal, BasicBlock::ssa_table.at(ident_val));
                            curr->varVals.insert_or_assign(ident, ident_val);
                            #ifdef DEBUG
                                std::cout << "updated ident [" << std::to_string(ident) << "] with val ident_val; bb Looks like: " << std::endl << curr->toString() << std::endl;
                            #endif
                        }
                    }
                    #ifdef DEBUG
                        std::cout << "mainWhile-blk after updating prevPhi looks like: " << std::endl << curr->toString() << std::endl;
                    #endif
                } else {
                    // if varVal mapping ident != ident_val, (waht does this mean)?
                    //      - means that [ident : old_ident_val] ?
                    // [12.11.2024]: if we update a val whose prev val was a phi && the phi is in [currBB] newInstr's, delete the phi...
                    
                    curr->updateInstructions(oldVal, BasicBlock::ssa_table.at(ident_val));
                    if (prevVal->compare(oldVal)) {
                        curr->varVals.insert_or_assign(ident, ident_val);
                        
                        if (curr->findSSA(prevVal)) {
                            curr->removeSSA(prevVal);
                            // if (prevVal->get_operator() == 0) {
                            //     delete prevVal;
                            //     prevVal = nullptr;
                            //     this->BB0->updateConstInstrs();
                            // }
                            #ifdef DEBUG
                                std::cout << "removed prevVal from currBB~" << std::endl;
                            #endif
                        }
                    }
                }
            } else {
                #ifdef DEBUG
                    std::cout << "currBB's varVals contains [ident:ident_val] pair!" << std::endl;
                #endif
                // 11.09.2024: no need to update instructions if proper mapping already exists
            }
        } else {
            #ifdef DEBUG
                std::cout << "ident doesn't exist in currBB! inserting new" << std::endl;
            #endif
            curr->varVals.insert_or_assign(ident, ident_val);
            curr->updateInstructions(oldVal, BasicBlock::ssa_table.at(ident_val));

            // 11.06.2024: nah i think smtn with the logic is wrong abt this...
            // 11.05.2024: if we don't find the [ident] defined as a key in [this->currBB->varVals],
            // - does it imply that we've reached the end of propagate (since this means there is no loop?; that's my assumption)?
        }

        if ((curr->child) && (std::find(seen.begin(), seen.end(), curr->child) == seen.end())) {
            propagateDown(curr->child, ident, oldVal, ident_val, false, seen);
        } 
        if ((curr->child2) && (std::find(seen.begin(), seen.end(), curr->child2) == seen.end())) {
            propagateDown(curr->child2, ident, oldVal, ident_val, false, seen);
        }
        // stub
        #ifdef DEBUG
            std::cout << "returning normally: no children remain! curr looks like: " << std::endl << curr->toString() << std::endl;
        #endif
    }

    // [11.11.2024]: a function which iterates thorough BBs checking to see
    // - if [oldVal] DOM [newVal]; i.e. they come from the same path then we don't need phi (bc they come from the same path)
    inline bool SSAisDOM(SSA *oldVal, SSA *newVal) const {
        #ifdef DEBUG
            std::cout << "entering [SSAisDOM] with [oldVal && newVal];" << std::endl << "\t[oldVal]: " << oldVal->toString() << std::endl << "\t[newVal]: " << newVal->toString() << std::endl << std::endl;
        #endif
        BasicBlock *curr = nullptr;
        bool oldFound = false;

        std::vector<BasicBlock *>seen = {};
        std::stack<BasicBlock *> s;
        s.push(this->BB0);
        seen.push_back(this->BB0);

        while (!s.empty()) {
            curr = s.top();
            s.pop();
            #ifdef DEBUG
                std::cout << curr->toString() << std::endl;
            #endif

            if (curr->findSSA(oldVal)) {
                #ifdef DEBUG    
                    std::cout << "found oldVal" << std::endl;
                #endif
                oldFound = true; // 11.11.2024: there should be no way that oldVal && newVal are in the sameBB (RIGHT?)
            }
            if (curr->findSSA(newVal)) {
                #ifdef DEBUG
                    std::cout << "found newVal" << std::endl;
                #endif
                if (oldFound) {
                    return true;
                } else { // if newVal was seen before old, false
                    return false;
                }
            }


            if (curr->child && (std::find(seen.begin(), seen.end(), curr->child) == seen.end())) {
                s.push(curr->child);
                seen.push_back(curr->child);
            }
            if (curr->child2 && (std::find(seen.begin(), seen.end(), curr->child2) == seen.end())) {
                s.push(curr->child2);
                seen.push_back(curr->child2);
            }
        }
        // if we've reached this point, it means that we couldn't find [newVal]
        #ifdef DEBUG
            std::cout << "exiting [Parser::SSAisDOM] with return val (false); coudln't find [newVal]: " << newVal->toString() << std::endl;
        #endif
        return false; 
    }

    // [11.11.2024]: return true if b dom d or b loops back to d, false otherwise
    inline bool BBisDOMLoop(BasicBlock *b, BasicBlock *d) const {
        #ifdef DEBUG
            std::cout << "[Pa热色入：BBisDOM()::b] 看像： " << b->toString() << std::endl;
            std::cout << "[Parser::BBisDOM()::d] 看像: " << d->toString() << std::endl;
        #endif
        BasicBlock *curr = nullptr;
        std::stack<BasicBlock *> s;
        std::vector<BasicBlock *> seen = {};
        s.push(b);
        seen.push_back(b);

        while (!s.empty()) {
            curr = s.top();
            s.pop();

            if ((curr->child) && (std::find(seen.begin(), seen.end(), curr->child) == seen.end())) {
                if (curr->child->compare(d)) { return true; }
                s.push(curr->child);
                seen.push_back(curr->child);
            }
            if ((curr->child2) && (std::find(seen.begin(), seen.end(), curr->child2) == seen.end())) {
                if (curr->child2->compare(d)) { return true; }
                s.push(curr->child2);
                seen.push_back(curr->child2);
            }
        }
        return false;
    }

    // [12.06.2024]: a function that returns true if [this->currBB] is in a loop, false otherwise
    inline bool currBBinLoop() {
        #ifdef DEBUG
            std::cout << "currBBinLoop(this->currBB=[" << std::to_string(this->currBB->blockNum) << "])" << std::endl;
        #endif
        BasicBlock *old = this->currBB;

        // while (curr) {
        if (std::find(this->blksSeen.begin(), this->blksSeen.end(), this->currBB->blockNum) == this->blksSeen.end()) {
            this->blksSeen.push_back(this->currBB->blockNum);
        } else {
            #ifdef DEBUG
                std::cout << "we've seen this BB before, loop confirmed!" << std::endl;
            #endif
            return true;
        }
            // // [12.06.2024]: prioritize choosing child2 if we have a choice since child2 will always be the loop-back child
            // curr = (curr->child2) ? curr->child2 : ((curr->child) ? curr->child : nullptr);
        
        BasicBlock *tmp = this->currBB;
        bool r1, r2;
        if (this->currBB->child2) {
            #ifdef DEBUG
                std::cout << "child2 [" << std::to_string(this->currBB->child2->blockNum) << "] exists" << std::endl;
            #endif
            this->currBB = this->currBB->child2;
            r2 = this->currBBinLoop();
        }
        this->currBB = tmp;
        if (this->currBB->child) {
            #ifdef DEBUG
                std::cout << "child [" << std::to_string(this->currBB->child->blockNum) << "] exists" << std::endl;
            #endif
            this->currBB = this->currBB->child;
            r1 = this->currBBinLoop();
        }
        this->currBB = old;
        // }
        return (r1 || r2);
    }

    // [12.08.2024]: this func should only be run in [p2_assignment()]
    // - assumes we know we are in a loop
    inline BasicBlock* getLoopHead() {
        BasicBlock *old = this->currBB;

        if (std::find(this->blksSeen.begin(), this->blksSeen.end(), this->currBB->blockNum) == this->blksSeen.end()) {
            this->blksSeen.push_back(this->currBB->blockNum);
            if (this->currBB->mainWhile) {
                BasicBlock *retVal = this->currBB;
                this->currBB = old;
                return retVal;
            } 
        }

        BasicBlock *tmp = this->currBB;
        BasicBlock *r1 = nullptr, *r2 = nullptr;
        if (this->currBB->child2) {
            this->currBB = this->currBB->child2;
            r2 = this->getLoopHead();
        }
        this->currBB = tmp;
        if (this->currBB->child) {
            this->currBB = this->currBB->child;
            r1 = this->getLoopHead();
        }
        this->currBB = old;
        // [12.06.2024]: prioritize choosing child2 if we have a choice since child2 will always be the loop-back child
        // curr = (curr->child2) ? curr->child2 : ((curr->child) ? curr->child : nullptr);
        return (r2) ? r2 : (r1 ? r1 : nullptr);
    }

    // [11/28/2024]: adds SSA_instrs from other [func]'s into main Parser's SSA_instrs
    // - this function is only called in [Parser::getSSA()]
    void updateSSA_instrs() {
        for (const auto &f : Parser::funcMap) {
            std::vector<SSA*> currInstrs = f.second->getSSA();
            #ifdef DEBUG
                std::cout << "got currInstrs for func [" << f.first->getName() << "] with size[" << currInstrs.size() << "]" << std::endl;
            #endif

            // [12.01.2024]: does this work instead of below?
            this->SSA_instrs.insert(this->SSA_instrs.end(), currInstrs.begin(), currInstrs.end());

            for (SSA *instr : currInstrs) {
                #ifdef DEBUG
                    std::cout << "pushing instr [" << instr->toString() << "] into [this->SSA_instrs]" << std::endl;
                #endif
                // [11.28.2024]: segfaulting here
                // - there's no way difff func's would use the same SSA? i lie but will ther be dupes here if we dont' check for existence?
                // if (std::find(this->SSA_instrs.begin(), this->SSA_instrs.end(), instr) == this->SSA_instrs.end()) {
                //     this->SSA_instrs.push_back(instr);
                // }
                this->SSA_instrs.push_back(instr);
            }
        }
    }

    // [11.28.2024]: this function is only called once in [main.cpp] after parsing to geneerate resuilt file
    inline std::vector<SSA*> getSSA() { 
        if (!this->isFunc) {
            this->updateSSA_instrs();
        }
        return this->SSA_instrs; 
    }

    // [11/28/2024]: adds SSA_instrs from other [func]'s into main Parser's SSA_instrs
    // - this function is only called in [Parser::getSSA()]
    void updateVarVals() {
        for (const auto &f : Parser::funcMap) {
            std::unordered_map<int, int> currVVs = f.second->VVs;
            #ifdef DEBUG
                std::cout << "got currVVs for func [" << f.first->getName() << "] with size[" << currVVs.size() << "]" << std::endl;
            #endif
            
            // [12.01.2024]: does this work instead of below?
            // this->VVs.insert(this->VVs.end(), currVVs.begin(), currVVs.end());
            
            for (const auto &instr : currVVs) {
                #ifdef DEBUG
                    std::cout << "pushing varVal [idnet: " << instr.first << ", val: " << instr.second << "] into [this->VVs]" << std::endl;
                #endif
                // [11.28.2024]: segfaulting here
                // - there's no way difff func's would use the same SSA? i lie but will ther be dupes here if we dont' check for existence?
                // if (std::find(this->SSA_instrs.begin(), this->SSA_instrs.end(), instr) == this->SSA_instrs.end()) {
                //     this->SSA_instrs.push_back(instr);
                // }
                this->VVs.insert(instr);
            }
        }
    }
    

    // [11.30.2024]: MUST add [func]'s varVals as well
    // [09/30/2024]: Converts to original varVal mapping from [this->VVs]
    std::unordered_map<std::string, SSA*> getVarVal() { 
        #ifdef DEBUG
            std::cout << "in getVarVal()" << std::endl;
            // std::cout << "this->currBB: " << std::endl << this->currBB->toString() << std::endl;
        #endif
        std::unordered_map<std::string, SSA *> res = {};

        // [12.01.2024]: add [func]'s VVs into [Parser::mainObj]
        this->updateVarVals();

        for (const auto pair : this->VVs) {
        // for (const auto  pair : this->currBB->varVals) {
            res.insert(std::pair<std::string, SSA *>(SymbolTable::symbol_table.at(pair.first), BasicBlock::ssa_table.at(pair.second)));
        }

        return res; 
    }

    // [11.11.2024]: assign an initial value of 0 if uninitialized var has been seen
    inline void handleUninitVar(int ident) {
        int constInt;

        this->varDeclarations.insert(ident);
        
        SSA *const0 = this->BB0->getConstSSA(0);
        if (const0 == nullptr) {
            const0 = this->addSSA1(new SSA(0, 0));
            constInt = this->add_SSA_table(const0);
        } else {
            constInt = BasicBlock::ssa_table_reversed.at(const0);
        }
        
        this->currBB->varVals.insert_or_assign(ident, constInt);
        this->VVs.insert_or_assign(ident, constInt);
        #ifdef DEBUG
            std::cout << "BB after handleUninitVar: " << std::endl << this->currBB->toString() << std::endl;
        #endif
    }

    inline void conditionalStmtPhiUpdate(std::unordered_set<int> vars, BasicBlock *if_parent, BasicBlock *then_blk, BasicBlock *else_blk) {
        // [12.09.2024]: if we have [if && else] blk, assumes [this->currBB == join_blk]
        #ifdef DEBUG
            std::cout << "else-blk exists!" << std::endl;
            std::cout << "[this->currBB] looks like: " << std::endl << this->currBB->toString() << std::endl;
        #endif
        BasicBlock *then_blk_last = this->currBB->parent;
        BasicBlock *else_blk_last = nullptr;
        if (this->currBB->parent2) {
            else_blk_last = this->currBB->parent2;
        } else {
            else_blk_last = else_blk;
        }
        

        for (const auto &p : vars) {
            int curr = p;

            // [11.25.2024]: modified to handle [func]'s ident if exists
            if (this->isFunc) {
                curr = this->generateFuncIdent();
            }
            #ifdef DEBUG
                std::cout << "current ident: [" << SymbolTable::symbol_table.at(curr) << "]" << std::endl;
            #endif
            if (then_blk_last->varVals.find(curr) == then_blk_last->varVals.end()) {
                BasicBlock *prevCurr = this->currBB;
                this->currBB = then_blk_last;
                this->handleUninitVar(curr);
                this->currBB = prevCurr;
            }

            if (else_blk_last->varVals.find(curr) == else_blk_last->varVals.end()) {
                BasicBlock *prevCurr = this->currBB;
                this->currBB = else_blk_last;
                this->handleUninitVar(curr);
                this->currBB = prevCurr;
            }
            
            // if a ident from [if_parent] exists in [then_blk] with a different value in [else_blk], we need a phi
            if (then_blk_last->varVals.at(curr) != else_blk_last->varVals.at(curr)) {
                SSA *then_phi = BasicBlock::ssa_table.at(then_blk_last->varVals.at(curr));
                SSA *else_phi = BasicBlock::ssa_table.at(else_blk_last->varVals.at(curr));
                
                #ifdef DEBUG
                    std::cout << "then_blk_last's ident val != else_blk_last's ident val; consolidating to one..." << std::endl;
                    std::cout << "then_phi: " << then_phi->toString() << std::endl;
                    std::cout << "else_phi: " << else_phi->toString() << std::endl;
                #endif
                
                SSA *phi_instr = nullptr;
                int phi_table_int;

                if (this->currBB->varVals.find(p) != this->currBB->varVals.end()) {
                    #ifdef DEBUG
                        std::cout << "conditionalStmtPhiUpdate's [currBB==join-blk], ident's prev Value is: [" << BasicBlock::ssa_table.at(this->currBB->varVals.at(p))->toString() << "]" << std::endl;
                    #endif
                    if (this->currBB->findSSA(BasicBlock::ssa_table.at(this->currBB->varVals.at(p)))) {
                        #ifdef DEBUG
                            std::cout << "[this->currBB]'s [newInstrs] contains the previous value!" << std::endl;
                        #endif
                        if (BasicBlock::ssa_table.at(this->currBB->varVals.at(p))->compare(6, BasicBlock::ssa_table.at(then_blk_last->varVals.at(p)), BasicBlock::ssa_table.at(else_blk_last->varVals.at(p)))) {
                            #ifdef DEBUG
                                std::cout << "\tthe previous value is the same as the value we were planning to add; no need to add or create new SSA!" << std::endl;
                            #endif
                            continue;
                        } else {
                            this->currBB->removeSSA(BasicBlock::ssa_table.at(this->currBB->varVals.at(p)));
                        }
                    }
                }

                phi_instr = this->addSSA1(6, BasicBlock::ssa_table.at(then_blk_last->varVals.at(p)), BasicBlock::ssa_table.at(else_blk_last->varVals.at(p)), true);
                phi_table_int = this->add_SSA_table(phi_instr);
                this->propagateDown(this->currBB, p, else_phi, phi_table_int, true);
                
                #ifdef DEBUG
                    std::cout << "phi_instr: " << phi_instr->toString() << std::endl;
                #endif
                
                // [10.28.2024]: Update BasicBlock's VV
                this->currBB->varVals.insert_or_assign(p, phi_table_int);
                this->VVs.insert_or_assign(p, phi_table_int);
                // #ifdef DEBUG
                //     std::cout << "currBB after update: " << this->currBB->toString() << std::endl;
                // #endif
            }
        }
    }

    static void printResultVec(std::vector<Result> r) { // [11.24.2024]: print result vector subset (for testing) in [funcDecl()]
        std::string resultStr = "";
        
        resultStr += "Result Vector Subset;\n[KIND]: \t\t\t\t[VALUE]\n";
        for (const Result &res : r) { 
            std::string kind = res.get_kind(), value = res.get_value();
            if ((kind).length() > 7) {
                resultStr += "[" + kind + "]" + ": \t\t\t[" + value + "]\n";
            } else if ((kind).length() > 3) {
                resultStr += "[" + kind + "]" + ": \t\t\t\t[" + value + "]\n";
            } else {
                resultStr += "[" + kind + "]" + ": \t\t\t\t\t[" + value + "]\n";
            }
        }
        
        std::cout << resultStr << std::endl;
    }

    static void printVVs(std::unordered_map<int, int> res) { // print varVals
        std::cout << "printing [varVals(size=" << res.size() << ")]:" << std::endl;
        for (const auto &p : res) {
            std::cout << "[" << p.first << ": " << SymbolTable::symbol_table.at(p.first) << "], [" << BasicBlock::ssa_table.at(p.second)->toString() << "]" << std::endl;
        }
        std::cout << std::endl;
    }

    void printPrevInstrs() const {
        std::stack<SSA *> temp = this->prevInstrs;
        
        if (temp.empty()) { std::cout << "\tempty!" << std::endl; }

        while (!temp.empty()) {
            std::cout << "\t" << temp.top()->toString() << std::endl;
            temp.pop();
        }
    }

    void printInstrList() const {
        std::cout << "[instrA]:\n\tinstrA1, instrA2, instrA3, ..." << std::endl;
        for (unsigned int i = 1; i < SymbolTable::operator_table.size() - 1; i++) {
            std::cout << "[" << SymbolTable::operator_table_reversed.at(i) << "]: " << std::endl << "\t";
            BasicBlock::instrList.at(i)->printList();
        } 

        #ifdef DEBUG
            std::cout << "done printing [this->instrList]" << std::endl;
        #endif
    }

    // [12.09.2024]: assume only called at the end
    std::string BBListToString(std::string res = "") {
        BasicBlock *oldCurr = this->currBB;
        std::string init = res;
        if (res == "") {
            this->blksSeen.clear();
            this->currBB = this->BB0;
            #ifdef DEBUG
                std::cout << "starting BB List toString();";
            #endif
        }
        #ifdef DEBUG
            std::cout << "[this->currBB[" << std::to_string(this->currBB->blockNum) << "]] looks like: " << std::endl << this->currBB->toString() << std::endl;
        #endif

        if (std::find(this->blksSeen.begin(), this->blksSeen.end(), this->currBB->blockNum) == this->blksSeen.end()) {
            #ifdef DEBUG
                std::cout << "blk has not been seen before; adding to [this->blksSeen]" << std::endl;
            #endif
            res += this->currBB->toString() + "\n\n";
            this->blksSeen.push_back(this->currBB->blockNum);
        } else {
            #ifdef DEBUG
                std::cout << "blk has already been added to [res]; returning pre-emptively" << std::endl;
            #endif
            return res;
        }

        BasicBlock *tmp = this->currBB;
        if (this->currBB->child) {
            #ifdef DEBUG
                std::cout << "currBB [" << std::to_string(this->currBB->blockNum) << "] has a child [" << std::to_string(this->currBB->child->blockNum) << "]" << std::endl;
            #endif
            this->currBB = this->currBB->child;
            res = this->BBListToString(res);
        }
        this->currBB = tmp;
        if (this->currBB->child2) {
            #ifdef DEBUG
                std::cout << "currBB [" << std::to_string(this->currBB->blockNum) << "] has a child2 [" << std::to_string(this->currBB->child2->blockNum) << "]" << std::endl;
            #endif
            this->currBB = this->currBB->child2;
            res = this->BBListToString(res);
        }

        this->currBB = oldCurr;
        #ifdef DEBUG
            if (init == "") {
                std::cout << "done BB List to String; res looks like: " << res << std::endl; 
            }
        #endif
        return res;
    }

    std::string instrListToString() const {
        std::string lst = "[instrA]:\n\tinstrA1, instrA2, instrA3, ...\n";
        
        lst += "[const]: \n\t" + this->BB0->constList->listToString();
        
        for (unsigned int i = 1; i < SymbolTable::operator_table.size() - 1; i++) {
            lst += "[" + SymbolTable::operator_table_reversed.at(i) + "]: \n" + BasicBlock::instrList.at(i)->listToString();
        }
        return lst;
    }

    int getInstrListSize() const {
        int size = 0;

        for (const auto &pair : BasicBlock::instrList) {
            // #ifdef DEBUG
            //     std::cout << "got op [" << pair.first << "] == " << SymbolTable::operator_table_reversed.at(pair.first) << "; with size=" << pair.second->length << std::endl;
            // #endif
            size += pair.second->length;
        }
        return size;
    }

    inline void printBlocks(BasicBlock *curr) {
        this->blksSeen.push_back(curr->blockNum);

        std::cout << curr->toString() << std::endl;
        if ((curr->child) && (std::find(this->blksSeen.begin(), this->blksSeen.end(), curr->child->blockNum) == this->blksSeen.end())) {
            this->printBlocks(curr->child);
        }
        if ((curr->child2) && (std::find(this->blksSeen.begin(), this->blksSeen.end(), curr->child2->blockNum) == this->blksSeen.end())) {
            this->printBlocks(curr->child2);
        }
    }

    // 11.11.2024: dfs to recursively generate blocks for DOT
    inline std::string generateBlocks(BasicBlock *curr, std::string res) {
        this->blksSeen.push_back(curr->blockNum);
        #ifdef DEBUG
            std::cout << "generateBlocks pushed_back: [" << curr->blockNum << "]" << std::endl;
            std::cout << curr->toString() << std::endl;
        #endif

        if (curr->blkType == 2) {
            res += "\tbb" + std::to_string(curr->blockNum) + " [shape=record, label=\"<b>join[" + std::to_string(curr->blkType) + "]\\n" + curr->toDOT() + "\"];\n";
        } else {
            res += "\tbb" + std::to_string(curr->blockNum) + " [shape=record, label=\"<b>[" + std::to_string(curr->blkType) + "]" + curr->toDOT() + "\"];\n";
        }
        // #ifdef DEBUG
        //     std::cout << "updated res: " << res << std::endl << std::endl;
        // #endif

        if ((curr->child) && (std::find(this->blksSeen.begin(), this->blksSeen.end(), curr->child->blockNum) == this->blksSeen.end())) {
            res = this->generateBlocks(curr->child, res);
        }
        if ((curr->child2) && (std::find(this->blksSeen.begin(), this->blksSeen.end(), curr->child2->blockNum) == this->blksSeen.end())) {
            res = this->generateBlocks(curr->child2, res);
        }

        return res;
    }

    inline std::string BBtoDOT() {
        std::string res = "digraph G {\n\trankdir=TB;\n\n";
        std::string c_res = "";

        // block definitions
        this->blksSeen.clear();
        res = this->generateBlocks(this->BB0, res);

        #ifdef DEBUG
            std::cout << "done creating block definitions res: " << res << std::endl;
        #endif

        // edge connections;
        // - label=[`branch`, `fall-through`, `dom`]
        //      note: `dom`    style=[dotted]
        //      note: `branch` iff condition == true (then_blk, while_blk)
        // NOTE: [branch && fall through] && [dom] imply 2 separate edge connection generations!
        BasicBlock *curr = nullptr;
        res += "\n\n";
        std::queue<BasicBlock *> tmp;
        std::vector<BasicBlock *> seen = {};

        // 11.07.2024: edge connections part 1 - [branch / fall-through]
        tmp.push(this->BB0);
        while (!tmp.empty()) {
            curr = tmp.front();
            tmp.pop();
            if (std::find(seen.begin(), seen.end(), curr) == seen.end()) {
                seen.push_back(curr);
            } else { // [10.27.2024]: only add if we haven't seen this [BasicBlock] before
                continue;
            }

            // [11.07.2024]: adding to prevent double arrows to same location
            if ((curr->child) && (curr->child2)) {
                if (curr->child->compare(curr->child2)) {
                    #ifdef DEBUG
                        std::cout << "set curr's child2 to nullptr bc dupe (child: " << curr->child->blockNum << ", child2: " << curr->child2->blockNum << "); curr: " << std::endl << curr->toString() << std::endl;
                    #endif
                    curr->child2 = nullptr;  // will this cause any probelms?
                }
            }

            if (curr->child) {
                res += "\tbb" + std::to_string(curr->blockNum) +  ":s -> bb" + std::to_string(curr->child->blockNum) + ":n [label=\"fall-through\"];\n";
                tmp.push(curr->child);
            }
            if (curr->child2) {
                res += "\tbb" + std::to_string(curr->blockNum) +  ":s -> bb" + std::to_string(curr->child2->blockNum) + ":n";

                if (curr->child) {
                    res += "[label=\"branch\"];\n";
                } else { // gets called when we have an if-statement without anything after it in a [statSeq]
                    res += "[label=\"fall-through\"];\n";
                }
                tmp.push(curr->child2);
            }
        }

        // [11.07.2024]: edge connections part 2 - [dom]
        seen.clear();
        tmp.push(this->BB0);
        while (!tmp.empty()) {
            curr = tmp.front();
            tmp.pop();
            if (std::find(seen.begin(), seen.end(), curr) == seen.end()) {
                seen.push_back(curr);
            } else { // [10.27.2024]: only add if we haven't seen this [BasicBlock] before
                continue;
            }

            // if we haven't seen the child before indicates this blk [dom] the child
            if (curr->child && (std::find(seen.begin(), seen.end(), curr->child) == seen.end())) {
                res += "\tbb" + std::to_string(curr->blockNum) +  ":b -> bb" + std::to_string(curr->child->blockNum) + ":b [color=blue, style=dotted, label=\"dom\"];\n";
                tmp.push(curr->child);
            }
            if (curr->child2 && (std::find(seen.begin(), seen.end(), curr->child2) == seen.end())) {
                res += "\tbb" + std::to_string(curr->blockNum) +  ":b -> bb" + std::to_string(curr->child2->blockNum) + ":b [color=blue, style=dotted, label=\"dom\"];\n";
                tmp.push(curr->child2);
            }
        }

        res += "}";

        #ifdef DEBUG
            std::cout << "done creating edge-connections defintiions; returning from 'BBtoDOT' res looks like: " << res << std::endl << std::endl;
        #endif

        return res;
    }

    inline void generateFuncDOTS() {
        // [11.26.2024]: todo - iterate through [funcMap]
        // - put all of this in a while loop and generate a new [dot] for each [func]
        // - this function will be called ONCE by the MAIN PARSER obj 
        // - and will loop through every [func] found in [funcMap]
        Func *f = nullptr;
        Parser *p = nullptr;
        
        for (const auto &fun : Parser::funcMap) {
            f = fun.first;
            p = fun.second;

            std::string fileName = "DOT";
            if (p->isFunc) {
                fileName += "_" + f->getName();
            }

            #ifdef DEBUG
                std::cout << "generating DOT for [" << fileName << "]" << std::endl;
            #endif

            // Open the file for writing
            std::ofstream file("res/" + fileName + ".dot");

            // Check if the file was opened successfully
            if (!file.is_open()) {
                std::cerr << "Failed to open the file." << std::endl;
                exit(EXIT_FAILURE); // Return false to indicate failure
            }

            #ifdef DEBUG
                std::cout << "file was opened successfullly" << std::endl;
            #endif
            
            // Get content
            std::string content = p->BBtoDOT();

            #ifdef DEBUG
                std::cout << "got content" << std::endl;
            #endif

            // Write content to the file
            file << content;

            // Close the file
            file.close();

            #ifdef DEBUG    
                std::cout << "file was closed successfully; done generating..." << std::endl;
            #endif
        }
    }

    inline void generateMainDOT() {
        #ifdef DEBUG
            std::cout << "generating DOT..." << std::endl;
        #endif

        // Open the file for writing
        std::ofstream file("res/DOT.dot");

        // Check if the file was opened successfully
        if (!file.is_open()) {
            std::cerr << "Failed to open the file." << std::endl;
            exit(EXIT_FAILURE); // Return false to indicate failure
        }

        #ifdef DEBUG
            std::cout << "file was opened successfullly" << std::endl;
        #endif

        // Get content
        std::string content = this->BBtoDOT();

        #ifdef DEBUG
            std::cout << "got content" << std::endl;
        #endif

        // Write content to the file
        file << content;

        // Close the file
        file.close();

        #ifdef DEBUG    
            std::cout << "file was closed successfully; done generating..." << std::endl;
        #endif
    }

    // [11.25.2024]: must ensure that [generateFuncIdent()] is only called when we KNOW that a func's ident is contained in [SymbolTable::identifiers]
    inline int generateFuncIdent() const {
        std::string res = "";

        if (this->isFunc) {
            res += this->func->getName() + "_" + this->sym.get_value();
        }
        return SymbolTable::identifiers.at(res);
    }

    inline Result getSym() const {
        return this->sym;
    }

    inline size_t getS_idx() const {
        return this->s_index;
    }

    static void clearFuncMap() {
        SymbolTable::clearFuncTable();

        for (auto &f : Parser::funcMap) {
            delete f.first;
            delete f.second;
        }
        // [11.26.2024]: will-this-work / is-this-necessary?
        Parser::funcMap.clear();
    }

    Parser* getFuncParser(Func *f) const {
        for (const auto &fc : Parser::funcMap) {
            if (fc.first->getName() == f->getName()) {
                return fc.second;
            }
        }
        return nullptr;
    }

    // [11.29.2024]: called after each func to check & add new CONST-SSA to main-BB0
    inline void updateConstBlk(Parser *p) {
        LinkedList *toAdd = p->BB0->constList;
        Node *curr = toAdd->tail;

        while (curr) {
            if (this->BB0->constList->contains(curr->instr) == nullptr) {
                this->BB0->constList->InsertAtTail(curr->instr);
            }
            curr = curr->prev;
        }
    }

    SSA* p_start();
    BasicBlock* parse(); // IR BB-representation 
    void parse_generate_SSA(); // generate all SSA Instructions
private:
    bool block;
    std::vector<int> blksSeen;

    Result sym;
    std::vector<Result> source;
    std::vector<SSA*> SSA_instrs; // [09/11/2024]: this is pretty much js for our testing/reference purposes now
    

    // [09/02/2024]: Do we need to optimize this? (i.e. <int, int> that is a table lookup probably in [SymbolTable]? Nope! in [Parser])
    // - Note: we probably should
    // std::unordered_map<std::string, SSA*> varVals; 
    std::unordered_map<int, int> VVs; // [key (int) = SymbolTable::symbol_table.at(key) (string)], [value (int) = BasicBlock::ssa_table.at(value) (string)]
    size_t s_index, source_len;

    // [09/02/2024]: Does this need to include the `const` SSA's? I don't think it should bc the const instr's only belong to BB0 (special)
    // current instruction list (copied for each BB)
    // [09/05/2024]: key=[SymbolTable::operator_table] values corresponding to specific SSA-instrs
    // std::unordered_map<int, LinkedList*> instrList; // [10/14/2024]: Should change this to <int, int> as well...
    
    BasicBlock *currBB;
    BasicBlock *BB0; // [09/02/2024]: special BB always generated at the start. Contains const SSA-instrs
    // BasicBlock *startBB; // first result obj generated from this file
    // BasicBlock *parentBB; // most recent BasicBlock (or 2nd most [if/while])

    bool prevJump;
    SSA *prevInstr;
    std::stack<SSA *>prevInstrs;

    SSA *SSA_start;
    SSA *SSA_parent; // copied from above's [BasicBlock]

    std::unordered_set<int> varDeclarations; // the int in the symbol table
    std::unordered_set<int> funcDeclarations; // the int in the symbol table

    bool isFunc;
    Func *func;

    SSA *p2_funcStart(Func *f); // [11.22.2024]: UDF funcBody
    void handle_getpar(Func *f, int paramNo);

    // [09/04/2024]: ToDo - transition into BasicBlocks
    SSA* p2_start();
    // void p2_varDecl();
    SSA* p2_statSeq();
    SSA* p2_statement(); 
    SSA* p2_assignment();
    SSA* p2_funcCall();
    SSA* p2_ifStatement();
    SSA* p2_whileStatement();
    SSA* p2_return();

    // SSA* p2_relation();
    // BasicBlock* p2_expr();
    // BasicBlock* p2_term();
    // BasicBlock* p2_factor();

    // Recursive Descent
    void p_varDecl();
    void p_funcDecl(bool retVal = false);
    void p_funcCallUDF(Parser *p);
    SSA* p_statSeq();
    SSA* p_statement(); 
    SSA* p_assignment();
    SSA* p_funcCall();
    SSA* p_ifStatement();
    SSA* p_whileStatement();
    SSA* p_return();

    SSA* p_relation();
    SSA* p_expr();
    SSA* p_term();
    SSA* p_factor();

    // peek & consume char
    inline void next() {
        // #ifdef DEBUG
        //     std::cout << "in [next(this->sym=" << this->sym.to_string_literal() << ", this->s_index=" << this->s_index << ")]" << std::endl;
        // #endif


        if (this->s_index < this->source_len) {
            this->sym = this->source.at(this->s_index++);
        } else {
            this->sym = Result(2, -1); // keyword: ["EOF": -1]
        }

        // #ifdef DEBUG
        //     std::cout << "\tmodified [this->sym] to: " << this->sym.to_string_literal() << "[" << this->sym.to_string() << "], this->s_index=" << this->s_index << std::endl;
        // #endif
    }

    // consumes [non-optional's]
    inline bool CheckFor(Result expected_token, bool optional = false) {
        #ifdef DEBUG
            std::cout << "Checking For [expected: " << expected_token.to_string() << "], [got: " << this->sym.to_string() << "]" << std::endl;
        #endif
        bool retVal = false;
        
        // Note: there might be some logic messed up abt this (relation: [expected_token, Result(2, -1)])
        if (!expected_token.compare(Result(2, -1))) { 
            // #ifdef DEBUG
            //     std::cout << "expected token != EOF token" << std::endl;
            // #endif
            if (optional) { // optional's don't get consumed
                // #ifdef DEBUG
                //     std::cout << "\toptional!" << std::endl;
                // #endif
                if (this->sym.compare(expected_token)) {
                    retVal = true;
                }
            } else {
                // #ifdef DEBUG
                //     std::cout << "\tnot an optional! (i.e. will consume if can)" << std::endl;
                // #endif
                if (!this->sym.compare(expected_token)) {
                    std::cout << "Error: CheckFor expected: [" << expected_token.to_string() << "], got: [" << this->sym.to_string() << "]! exiting prematurely..." << std::endl;
                    exit(EXIT_FAILURE);
                } else { 
                    this->next(); 
                }
                retVal = true;
            }
        }

        // #ifdef DEBUG
        //     std::cout << "\treturning [" << retVal << "]" << std::endl;
        // #endif
        return retVal;
    }

    // [09/03/2024]: Consumes if exists else does nothing
    inline void CheckFor_udf_optional_paren() {
        // optional parenthesis for functions without arguments
        if (this->CheckFor(Result(2, 13), true)) { // check for `(`
            next();
            this->CheckFor(Result(2, 14)); // check for `)`
        }
    }

    inline bool SSA_exists(SSA new_instr) const {
        SSA *instr = nullptr;


        if (new_instr.get_operator() == 0) { // const
            for (SSA* i : this->SSA_instrs) {
                instr = i;
                if (new_instr.compareConst(instr)) {
                    return true;
                }
            }
        } else {            
            for (SSA* i : this->SSA_instrs) {
                instr = i;
                if (new_instr.compare(instr)) {
                    return true;
                }
            }
        }
        return false;
    }


};

#endif