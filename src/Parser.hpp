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
        
        this->sym = this->source .at(this->s_index);
    }

    // peek & consume char
    inline void next() {
        if (this->s_index < this->source_len) {
            this->sym = this->source.at(this->s_index);
            this->s_index++;
        } else {
            this->sym = Result(2, -1); // keyword: ["EOF": -1]
        }
    }

    // [07/25/2024]: ToDo
    int getOp() const {
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


    SSA* CheckExistence(SSA *x, SSA *y) const {
        SSA *ret = nullptr;
        const int op = this->getOp();
        for (SSA instr : this->SSA_instrs) {
            if (instr.compare(op, x, y)) {
                *ret = instr;
                break;
            }
        }
        return ret;
    }

    SSA* CheckConstExistence(int opnd) const {
        SSA *ret = nullptr;
        const int op = this->getOp();
        for (SSA instr : this->SSA_instrs) {
            if (instr.compareConst(op, opnd)) {
                *ret = instr;
                break;
            }
        }
        return ret;
    }

    // [07/25/2024]: ToDo
    SSA* BuildIR(SSA *x, SSA *y) {
        SSA *ret = CheckExistence(x, y);
        // if we don't get [nullptr] from CheckExistence(), that implies there was a previous SSA of the exact same type; so rather than creating a dupe, we can simply use the previously existing one (Copy Propagation)
        //      Note: check that all previous instructions will DOM this SSA instr
        if (ret) { return ret; } 


        switch (this->sym.get_kind_literal()) {
            case 0: // const
                break;
            case 1: // ident
                break;
            case 2: // keyword
                switch (this->sym.get_value_literal()) {
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
                break;
        }
        this->SSA_instrs.push_back(*ret);
        return ret;
    }

    std::vector<SSA> getSSA() const { return this->SSA_instrs; }

    BasicBlock parse(); // IR BB-representation
    void parse_generate_SSA(); // generate all SSA Instructions
private:
    Result sym;
    std::vector<Result> source;
    std::vector<SSA> SSA_instrs;
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

    bool CheckFor(Result expected_token, bool optional = false) {
        if (expected_token == Result(2, -1)) { return false; }

        if (optional) { // optional's don't get consumed
            if (this->sym != expected_token) {
                return false;
            }
            return true;
        } else {
            if (this->sym != expected_token) {
                std::stringstream ss;
                ss << "Expected: " << expected_token.to_string() << ". Got: " << this->sym.to_string() << "." << std::endl;
                tinyExceptions_ns::SyntaxError(ss.str());
            } else { next(); }
            return true;
        }
    }

    bool SSA_exists(SSA new_instr) const {
        SSA *instr = nullptr;


        if (new_instr.get_operator() == 0) { // const
            for (SSA i : this->SSA_instrs) {
                *instr = i;
                if (new_instr.compareConst(instr)) {
                    return true;
                }
            }
        } else {            
            for (SSA i : this->SSA_instrs) {
                *instr = i;
                if (new_instr.compare(instr)) {
                    return true;
                }
            }
        }
        return false;
    }
};

#endif