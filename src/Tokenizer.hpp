#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP


#include <iostream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "Token.hpp"
#define DEBUG
#define DEBUG_RESULTS


// takes chars, turns to keywords & ident & digits
class Tokenizer {
public:
    Tokenizer(std::string in) 
        // : source(std::move(in))
    {
        this->tokens = {};
        this->s_index = 0;
        // this->curr = this->source.at(this->s_index++);
        // this->source = in.c_str();
        this->source = std::move(in);
        // this->source_len = this->source.length();
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

    std::unordered_map<std::string, int> get_sym_table() { return this->sym_table; }

    int get_eof_val() const noexcept { return this->sym_table.find(".")->second; }
    
    size_t source_len, s_index;
    // const std::string source;
    std::string buff;
    std::vector<int> tokens;
    std::string source;
    char *source_start, sym;
    int token, keyword_identifier_separator = 30; // if number:: val; if ident::sym_table id 
    // this->curr_sym_table_index = 31; [can just be size - 1]
    // const int eof_val = static_cast<const int&>(this->sym_table.find("eof")->second);
private:
    int GetNext();
    void number();
    void identkeyword();
    std::unordered_map<std::string, int> sym_table = {
        {"EOF", -1},
        {"+", 0},
        {"-", 1},
        {"*", 2},
        {"/", 3},
        {";", 4},
        {"var", 5}, 
        {"let", 6}, 
        {"<-", 7}, 
        {">", 8}, 
        {"<", 9},  
        {"==", 10}, 
        {">=", 11}, 
        {"<=", 12}, 
        {"(", 13}, 
        {")", 14},
        {"{", 15}, 
        {"}", 16},  
        {",", 17}, 
        {"if", 18}, 
        {"then", 19}, 
        {"else", 20}, 
        {"fi", 21},
        {"while", 22}, 
        {"do", 23}, 
        {"od", 24}, 
        {"call", 25}, 
        {"function", 26},
        {"void", 27}, 
        {"return", 28}, 
        {"main", 29},
        {".", 30}
    };
    std::unordered_set<char> whitespace_symbols = {' ', '\n', '\0'};

    // DOES consume char
    inline void next() {
        // #ifdef DEBUG
        //     std::cout << "in next(this->s_index=" << this->s_index << ", this->source_len=" << this->source_len << ")" << std::endl;
        // #endif
        if (this->s_index < this->source_len) {
            // const char *ret = &(this->source[this->s_index]);
            // this->sym = const_cast<char *>(&(this->source[this->s_index]));
            this->sym = *(this->source_start + s_index);
            this->s_index++;
        } else {
            #ifdef DEBUG
                std::cout << "\tindex exceeds source_len!!! exit(EXIT_FAILURE)" << std::endl;
            #endif
            this->sym = '\0';
            // exit(EXIT_FAILURE);
        }
    }
    // inline char consume() noexcept { return this->source[this->s_index++]; }




    // OLD STUFF [Lexer.hpp]
    // take a look, but don't advance the counter
    // std::string peek_deterministic_token(int characters) {
    //     std::string str = "";
    //     size_t curr = s_index;
    //     for (int i = 0; i < characters; i++) {
    //         str += source.at(curr);
    //         curr++;
    //         if (curr >= source_len) { return ""; }
    //     }
    //     return str;
    // }

    // // simply returns the next [characters] chars or [""] if EOF
    // std::string get_deterministic_token(int characters) {
    //     char *c;
    //     for (int i = 0; i < characters; i++) {
    //         if (((c = next()) != nullptr) && (std::isalpha(*(c = next())))) { this->buff.push_back(std::tolower(consume())); } 
    //         else if ((c = next()) == nullptr) { return ""; }
    //     }
    //     std::string res = this->buff;
    //     this->buff.clear();
    //     return res;
    // }

    // // return on whitespace, assume [next()] points to first char
    // // NOTE: on return, must check [if (c == nullptr)]
    // void tokenize_ident();
    // void tokenize_factor();
    // void tokenize_term();

    // // break down the [expression] into  it's proper tokens and insert into [tokens]
    // void tokenize_expr();
    // void tokenize_returnStatement();
    // void tokenize_relation();
    // void tokenize_whileStatement();
    // void tokenize_ifStatement();
    // void tokenize_assignment();
    // void tokenize_statement();

    // // the [next()] character that is returned from this function should be `}`
    // // assume [tokenize_statement()] will always return when the character is ';'
    // // FOR NOW: assume [tokenize_statSequence()] will properly terminate even when we don't see the last `;`
    // // TODO: configure a deterministic way to decide when the statement is done (bc it's a non strictly-necessary terminating `;`)
    // void tokenize_statSequence();
    
    // void tokenize_varDecl();
    // void tokenize_func();
    
    // void tokenizer();
};



#endif