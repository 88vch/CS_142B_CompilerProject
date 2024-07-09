#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>
#include "Result.hpp"
#include "BasicBlock.hpp"

// first:   pointer to the start Result of this AST
// second:  IR through BasicBlocks

// Note: all nodes will be inherited from BasicBlock
// Note: shouldn't need keywords bc we only care about computational code
class Parser {
public:
    Parser(const std::vector<int> &tkns)
        :source(std::move(tkns)) 
    {
        this->start = nullptr;
        this->source_len = tkns.size();
    }

    // peek & consume char
    inline void next() {
        if (this->s_index < this->source_len) {
            this->sym = this->source.at(this->s_index);
            this->s_index++;
        } else {
            this->sym = SymbolTable::keywords.at("EOF"); // what value to denote end???
        }
    }   
    
    Res::Result parse_first(); // AST generation
    // BasicBlock parse_second(); // IR BB-representation
private:
    int sym;
    size_t source_len, s_index = 0;
    std::vector<int> source;
    Res::Result *start; // first result obj generated from this file
};

#endif