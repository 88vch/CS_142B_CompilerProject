#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>
#include "Result.hpp"
#include "BasicBlock.hpp"
#include "SSA.hpp"
#include "LinkedList.hpp"

// parse(): IR through BasicBlocks

// Note: all nodes will be inherited from BasicBlock
// Note: shouldn't need keywords bc we only care about computational code
class Parser {
public:
    Parser(const std::vector<Res::Result> &tkns)
        :source(std::move(tkns)) 
    {
        this->start = nullptr;
        this->source_len = tkns.size();
        this->parent = nullptr;
        this->SSA_instrs = {};
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

    void generate_SSA(); // generate all SSA Instructions
    BasicBlock parse(); // IR BB-representation
private:
    Res::Result sym;
    std::vector<Res::Result> source;
    std::vector<SSA> SSA_instrs;
    size_t source_len, s_index = 0;
    std::unordered_map<int, LinkedList> instrList; // current instruction list (copied for each BB)
    BasicBlock *start; // first result obj generated from this file
    BasicBlock *parent; // most recent BasicBlock (or 2nd most [if/while])
};

#endif