#ifndef LEXER_HPP
#define LEXER_HPP

#include <iostream>
#include <vector>

#include "tinyExceptions.hpp"
#include "helper.hpp"
#include "Token.hpp"

// namespace OPERATION {
//     const char PLUS = '+';
//     const char MINUS = '-';
//     const char MULTIPLY = '*';
//     const char DIVIDE = '/';
// };

using namespace tinyExceptions::Lexer;

class Lexer {
public:
    Lexer(std::string in) : source(in) {}

    std::vector<TOKEN> lex() {
        try {
            tokenizer();
            std::cout << "Done Tokenizing." << std::endl;
            return this->tokens;
        } catch (const std::out_of_range &e) {
            std::cerr << "Out of range error: " << e.what() << std::endl;
        } catch (const Incomplete_FACTOR_LexException &e) {
            std::cerr << "Lexer Factor Error: " << e.what() << std::endl;
        } catch (const Incomplete_TERM_LexException &e) {
            std::cerr << "Lexer Term Error: " << e.what() << std::endl;
        } catch (const Incomplete_EXPRESSION_LexException &e) {
            std::cerr << "Lexer Expression Error: " << e.what() << std::endl;
        } catch (const Incomplete_LET_LexException &e) {
            std::cerr << "Lexer Let Error: " << e.what() << std::endl;
        } catch (const Incomplete_MAIN_LexException &e) {
            std::cerr << "Lexer Main Error: " << e.what() << std::endl;
        } catch (...) {
            // Catch-all block for all other exceptions
            std::cerr << "An unknown exception occurred." << std::endl;
        }
        return {};
    }
private:
    bool DEBUG = true;
    // we're not going to know the size/len of the file when we get it
    std::string source;
    size_t s_index = 0;
    // const size_t BUFF_SIZE = 64; // keep const buff size
    // char* buff, overflow;
    std::vector<TOKEN> tokens;

    inline char* next() const {
        if (s_index < source.length()) {
            char *ret = (char *)malloc(sizeof(char));
            *ret = source.at(s_index);
            return ret;
        } else { // EOF
            // OG: 
            // return NULL;
            // Revision: we should use pointers
            // - replace w (char *) and return nullptr
            return nullptr;
            // throw std::out_of_range("EOF");
        }
        return nullptr;
    }

    inline char consume() noexcept {
        return source.at(s_index++);
    }

    void skip_whitespace() {
        char *c;
        while ((c = next()) != nullptr && std::isspace(*c)) { consume(); free(c); }
    }

    void tokenize_factor() {
        // factor = [ident] | [number] | ["(" expr ")"]
        char *c;
        std::string buff;


        // get the first [whatever]
        if ((c = next()) == nullptr) {
            std::stringstream ss;
            if (DEBUG) {
                ss << "(tokenize_factor(), line=91) ";
            }
            ss << "FACTOR expected `[ident] | [number] | ['(' expr ')']`. Got: EOF.";
            throw tinyExceptions::Lexer::Incomplete_FACTOR_LexException(ss.str());
        }

        // case: ["(" expression ")"]
        if (*c == '(') {
            // [1]: open paren
            std::string open(1, *c);
            tokens.push_back(TOKEN{ TOKEN_TYPE::OPEN_PAREN, open});
            free(c);
            
            // [2]: expr
            tokenize_expr();

            // [3]: close paren
            if ((c = next()) != nullptr) {
                if (*c == ')') {
                    std::string close(1, *c);
                    tokens.push_back(TOKEN{ TOKEN_TYPE::CLOSE_PAREN, close});
                    free(c);
                } else {
                    std::stringstream ss;
                    if (DEBUG) {
                        ss << "(tokenize_factor(), line=117) ";
                    }
                    ss << "FACTOR expected `)`. Got: " << *c << ".";
                    throw tinyExceptions::Lexer::Incomplete_FACTOR_LexException(ss.str());
                }
            } else {
                std::stringstream ss;
                if (DEBUG) {
                    ss << "(tokenize_factor(), line=124) ";
                }
                ss << "FACTOR expected `)`. Got: EOF.";
                throw tinyExceptions::Lexer::Incomplete_FACTOR_LexException(ss.str());
            }

        } else {
            // check if it's [ident] | [number]
            if ((c = next()) == nullptr) {
                std::stringstream ss;
                if (this->DEBUG) {
                    ss << "(in tokenize_factor() line=135) ";
                }
                ss << "FACTOR expected `[ident] | [number]`. Got: EOF.";
                throw tinyExceptions::Lexer::Incomplete_FACTOR_LexException(ss.str());
            }
            if (std::isalpha(*c)) { // ident
                buff.push_back(consume());
                free(c);

                while ((c = next()) != nullptr && std::isalnum(*c)) {
                    buff.push_back(consume());
                    free(c);
                }
                // insert token into [tokens]
                if (DEBUG) {
                    std::cout << "\tnew token: " << TOKEN_TYPE_toString(TOKEN_TYPE::IDENTIFIER) << std::endl;
                }
                tokens.push_back(TOKEN{ TOKEN_TYPE::IDENTIFIER, buff });
                buff.clear();
                
            } else if (std::isdigit(*c)) { // number
                buff.push_back(consume());
                free(c);

                while ((c = next()) != nullptr && std::isdigit(*c)) {
                    buff.push_back(consume());
                    free(c);
                }
                // insert token into [tokens]
                if (DEBUG) {
                    std::cout << "\tnew token: " << TOKEN_TYPE_toString(TOKEN_TYPE::NUMBER) << std::endl;
                }
                tokens.push_back(TOKEN{ TOKEN_TYPE::NUMBER, buff });
                buff.clear();
            } else {
                std::stringstream ss;
                if (DEBUG) {
                    ss << "(tokenize_factor(), line=166) ";
                }
                ss << "FACTOR expected `[ident] | [number]`. Got: unexpected factor (" << *c << ").";
                throw tinyExceptions::Lexer::Incomplete_FACTOR_LexException(ss.str());
            }
        }
        skip_whitespace();
    }

    void tokenize_term() {
        char *c;
        std::string buff;

        // [1]: factor1
        skip_whitespace(); // [default]: auto in case there is whitespace
        tokenize_factor(); // [assumption]: this function handles cleanup
        
        // [intermediate processing step]
        if ((c = next()) == nullptr) {
            std::stringstream ss;
            if (DEBUG) {
                ss << "(tokenize_factor(), line=187) ";
            }
            ss << "TERM expected a `[*, /]` operator. Got: EOF.";
            throw tinyExceptions::Lexer::Incomplete_TERM_LexException(ss.str());
        }

        // [2]: op
        std::string op(1, *c);
        if (*c == '*') {
            // insert token into [tokens]
            if (DEBUG) {
                std::cout << "\tnew token: " << TOKEN_TYPE_toString(TOKEN_TYPE::MULTIPLY) << std::endl;
            }
            tokens.push_back(TOKEN{ TOKEN_TYPE::MULTIPLY, op });
        }
        else if (*c == '/') {
            // insert token into [tokens]
            if (DEBUG) {
                std::cout << "\tnew token: " << TOKEN_TYPE_toString(TOKEN_TYPE::DIVIDE) << std::endl;
            }
            tokens.push_back(TOKEN{ TOKEN_TYPE::DIVIDE, op });
        }
        else {
            std::stringstream ss;
            if (DEBUG) {
                ss << "(tokenize_factor(), line=206) ";
            }
            ss << "TERM expected a `[*, /]` operator. Got: " << op << ".";
            throw tinyExceptions::Lexer::Incomplete_TERM_LexException(ss.str());
        }
        free(c);

        // [3] factor2
        skip_whitespace();
        tokenize_factor();

        skip_whitespace();
        // END OF FUNCTION;
    }

    // break down the [expression] into  it's proper tokens and insert into [tokens]
    void tokenize_expr() { // checks will be handled later recursively by the factor
        char *c;
        std::string buff;
        
        
        // [1]: term1
        skip_whitespace(); // [default]: auto in case there is whitespace
        tokenize_term(); // [assumption]: this function handles cleanup
        
        // [intermediate processing step]
        if ((c = next()) == nullptr) {
            std::stringstream ss;
            if (DEBUG) {
                ss << "(tokenize_factor(), line=235) ";
            }
            ss << "EXPRESSION expected a `+/-` operator. Got: EOF.";
            throw tinyExceptions::Lexer::Incomplete_EXPRESSION_LexException(ss.str());
        }

        // [2]: op
        std::string op(1, *c);
        if (*c == '+') {
            // insert token into [tokens]
            if (DEBUG) {
                std::cout << "\tnew token: " << TOKEN_TYPE_toString(TOKEN_TYPE::PLUS) << std::endl;
            }
            tokens.push_back(TOKEN{ TOKEN_TYPE::PLUS, op });
        }
        else if (*c == '-') {
            // insert token into [tokens]
            if (DEBUG) {
                std::cout << "\tnew token: " << TOKEN_TYPE_toString(TOKEN_TYPE::MINUS) << std::endl;
            }
            tokens.push_back(TOKEN{ TOKEN_TYPE::MINUS, op });
        }
        else {
            std::stringstream ss;
            if (DEBUG) {
                ss << "(tokenize_factor(), line=254) ";
            }
            ss << "EXPRESSION expected a `+/-` operator. Got: " << op << ".";
            throw tinyExceptions::Lexer::Incomplete_EXPRESSION_LexException(ss.str());
        }
        free(c);

        // [3] term2
        skip_whitespace();
        tokenize_term();

        skip_whitespace();
        // END OF FUNCTION;
    }

    void tokenizer() {
        this->s_index = 0; // default behavior: reset to 0

        // TOOD: optimize
        // handling parsing with strings for now...
        std::string buff;
        // [DONE]: switch for[i]-loop to be a while[i]
        // - termination condition (figure out what flag indicates EOF!)
        // [REVISED]: handled by changing to char* and checking when nullptr
        char *c;
        // [Below]: ensuring that the program always starts with "main"
        skip_whitespace();
        int main_c = 0;
        while (main_c < 4 && (c = next()) != nullptr && !std::isspace(*c)) {
            buff.push_back(consume());
            free(c);
            main_c++;
        }
        if (c == nullptr) {
            return; // we're done tokenizing (i guess???)
        }
        // [else]: continue execution
        if ((buff.compare("main") != 0) && (buff.compare("MAIN") != 0)) {
            std::stringstream ss;
            if (DEBUG) {
                ss << "(tokenize_factor(), line=294) ";
            }
            ss << "tiny: Expected startKeyword = [main] . Got: " << buff << ".";
            throw tinyExceptions::Lexer::Incomplete_MAIN_LexException(ss.str());
        }
        // insert [main] token into [tokens]
        tokens.push_back(TOKEN{ TOKEN_TYPE::MAIN, buff });    
        buff.clear();
        skip_whitespace();

        // while there are tokens
        while ((c = next()) != nullptr && (*c != '.')) {
            // do something to c
            if (std::isspace(*c)) { consume(); free(c); continue; }
            // isalpha() NOT isalphanum() bc identifiers should start with a letter!
            if (std::isalpha(*c)) {
                buff.push_back(consume());
                free(c);

                // get the rest of the identifier (which can be letters or numbers)
                while ((c = next()) != nullptr && std::isalnum(*c)) {
                    buff.push_back(consume());
                    free(c);
                }
                // if c is not the EOF
                if (c != nullptr) {
                    // determine type
                    if (buff.compare("let") == 0 || buff.compare("LET") == 0) {
                        // insert token into [tokens]
                        if (DEBUG) {
                            std::cout << "\tnew token: " << TOKEN_TYPE_toString(TOKEN_TYPE::LET) << std::endl;
                        }
                        tokens.push_back(TOKEN{ TOKEN_TYPE::LET, buff });    
                        buff.clear();
                        // get ident & expr
                        // first skip the whitespace
                        skip_whitespace();
                        // [1] [ident] must start with alpha!
                        if (std::isalpha(*c)) {
                            buff.push_back(consume());
                            free(c);
                            // gets the entire identifier
                            while ((c = next()) != nullptr && std::isalnum(*c)) { 
                                buff.push_back(consume());
                                free(c);
                            }
                            // insert token into [tokens]
                            if (DEBUG) {
                                std::cout << "\tnew token: " << TOKEN_TYPE_toString(TOKEN_TYPE::IDENTIFIER) << std::endl;
                            }
                            tokens.push_back(TOKEN{ TOKEN_TYPE::IDENTIFIER, buff });    
                            buff.clear();
                            skip_whitespace();
                            // gets the next token; expected: assignment operator [<-]
                            int i = 0; // counter because we should only need to check the next 2 characters
                            while (i < 2 && (c = next()) != nullptr && !std::isspace(*c)) { 
                                buff.push_back(consume());
                                free(c);
                                i++;
                            }
                            // if we got the expected token [<-]
                            if (buff.compare("<-") == 0) {
                                // insert token into [tokens]
                                if (DEBUG) {
                                    std::cout << "\tnew token: " << TOKEN_TYPE_toString(TOKEN_TYPE::ASSIGNMENT) << std::endl;
                                }
                                tokens.push_back(TOKEN{ TOKEN_TYPE::ASSIGNMENT, buff });    
                                buff.clear();
                            } else {
                                std::stringstream ss;
                                if (DEBUG) {
                                    ss << "(tokenize_factor(), line=356) ";
                                }
                                ss << "LET keyword expected [ASSIGNMENT] `<-`. Got: " << buff << ".";
                                throw tinyExceptions::Lexer::Incomplete_LET_LexException(ss.str());
                            }
                            // ASSUMPTION: syntactically valid file [no expr checking!]
                            // REVISED: we actually will grab the entire expression, and pass it into an expr_tokenizer of some sort to handle it
                            skip_whitespace();
                            tokenize_expr();
                            // TODO: last check for valid expr `;`
                            c = next();

                            if (c != nullptr) {
                                if (*c == ';') {
                                    std::string semi(1, *c);
                                    tokens.push_back(TOKEN{ TOKEN_TYPE:: SEMICOLON, semi });
                                } else {
                                    std::stringstream ss;
                                    if (DEBUG) {
                                        ss << "(tokenize_factor(), line=375) ";
                                    }
                                    ss << "LET keyword expected `;`. Got: " << *c << ".";
                                    throw tinyExceptions::Lexer::Incomplete_LET_LexException(ss.str());
                                }
                                free(c);
                            } else {
                                std::stringstream ss;
                                if (DEBUG) {
                                    ss << "(tokenize_factor(), line=384) ";
                                }
                                ss << "LET keyword expected `;`. Got: EOF.";
                                throw tinyExceptions::Lexer::Incomplete_LET_LexException(ss.str());
                            }
                            skip_whitespace();
                        } else {
                            std::stringstream ss;
                            if (DEBUG) {
                                ss << "(tokenize_factor(), line=393) ";
                            }
                            ss << "LET keyword expected `identifier`. Got: " << *c << ". `identifiers` start with a letter.";
                            throw tinyExceptions::Lexer::Incomplete_LET_LexException(ss.str());
                        }
                        continue;
                    } else { // if all else fails, assume it's an [expression]
                        this->s_index -= buff.length();
                        // [2]: [expr]
                        tokenize_expr();
                        // [assumption]: all keywords have a space in between them (ex. [expresion, ` `, `;`])

                        // [3]: parse the `;`
                        std::string semi(1, *c);
                        if ((c = next()) != nullptr) {
                            if (*c == ';') {
                                tokens.push_back(TOKEN{ TOKEN_TYPE::SEMICOLON, semi });
                                free(c);
                            } else {
                                std::stringstream ss;
                                if (DEBUG) {
                                    ss << "(tokenize_factor(), line=414) ";
                                }
                                ss << "EXPRESSION Expected `;`. Got: " << *c << ".";        
                                throw tinyExceptions::Lexer::Incomplete_EXPRESSION_LexException(ss.str());
                            }
                        } else {
                            std::stringstream ss;
                            if (DEBUG) {
                                ss << "(tokenize_factor(), line=422) ";
                            }
                            ss << "EXPRESSION Expected `;`. Got: EOF.";
                            throw tinyExceptions::Lexer::Incomplete_EXPRESSION_LexException(ss.str());
                        }
                    }
                }
            }
            
            
            if (c == nullptr) { break; }
        }
        
        // [3]: `.` token; end
        if (*c == '.') {
            std::string end(1, *c);
            // insert token into [tokens]
            if (DEBUG) {
                std::cout << "\tnew token: " << TOKEN_TYPE_toString(TOKEN_TYPE::END_OF_FILE) << std::endl;
            }
            tokens.push_back(TOKEN{ TOKEN_TYPE::END_OF_FILE, end });
        }


        this->s_index = 0; // default behavior: reset to 0
    }
};

#endif