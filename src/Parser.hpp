#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>
#include <sstream>
#include "Result.hpp"
#include "BasicBlock.hpp"
#include "SSA.hpp"
#include "LinkedList.hpp"
#include "TinyExceptions.hpp"

// parse(): IR through BasicBlocks

// Note: all nodes will be inherited from BasicBlock
// Note: shouldn't need keywords bc we only care about computational code
class Parser {
public:
    Parser(const std::vector<Res::Result> &tkns)
        :source(std::move(tkns)) 
    {
        this->start = nullptr;
        this->s_index = 0;
        this->source_len = tkns.size();
        this->parent = nullptr;
        this->SSA_instrs = {};
        this->sym = this->source .at(this->s_index);
    }

    // peek & consume char
    inline void next() {
        if (this->s_index < this->source_len) {
            this->sym = this->source.at(this->s_index);
            this->s_index++;
        } else {
            this->sym = Res::Result(2, -1); // keyword: ["EOF": -1]
        }
    }

    BasicBlock parse(); // IR BB-representation
    void parse_generate_SSA(); // generate all SSA Instructions
private:
    Res::Result sym;
    std::vector<Res::Result> source;
    std::vector<SSA> SSA_instrs;
    size_t source_len, s_index;
    std::unordered_map<int, LinkedList> instrList; // current instruction list (copied for each BB)
    BasicBlock *start; // first result obj generated from this file
    BasicBlock *parent; // most recent BasicBlock (or 2nd most [if/while])

    std::unordered_set<int> varDeclarations; // the int in the symbol table
    std::unordered_set<int> funcDeclarations; // the int in the symbol table

    // Recursive Descent
    void p_start();
    void p_varDecl();
    void p_statSeq();
    void p_statement(); 
    void p_assignment();
    void p_funcCall();
    void p_ifStatement();
    void p_whileStatement();
    void p_return();

    void p_relation();
    Res::Result p_expr();
    Res::Result p_term();
    Res::Result p_factor();

    bool CheckFor(Res::Result expected_token, bool optional = false) {
        if (expected_token == Res::Result(2, -1)) { return false; }

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
        for (SSA instr : this->SSA_instrs) {
            if (SSA::compare(instr, new_instr)) {
                return true;
            }
        }
        return false;
    }
};

#endif