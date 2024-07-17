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

    bool CheckFor(Res::Result expected_token, bool optional = false) {
        if (expected_token == Res::Result(2, -1)) { return false; }

        if (optional) {
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
};

#endif