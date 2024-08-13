#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>
#include <sstream>
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
    Parser(const std::vector<Result> &tkns)
        :source(std::move(tkns)) 
    {
        this->start = nullptr;
        this->parent = nullptr;

        this->s_index = 0;
        this->source_len = tkns.size();
        this->SSA_instrs = {};
        this->varVals = {};
        
        next();
    }

    // [07/25/2024]: ToDo
    inline int getOp() const {
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
    }


    inline SSA* CheckExistence(SSA *x, SSA *y) const {
        SSA *ret = nullptr;
        const int op = this->getOp();
        for (SSA* instr : this->SSA_instrs) {
            if (instr->compare(op, x, y)) {
                ret = instr;
                break;
            }
        }
        return ret;
    }

    inline SSA* CheckConstExistence() const {
        #ifdef DEBUG
            std::cout << "\tcomparing value: [" << this->sym.get_value_literal() << "]" << std::endl;
        #endif
        SSA *ret = nullptr;
        for (SSA* instr : this->SSA_instrs) {
            #ifdef DEBUG
                std::cout << "\tthis instr: [" << instr->toString() << "]" << std::endl;
            #endif
            if (instr->compareConst(this->sym.get_value_literal())) {
                ret = instr;
                break;
            }
        }
        return ret;
    }

    // [07/25/2024]: ToDo
    inline SSA* BuildIR(int op, SSA *x, SSA *y) {
        #ifdef DEBUG
            std::cout << "\t[Parser::BuildIR(this->sym=" << this->sym.to_string() << ")]: SSA x = " << x->toString() << ", SSA y = " << y->toString() << std::endl;
        #endif
        SSA *ret = CheckExistence(x, y); // [08/12/2024]: Note we might have to include the `op` here to check diff types (i.e. `+` vs `*`)
        // if we don't get [nullptr] from CheckExistence(), that implies there was a previous SSA of the exact same type; so rather than creating a dupe, we can simply use the previously existing one (Copy Propagation)
        //      Note: check that all previous instructions will DOM this SSA instr
        if (ret) { return ret; } 

        // assume op represents the keyword on the [SymbolTable::symbol_table]
        switch (op) {
                case 0: // `+`
                    *ret = SSA(SymbolTable::operator_table.at("add"), x, y);
                    break;
                case 1: // `-`
                    *ret = SSA(SymbolTable::operator_table.at("sub"), x, y);
                    break;
                case 2: // `*`
                    *ret = SSA(SymbolTable::operator_table.at("mul"), x, y);
                    break;
                case 3: // `/`
                    *ret = SSA(SymbolTable::operator_table.at("div"), x, y);
                    break;
            }
        // switch (this->sym.get_kind_literal()) {
        //     case 0: // const
        //         break;
        //     case 1: // ident
        //         break;
        //     case 2: // keyword
        //         switch (this->sym.get_value_literal()) {
        //             case 0: // `+`
        //                 *ret = SSA(SymbolTable::operator_table.at("add"), x, y);
        //                 break;
        //             case 1: // `-`
        //                 *ret = SSA(SymbolTable::operator_table.at("sub"), x, y);
        //                 break;
        //             case 2: // `*`
        //                 *ret = SSA(SymbolTable::operator_table.at("mul"), x, y);
        //                 break;
        //             case 3: // `/`
        //                 *ret = SSA(SymbolTable::operator_table.at("div"), x, y);
        //                 break;
        //         }
        //         break;
        // }
        this->SSA_instrs.push_back(ret);
        return ret;
    }

    std::vector<SSA*> getSSA() const { return this->SSA_instrs; }

    std::unordered_map<std::string, SSA*> getVarVal() const { return this->varVals; }

    BasicBlock parse(); // IR BB-representation
    void parse_generate_SSA(); // generate all SSA Instructions
private:
    Result sym;
    std::vector<Result> source;
    std::vector<SSA*> SSA_instrs;
    std::unordered_map<std::string, SSA*> varVals;
    size_t s_index, source_len;
    std::unordered_map<int, LinkedList> instrList; // current instruction list (copied for each BB)
    
    // BasicBlock *start; // first result obj generated from this file
    // BasicBlock *parent; // most recent BasicBlock (or 2nd most [if/while])

    SSA *start;
    SSA *parent; // copied from above's [BasicBlock]

    std::unordered_set<int> varDeclarations; // the int in the symbol table
    std::unordered_set<int> funcDeclarations; // the int in the symbol table

    // Recursive Descent
    SSA* p_start();
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
            std::cout << "in [next(this->sym=" << this->sym.to_string_literal() << ")]" << std::endl;
        #endif


        if (this->s_index < this->source_len) {
            this->sym = this->source.at(this->s_index++);
        } else {
            this->sym = Result(2, -1); // keyword: ["EOF": -1]
        }

        #ifdef DEBUG
            std::cout << "\tmodified [this->sym] to: " << this->sym.to_string_literal() << std::endl;
        #endif
    }

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