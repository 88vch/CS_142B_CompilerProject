#ifndef PARSER_HPP
#define PARSER_HPP

#include <stack>
#include <queue>
#include <vector>
#include <sstream>
#include <cstring>
#include <fstream>
// #include <stdio.h>
// #include <cstdio>
#include "Func.hpp"
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
    Parser(const std::vector<Result> &tkns, bool blk = false)
    {
        this->source = tkns;

        this->SSA_start = nullptr;
        this->SSA_parent = nullptr;

        this->s_index = 0;
        this->source_len = tkns.size();
        this->SSA_instrs = {};
        // this->varVals = {};
        this->VVs = {};
        
        this->currBB = nullptr;
        // this->BB0 = new BasicBlock(this->instrList, true);
        this->BB0 = new BasicBlock(true);
        // this->startBB = nullptr;
        // this->parentBB = this->BB0;

        for (unsigned int i = 1; i < SymbolTable::operator_table.size(); i++) {
            this->instrList.insert({i, new LinkedList()});
        }

        this->prevInstrs = std::stack<SSA *>();
        this->prevJump = false;
        this->prevInstr = nullptr;

        this->block = blk;
        // this->blocksSeen = {};

        next(); // [10/14/2024]: Why do we do this?
        // - load's first token into [this->sym]
    }

    ~Parser() {
        #ifdef DEBUG
            std::cout << "Start ~Parser()" << std::endl;
        #endif
        delete this->BB0;
        
        for (unsigned int i = 1; i < this->instrList.size(); i++) {
            // #ifdef DEBUG
            //     std::cout << "deleting linked list [" << SymbolTable::operator_table_reversed.at(i) << "];" << std::endl;
            // #endif
            delete this->instrList.at(i);
            this->instrList[i] = nullptr;
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
        // #ifdef DEBUG
        //     std::cout << "\t[Parser::CheckExistence(op=[" << op << "], x=" << x->toString() << ",y=" << y->toString() << "), this->SSA_instrs.size()=" << this->SSA_instrs.size() << "]" << std::endl;
        // #endif
        SSA *ret = nullptr;
        
        if (this->instrList.find(op) == this->instrList.end()) {
            return nullptr;
        }

        // [09/19/2024]: Replaced Below (previous=this->SSA_instrs; current=this->instrList)
        Node *curr = this->instrList.at(op)->tail;
        while (curr) {
            // #ifdef DEBUG
            //     std::cout << "\t\tcomparing instr=[" << curr->instr->toString() << "]" << std::endl;
            // #endif
            if (curr->instr->compare(op, x, y)) {
                // #ifdef DEBUG
                //     std::cout << "\t\tgot true!!!" << std::endl;
                // #endif
                ret = curr->instr;
                break;
            } else {
                // #ifdef DEBUG
                //     std::cout << "\t\tgot false" << std::endl;
                // #endif
            }
            curr = curr->prev;
        }

        // #ifdef DEBUG
        //     if (ret != nullptr) {
        //         std::cout << "\t\treturning: [" << ret->toString() << "]" << std::endl;
        //     } else { 
        //         std::cout << "\t\treturning: nullptr!" <<std::endl;
        //     }
        // #endif
        return ret;
    }

    // [09/20/2024]: Const is not found in instrList, rather in BB0
    // [08/22/2024]: might need to revise (like CheckExistence()) to include op and curr sym's values
    inline SSA* CheckConstExistence(int val = -1) const {
        #ifdef DEBUG
            std::cout << "in Parser::CheckConstExistence(val=" << val << ")" << std::endl;
        #endif
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
            #ifdef DEBUG
                std::cout << "\tthis instr: [" << curr->instr->toString() << "]" << std::endl;
            #endif
            if (curr->instr->compareConst(comparisonVal)) {
                ret = curr->instr;
                #ifdef DEBUG
                    std::cout << "ret is now: " << ret->toString() << std::endl;
                #endif
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
            
            this->prevInstr->set_operand2(instr);

            // this->prevJump = false;
            this->prevInstr = nullptr;
            this->prevInstrs.pop();

            // [10.24.2024]: trying something out...
            if (this->prevInstrs.empty()) {
                this->prevJump = false;
            }
        }
    }

    inline SSA* addSSA1(int op, SSA *x = nullptr, SSA *y = nullptr, bool check = false) {
        // [10.23.2024]: Assume addSSA1() handles deletion properly...
        SSA *tmp = new SSA(op, x, y);
        SSA *res = this->addSSA1(tmp, check);
        
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
            #ifdef DEBUG
                std::cout << "\tinstr is const" << std::endl;
            #endif
            if (check) {
                SSA *tmp = this->CheckConstExistence(*(instr->get_constVal())); 
                if (tmp == nullptr) {
                    instr = this->addSSA(instr);
                } else {
                    #ifdef DEBUG
                        std::cout << "instr(" << instr->toString() << ") exists already: " << tmp->toString() << std::endl;
                    #endif
                    // [10.23.2024]: CAN WE DO THIS???
                    delete instr;
                    instr = tmp;
                }
            } else {
                #ifdef DEBUG
                    std::cout << "not checking! instr=" << instr->toString() << std::endl;
                #endif
                instr = this->addSSA(instr);
            }
        } else {
            #ifdef DEBUG
                std::cout << "\tinstr is NOT const" << std::endl;
            #endif
            if (check) {
                SSA *tmp = this->CheckExistence(instr->get_operator(), instr->get_operand1(), instr->get_operand2());
                if (tmp == nullptr) {
                    instr = this->addSSA(instr);
                } else {
                    #ifdef DEBUG
                        std::cout << "instr(" << instr->toString() << ") exists already: " << tmp->toString() << std::endl;
                    #endif
                    // [10.23.2024]: CAN WE DO THIS???
                    delete instr;
                    instr = tmp;

                    #ifdef DEBUG
                        std::cout << "instr is now: " << instr->toString() << std::endl;
                    #endif
                }
            } else {
                #ifdef DEBUG
                    std::cout << "not checking! instr=" << instr->toString() << std::endl;
                #endif
                instr = this->addSSA(instr);
            }
        }

        #ifdef DEBUG
            std::cout << "returning this->addSSA1() instr=" << instr->toString() << std::endl;
        #endif
        return instr;
    }

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
                this->BB0->constList->InsertAtTail(instr);
            }
        } else {
            #ifdef DEBUG
                std::cout << "\tinstr op == " << instr->get_operator() << std::endl;
            #endif
            
            if (this->block) {
                #ifdef DEBUG
                    if (this->currBB) {
                        std::cout << "currBB looks like: " << this->currBB->toString() << std::endl;
                    } else {
                        std::cout << "currBB looks like: nullptr!" << std::endl;
                    }
                #endif
                // [10.23.2024]: [LinkedList::InsertAtTail()] returns [Node*]
                this->currBB->newInstrs.push_back(this->instrList.at(instr->get_operator())->InsertAtTail(instr));
                #ifdef DEBUG
                    std::cout << "done pushing into [currBB]'s [newInstrs]" << std::endl;
                #endif
            } else {
                this->instrList.at(instr->get_operator())->InsertAtTail(instr);
            }
        }

        // [10.20.2024]: Modification
        this->checkPrevJmp(instr);

        // this->currBB->setInstructionList(this->instrList);

        this->SSA_instrs.push_back(instr);
        return instr;
    }

    // [10/09/2024]: ToDo - should check if [SSA] already exists in [this->ssa_table] 
    // - specifically for [this->VVs]
    inline int add_SSA_table(SSA *toAdd) {
        ssa_table.insert(std::pair<int, SSA *>(ssa_table.size(), toAdd));
        ssa_table_reversed.insert(std::pair<SSA *, int>(toAdd, ssa_table.size() - 1));

        return ssa_table.size() - 1; // [09/30/2024]: return the int that is associated with SSA-instr
    }

    inline std::vector<SSA*> getSSA() const { return this->SSA_instrs; }

    // [09/30/2024]: Converts to original varVal mapping
    std::unordered_map<std::string, SSA*> getVarVal() const { 
        std::unordered_map<std::string, SSA *> res = {};

        for (const auto pair : this->VVs) {
            res.insert(std::pair<std::string, SSA *>(SymbolTable::symbol_table.at(pair.first), this->ssa_table.at(pair.second)));
        }
        
        return res; 
    }

    void printVVs() { // print varVals
        std::unordered_map<std::string, SSA *> res = this->getVarVal();
        
        std::cout << "printing [this->varVals(size=" << res.size() << ")]:" << std::endl;
        for (const auto &p : res) {
            std::cout << "[" << p.first << "], [" << p.second->toString() << "]" << std::endl;
        }
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
            this->instrList.at(i)->printList();
        } 

        #ifdef DEBUG
            std::cout << "done printing [this->instrList]" << std::endl;
        #endif
    }

    std::string instrListToString() const {
        std::string lst = "[instrA]:\n\tinstrA1, instrA2, instrA3, ...\n";
        
        lst += "[const]: \n\t" + this->BB0->constList->listToString();
        
        for (unsigned int i = 1; i < SymbolTable::operator_table.size() - 1; i++) {
            lst += "[" + SymbolTable::operator_table_reversed.at(i) + "]: \n\t" + this->instrList.at(i)->listToString();
        }
        return lst;
    }

    int getInstrListSize() const {
        int size = 0;

        for (const auto &pair : this->instrList) {
            // #ifdef DEBUG
            //     std::cout << "got op [" << pair.first << "] == " << SymbolTable::operator_table_reversed.at(pair.first) << "; with size=" << pair.second->length << std::endl;
            // #endif
            size += pair.second->length;
        }
        return size;
    }

    inline void printBlocks(BasicBlock *curr, std::vector<BasicBlock *>blocksSeen) {
        blocksSeen.push_back(curr);

        std::cout << curr->toString() << std::endl;
        if ((curr->child) && (std::find(blocksSeen.begin(), blocksSeen.end(), curr->child) == blocksSeen.end())) {
            this->printBlocks(curr->child, blocksSeen);
        }
        if ((curr->child2) && (std::find(blocksSeen.begin(), blocksSeen.end(), curr->child2) == blocksSeen.end())) {
            this->printBlocks(curr->child2, blocksSeen);
        }
    }

    inline std::string generateBlocks(BasicBlock *curr, std::string res, std::vector<BasicBlock *>blocksSeen) {
        blocksSeen.push_back(curr);

        res += "\tbb" + std::to_string(curr->blockNum) + " [shape=record, label=\"" + curr->toDOT() + "\"];\n";
        #ifdef DEBUG
            std::cout << "updated res: " << res << std::endl << std::endl;
        #endif

        if ((curr->child) && (std::find(blocksSeen.begin(), blocksSeen.end(), curr->child) == blocksSeen.end())) {
            res = this->generateBlocks(curr->child, res, blocksSeen);
        }
        if ((curr->child2) && (std::find(blocksSeen.begin(), blocksSeen.end(), curr->child2) == blocksSeen.end())) {
            res = this->generateBlocks(curr->child2, res, blocksSeen);
        }

        return res;
    } 

    inline std::string BBtoDOT() {
        std::string res = "digraph G {\n\trankdir=TB;\n\n";
        std::string c_res = "";

        // block definitions
        // for (BasicBlock *blk : this->blocksSeen) {
        //     blk = nullptr;
        // }
        std::vector<BasicBlock *> blks = {};
        #ifdef DEBUG
            std::cout << "entering [generateBlocks]..." << std::endl;
        #endif
        res = this->generateBlocks(this->BB0, res, blks);

        // BasicBlock *curr = this->BB0;

        // while (curr != nullptr) {
        //     res += "\tbb" + std::to_string(curr->blockNum) + " [shape=record, label=\"" + curr->toDOT() + "\"];\n";
        //     curr = curr->child;
        // }

        #ifdef DEBUG
            std::cout << "done creating block definitions res: " << res << std::endl;
        #endif

        // edge connections
        BasicBlock *curr = nullptr;
        res += "\n\n";
        std::queue<BasicBlock *> tmp;
        std::vector<BasicBlock *> seen = {};
        tmp.push(this->BB0);

        while (!tmp.empty()) {
            curr = tmp.front();
            if (std::find(seen.begin(), seen.end(), curr) == seen.end()) {
                seen.push_back(curr);
            } else { // [10.27.2024]: only add if we haven't seen this [BasicBlock] before
                tmp.pop();
                continue;
            }

            if (curr->child) {
                res += "\tbb" + std::to_string(curr->blockNum) +  "-> bb" + std::to_string(curr->child->blockNum) + ";\n";
                tmp.push(curr->child);
            }
            if (curr->child2) {
                res += "\tbb" + std::to_string(curr->blockNum) +  "-> bb" + std::to_string(curr->child2->blockNum) + ";\n";
                tmp.push(curr->child2);
            }
            tmp.pop();
        }

        res += "}";

        #ifdef DEBUG
            std::cout << "done creating edge-connections defintiions; returning from 'BBtoDOT'" << std::endl;
        #endif

        return res;
    }

    inline void generateDOT() {
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
            std::cout << "done generating..." << std::endl;
        #endif
    }

    SSA* p_start();
    BasicBlock* parse(); // IR BB-representation 
    void parse_generate_SSA(); // generate all SSA Instructions
private:
    bool block;

    Result sym;
    std::vector<Result> source;
    std::vector<SSA*> SSA_instrs; // [09/11/2024]: this is pretty much js for our testing/reference purposes now
    
    // [09/30/2024]: Though this may be for a legitimate reason
    std::unordered_map<int, SSA *> ssa_table = {}; // key=[value's in this->VVs] : value=SSA-corresponding to int-val
    std::unordered_map<SSA *, int> ssa_table_reversed = {};

    // [09/02/2024]: Do we need to optimize this? (i.e. <int, int> that is a table lookup probably in [SymbolTable]? Nope! in [Parser])
    // - Note: we probably should
    // std::unordered_map<std::string, SSA*> varVals; 
    std::unordered_map<int, int> VVs; // [key (int) = SymbolTable::symbol_table.at(key) (string)], [value (int) = Parser::ssa_table.at(value) (string)]
    size_t s_index, source_len;

    // [09/02/2024]: Does this need to include the `const` SSA's? I don't think it should bc the const instr's only belong to BB0 (special)
    // current instruction list (copied for each BB)
    // [09/05/2024]: key=[SymbolTable::operator_table] values corresponding to specific SSA-instrs
    std::unordered_map<int, LinkedList*> instrList; // [10/14/2024]: Should change this to <int, int> as well...
    
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

    // [09/04/2024]: ToDo - transition into BasicBlocks
    SSA* p2_start();
    void p2_varDecl();
    SSA* p2_statSeq();
    SSA* p2_statement(); 
    SSA* p2_assignment();
    SSA* p2_funcCall();
    SSA* p2_ifStatement();
    SSA* p2_whileStatement();
    SSA* p2_return();

    SSA* p2_relation();
    BasicBlock* p2_expr();
    BasicBlock* p2_term();
    BasicBlock* p2_factor();

    // Recursive Descent
    void p_varDecl();
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
        #ifdef DEBUG
            std::cout << "in [next(this->sym=" << this->sym.to_string_literal() << ", this->s_index=" << this->s_index << ")]" << std::endl;
        #endif


        if (this->s_index < this->source_len) {
            this->sym = this->source.at(this->s_index++);
        } else {
            this->sym = Result(2, -1); // keyword: ["EOF": -1]
        }

        #ifdef DEBUG
            std::cout << "\tmodified [this->sym] to: " << this->sym.to_string_literal() << ", this->s_index=" << this->s_index << std::endl;
        #endif
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
                #ifdef DEBUG
                    std::cout << "\toptional!" << std::endl;
                #endif
                if (this->sym.compare(expected_token)) {
                    retVal = true;
                }
            } else {
                #ifdef DEBUG
                    std::cout << "\tnot an optional! (i.e. will consume if can)" << std::endl;
                #endif
                if (!this->sym.compare(expected_token)) {
                    std::cout << "Error: CheckFor expected: [" << expected_token.to_string() << "], got: [" << this->sym.to_string() << "]! exiting prematurely..." << std::endl;
                    exit(EXIT_FAILURE);
                } else { 
                    this->next(); 
                }
                retVal = true;
            }
        }

        #ifdef DEBUG
            std::cout << "\treturning [" << retVal << "]" << std::endl;
        #endif
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