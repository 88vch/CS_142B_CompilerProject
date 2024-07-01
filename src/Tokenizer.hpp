#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP


#include <iostream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "Token.hpp"
#include "SymbolTable.hpp"
#undef DEBUG
#undef DEBUG_RESULTS

// class SymbolTable; // forward declaration (?)

class Tokenizer {
public:
    Tokenizer(std::string in) 
        // : source(std::move(in))
    {
        this->tokens = {};
        this->s_index = 0;
        this->source = std::move(in);
        this->source_len = this->source.length();
        #ifdef DEBUG
            std::cout << "[Tokenizer]: Read input. Got: \n" << this->source << std::endl;
        #endif
        this->source_start = this->source.data();
        next(); // load first char into [sym]      
        #ifdef DEBUG
            std::cout << "\tnext() turned [this->sym] into val [" << this->sym << "]" << std::endl;  
        #endif
    }
    std::vector<int> tokenize();

    int get_eof_val() const noexcept { return SymbolTable::symbol_table.find(".")->second; }
    
    size_t source_len, s_index;
    std::string buff;
    std::vector<int> tokens;
    std::string source;
    char *source_start, sym;
    int token, keyword_identifier_separator = 30; // if number:: val; if ident::SymbolTable::symbol_table id 
private:
    int GetNext();
    void number();
    void identkeyword();
    std::unordered_set<char> whitespace_symbols = {' ', '\n', '\0'};

    // DOES consume char
    inline void next() {
        if (this->s_index < this->source_len) {
            this->sym = *(this->source_start + s_index);
            this->s_index++;
        } else {
            #ifdef DEBUG
                std::cout << "\tindex exceeds source_len!!! exit(EXIT_FAILURE)" << std::endl;
            #endif
            this->sym = '\0';
        }
    }
};



#endif