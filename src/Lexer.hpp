#ifndef LEXER_HPP
#define LEXER_HPP

#include <iostream>
#include <sstream>
#include <vector>

#include "Token.hpp"
#define DEBUG


// namespace OPERATION {
//     const char PLUS = '+';
//     const char MINUS = '-';
//     const char MULTIPLY = '*';
//     const char DIVIDE = '/';
// };


class Lexer {
public:
    Lexer(std::string in) 
        : source(std::move(in))
    {
        this->source_len = this->source.length();
        #ifdef DEBUG
            std::cout << "Read input. Got: \n" << this->source << std::endl;
        #endif
    }


    std::vector<TOKEN> lex();
    size_t source_len, s_index = 0;
    const std::string source;
    std::string buff;
    std::vector<TOKEN> tokens;
private:
    // we're not going to know the size/len of the file when we get it
    // const size_t BUFF_SIZE = 64; // keep const buff size
    // char* buff, overflow;

    // does NOT consume char, only peeks()
    inline char* next() const {
        if (this->s_index < this->source_len) {
            const char *ret = &(source.at(s_index));
            char *tmp = const_cast<char *>(ret);
            return tmp;
        }
        // else: EOF
        // OG: return NULL;
        // Revision: we should use pointers
        // - replace w (char *) and return nullptr
        return nullptr;
    }

    inline char consume() noexcept {
        return source.at(s_index++);
    }

    void skip_whitespace() {
        char *c;
        while ((c = next()) != nullptr && std::isspace(*c)) { consume(); }
    }

    // simply returns the next [characters] chars or [""] if EOF
    std::string get_deterministic_token(int characters) {
        char *c;
        for (int i = 0; i < characters; i++) {
            if (((c = next()) != nullptr) && (std::isalpha(*(c = next())))) { this->buff.push_back(std::tolower(consume())); } 
            else if ((c = next()) == nullptr) { return ""; }
        }
        std::string res = this->buff;
        this->buff.clear();
        return res;
    }

    // return on whitespace, assume [next()] points to first char
    // NOTE: on return, must check [if (c == nullptr)]
    void tokenize_ident();
    void tokenize_factor();
    void tokenize_term();

    // break down the [expression] into  it's proper tokens and insert into [tokens]
    void tokenize_expr();
    void tokenize_returnStatement();
    void tokenize_whileStatement();
    void tokenize_ifStatement();
    void tokenize_assignment();
    void tokenize_statement();

    // the [next()] character that is returned from this function should be `}`
    // assume [tokenize_statement()] will always return when the character is ';'
    // FOR NOW: assume [tokenize_statSequence()] will properly terminate even when we don't see the last `;`
    // TODO: configure a deterministic way to decide when the statement is done (bc it's a non strictly-necessary terminating `;`)
    void tokenize_statSequence();
    
    void tokenize_varDecl();
    void tokenize_func();
    
    void tokenizer();
};

#endif