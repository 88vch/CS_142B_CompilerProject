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
    Lexer(std::string in) 
        : source(std::move(in))
    {
        this->source_len = this->source.length();
    }


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
    const std::string source;
    std::string buff;
    size_t source_len, s_index = 0;
    // const size_t BUFF_SIZE = 64; // keep const buff size
    // char* buff, overflow;
    std::vector<TOKEN> tokens;

    // does NOT consume char, only peeks()
    inline char* next() const {
        if (this->s_index < this->source_len) {
            char *ret;
            *ret = source.at(s_index);
            return ret;
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
        while ((c = next()) != nullptr && std::isspace(*c)) { consume(); free(c); }
    }

    std::string get_deterministic_token(int characters, std::string expected) {
        char *c;
        for (int i = 0; i < characters; i++) {
            if (((c = next()) != nullptr) && (std::isalpha(*(c = next())))) {
                this->buff.push_back(std::tolower(consume()));
            } else if ((c = next()) == nullptr) {
                std::stringstream ss;
                ss << "ifStatement Expected `" << expected << "`. Got: EOF.";
                throw Incomplete_Token_LexException(ss.str());
            }
        }
    }

    // return on whitespace, assume [next()] points to first char
    // NOTE: on return, must check [if (c == nullptr)]
    void tokenize_ident() {
        char *c;

        if (((c = next()) != nullptr) && (std::isalpha(*(c = next())))) {
            do {
                this->buff.push_back(consume());
            } while (((c = next()) != nullptr) && (std::isalnum(*(c = next()))));

            if ((c != nullptr) && (*c != ' ')) {
                std::stringstream ss;
                ss << "Identifier expected to contain only {letter | digit}";
                throw Incomplete_IDENTIFIER_LexException(ss.str());
            }

            this->tokens.push_back(TOKEN{ TOKEN_TYPE::IDENTIFIER, this->buff });
            this->buff.clear();
        } else {
            std::stringstream ss;
            if (c == nullptr) {
                ss << "Identifier Expected to start with a letter [a-z;A-Z]. Got: EOF.";
            } else {
                ss << "Identifier Expected to start with a letter [a-z;A-Z]. Got: " << *c << ".";
            }
            throw Incomplete_IDENTIFIER_LexException(ss.str());
        }

    }

    void tokenize_factor() {
        // factor = [ident] | [number] | ["(" expr ")"]
        char *c;
        // std::string buff;


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
                this->buff.push_back(consume());
                free(c);

                while ((c = next()) != nullptr && std::isalnum(*c)) {
                    this->buff.push_back(consume());
                    free(c);
                }
                // insert token into [tokens]
                if (DEBUG) {
                    std::cout << "\tnew token: " << TOKEN_TYPE_toString(TOKEN_TYPE::IDENTIFIER) << std::endl;
                }
                tokens.push_back(TOKEN{ TOKEN_TYPE::IDENTIFIER, this->buff });
                this->buff.clear();
                
            } else if (std::isdigit(*c)) { // number
                this->buff.push_back(consume());
                free(c);

                while ((c = next()) != nullptr && std::isdigit(*c)) {
                    this->buff.push_back(consume());
                    free(c);
                }
                // insert token into [tokens]
                if (DEBUG) {
                    std::cout << "\tnew token: " << TOKEN_TYPE_toString(TOKEN_TYPE::NUMBER) << std::endl;
                }
                tokens.push_back(TOKEN{ TOKEN_TYPE::NUMBER, this->buff });
                this->buff.clear();
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

    void tokenize_returnStatement() {
        // if we do get an expression, 
        // - we're going to handle it in the respective function either way so nothing needs to be done here either 
        try { tokenize_expr(); } 
        catch (const std::exception &err) {}
        // if it threw an exception, 
        // - then we did not have an [expression] (optional arg)
        // we're only catching it so our Lexer can continue to tokenize!
    }

    void tokenize_whileStatement() {
        tokenize_relation(); // should return on [whitespace] (i guess??)
        skip_whitespace();

        char *c;
        this->buff = get_deterministic_token(2, "do");

        if (this->buff.compare(TOKEN_TYPE_toStringLower(TOKEN_TYPE::DO)) != 0) {
            std::stringstream ss;
            ss << "ifStatement Expected `DO`. Got: " << this->buff << ".";
            throw Incomplete_ifStatement_LexException(ss.str());
        }

        this->tokens.push_back(TOKEN{ TOKEN_TYPE::DO, this->buff });
        this->buff.clear();

        skip_whitespace();
        tokenize_statSequence();
        skip_whitespace();

        this->buff = get_deterministic_token(2, TOKEN_TYPE_toStringLower(TOKEN_TYPE::OD));

        if (this->buff.compare(TOKEN_TYPE_toStringLower(TOKEN_TYPE::DO)) != 0) {
            std::stringstream ss;
            ss << "ifStatement Expected `DO`. Got: " << this->buff << ".";
            throw Incomplete_ifStatement_LexException(ss.str());
        }

        this->tokens.push_back(TOKEN{ TOKEN_TYPE::OD, this->buff });
        this->buff.clear();
    }
    
    
    void tokenize_ifStatement() {
        tokenize_relation(); // should return on [whitespace] (i guess??)
        skip_whitespace();
        
        char *c;
        this->buff = get_deterministic_token(4, "then");

        if (this->buff.compare(TOKEN_TYPE_toStringLower(TOKEN_TYPE::THEN)) != 0) {
            std::stringstream ss;
            ss << "ifStatement Expected `THEN`. Got: " << this->buff << ".";
            throw Incomplete_ifStatement_LexException(ss.str());
        }

        this->tokens.push_back(TOKEN{ TOKEN_TYPE::THEN, this->buff });
        this->buff.clear();

        skip_whitespace();
        tokenize_statSequence();
        skip_whitespace();

        while (((c = next()) != nullptr) && (*(c = next()) != ' ')) {
            this->buff.push_back(std::tolower(consume()));
        }

        if (this->buff.compare(TOKEN_TYPE_toStringLower(TOKEN_TYPE::ELSE)) == 0) {
            this->tokens.push_back(TOKEN{ TOKEN_TYPE::ELSE, this->buff });
            this->buff.clear();

            skip_whitespace();
            tokenize_statSequence(); // i guess we can say they end with a space

            this->buff = get_deterministic_token(2, TOKEN_TYPE_toStringLower(TOKEN_TYPE::FI));
            if (this->buff.compare(TOKEN_TYPE_toStringLower(TOKEN_TYPE::FI)) == 0) {
                this->tokens.push_back(TOKEN{ TOKEN_TYPE::FI, this->buff });
                this->buff.clear();
                // we've successfully tokenized the [ifStatement]
            } else {
                std::stringstream ss;
                ss << "End ifStatement Expected `FI`. Got: " << this->buff << ".";
                throw Incomplete_ifStatement_LexException(ss.str());
            }
        } else if (this->buff.compare(TOKEN_TYPE_toStringLower(TOKEN_TYPE::FI)) == 0) {
            this->tokens.push_back(TOKEN{ TOKEN_TYPE::FI, this->buff });
            this->buff.clear();
            // we've successfully tokenized the [ifStatement]
        } else {
            std::stringstream ss;
            ss << "ifStatement Expected `ELSE` | `FI`. Got: " << this->buff << ".";
            throw Incomplete_ifStatement_LexException(ss.str());
        }
    }

    void tokenize_assignment() { // already got first token "let"
        tokenize_ident(); // should return on `<-` (assignment)

        char *c;
        if ((c = next()) == nullptr) {
            std::stringstream ss;
            ss << "Assignemnt Expected `<-`. Got: EOF.";
            throw Incomplete_ASSIGNMENT_LexException(ss.str());
        } 
        skip_whitespace();
        tokenize_expr();
    }

    void tokenize_statement() {
        // statement = { assignment | funcCall3 | ifStatement | whileStatement | returnStatement }
        skip_whitespace(); // sanity check: skip whitespace if we accidentally forgot to skip somewhere before calling [tokenize_statement()]
        

        /*
            assignment expects "let"
            ifStatement expects "if"
            whileStatement expects "while"
            returnStatement expects "return"

            NOTE: ignoring [funcCall] for now...REMEMBER [TODO] LATER
        */
        char *c;
        if (std::isalpha(*c)) { 
            this->buff.push_back(std::tolower(consume()));
            // while we have letters, push them into buffer
            while (((c = next()) != nullptr)) {
                if (std::isalpha(*c)) {
                    this->buff.push_back(std::tolower(consume()));
                } else if (*c == ' ') {
                    skip_whitespace();
                    // we should have a token in the buffer now
                    if (buff.compare(TOKEN_TYPE_toStringLower(TOKEN_TYPE::LET)) == 0) {
                        this->tokens.push_back(TOKEN{ TOKEN_TYPE::LET, buff });
                        this->buff.clear();
                        tokenize_assignment();
                    } else if (buff.compare(TOKEN_TYPE_toStringLower(TOKEN_TYPE::IF)) == 0) {
                        this->tokens.push_back(TOKEN{ TOKEN_TYPE::IF, buff });
                        this->buff.clear();
                        tokenize_ifStatement();
                    } else if (buff.compare(TOKEN_TYPE_toStringLower(TOKEN_TYPE::WHILE)) == 0) {
                        this->tokens.push_back(TOKEN{ TOKEN_TYPE::WHILE, buff });
                        this->buff.clear();
                        tokenize_whileStatement();
                    } else if (buff.compare(TOKEN_TYPE_toStringLower(TOKEN_TYPE::RETURN)) == 0) {
                        this->tokens.push_back(TOKEN{ TOKEN_TYPE::RETURN, buff });
                        this->buff.clear();
                        tokenize_returnStatement();
                    } else {
                        std::stringstream ss;
                        ss << "Invalid statement! Expected { assignment | funcCall3 | ifStatement | whileStatement | returnStatement }. Got: " << this->buff << ".";
                        throw Incomplete_statement_LexException(ss.str());
                    }
                    break; // exit when we have finished tokenizing the statement
                }
            } 


            if (c == nullptr) {
                std::stringstream ss;
                ss << "End of statement Expected `;`. Got: " << *c << ".";
                throw Incomplete_statement_LexException(ss.str());
            }
        } else {
            std::stringstream ss;
            ss << "Statement Expected to start with [letter]. Got: " << *c << ".";
            throw Incomplete_statement_LexException(ss.str());
        }
        skip_whitespace();
    }

    // the [next()] character that is returned from this function should be `}`
    // assume [tokenize_statement()] will always return when the character is ';'
    // FOR NOW: assume [tokenize_statSequence()] will properly terminate even when we don't see the last `;`
    // TODO: configure a deterministic way to decide when the statement is done (bc it's a non strictly-necessary terminating `;`)
    void tokenize_statSequence() {
        skip_whitespace();
        tokenize_statement(); // should return on ";" (optional for terminating); we're going to push a ';' on termination regardless (for easy parsing later)

        char *c;
        if (((c = next()) != nullptr)) {
            if (*c == ';') {
                this->buff.push_back(consume());
                this->tokens.push_back(TOKEN{ TOKEN_TYPE::SEMICOLON, this->buff });
                this->buff.clear();
            } else if (std::isalnum(*c)) {
                // if we get another char that isn't `;`, we can assume that the most recent statement was the last statement in this statSequence (since it may not necessarily return a ;)
                // let's make this a deterministic situation for our tokens
                this->buff.push_back(';');
                this->tokens.push_back(TOKEN{ TOKEN_TYPE::SEMICOLON, this->buff });
                this->buff.clear();
                return; // no need to continue tokenizing "statements" if we've already reached the end
            }
        }

        while (((c = next()) != nullptr) && (*(c = next()) != '}')) {
            skip_whitespace();
            tokenize_statement(); // expected `;` return [refer to TODO]
            if (((c = next()) != nullptr)) {
                if (*c == ';') {
                    this->buff.push_back(consume());
                    this->tokens.push_back(TOKEN{ TOKEN_TYPE::SEMICOLON, this->buff });
                    this->buff.clear();
                } else if (std::isalnum(*c)) {
                    // if we get another char that isn't `;`, we can assume that the most recent statement was the last statement in this statSequence (since it may not necessarily return a ;)
                    // let's make this a deterministic situation for our tokens
                    this->buff.push_back(';');
                    this->tokens.push_back(TOKEN{ TOKEN_TYPE::SEMICOLON, this->buff });
                    this->buff.clear();
                    break;
                }
            }
        }

        if (c == nullptr) {
            std::stringstream ss;
            ss << "End of statSequence Expected `}`. Got: EOF.";
            throw Incomplete_statSequence_LexException(ss.str());
        }
    }

    void tokenizer() {
        this->s_index = 0; // default behavior: reset to 0

        // TOOD: optimize
        // handling parsing with strings for now...
        // [DONE]: switch for[i]-loop to be a while[i]
        // - termination condition (figure out what flag indicates EOF!)
        // [REVISED]: handled by changing to char* and checking when nullptr
        char *c;
        // [Below]: ensuring that the program always starts with "main"
        skip_whitespace(); // if there is any whitespace before the first letter, skip it
        int main_c = 0;
        while (main_c < 4 && (c = next()) != nullptr && !std::isspace(*c)) {
            this->buff.push_back(std::tolower(consume()));
            free(c);
            main_c++;
        }
        if (c == nullptr) {
            return; // we're done tokenizing (i guess???)
        }
        // [else]: continue execution
        if (this->buff.compare("main") != 0) {
            std::stringstream ss;
            // if (DEBUG) {
            //     ss << "(tokenize_factor(), line=294) ";
            // }
            ss << "tiny: Expected startKeyword = [main] . Got: " << this->buff << ".";
            throw tinyExceptions::Lexer::Incomplete_MAIN_LexException(ss.str());
        }
        // insert [main] token into [tokens]
        tokens.push_back(TOKEN{ TOKEN_TYPE::MAIN, this->buff });    
        this->buff.clear();
        skip_whitespace();

        // while there are tokens
        while ((c = next()) != nullptr && this->s_index < this->source_len) {
            // do something to c
            if (std::isspace(*c)) { 
                consume(); 
                continue;
            } else if (std::isalpha(*c)) { 
                // isalpha() NOT isalphanum() bc identifiers should start with a letter!
                this->buff.push_back(consume());
                free(c);

                // get the rest of the identifier (which can be letters or numbers)
                while ((c = next()) != nullptr && std::isalnum(*c)) {
                    this->buff.push_back(consume());
                    free(c);
                }
                // if c is not the EOF
                if (c != nullptr) {
                    // determine type
                    if (this->buff.compare("let") == 0 || this->buff.compare("LET") == 0) {
                        // insert token into [tokens]
                        if (DEBUG) {
                            std::cout << "\tnew token: " << TOKEN_TYPE_toString(TOKEN_TYPE::LET) << std::endl;
                        }
                        tokens.push_back(TOKEN{ TOKEN_TYPE::LET, this->buff });    
                        this->buff.clear();
                        // get ident & expr
                        // first skip the whitespace
                        skip_whitespace();
                        // [1] [ident] must start with alpha!
                        if (std::isalpha(*c)) {
                            this->buff.push_back(consume());
                            free(c);
                            // gets the entire identifier
                            while ((c = next()) != nullptr && std::isalnum(*c)) { 
                                this->buff.push_back(consume());
                                free(c);
                            }
                            // insert token into [tokens]
                            if (DEBUG) {
                                std::cout << "\tnew token: " << TOKEN_TYPE_toString(TOKEN_TYPE::IDENTIFIER) << std::endl;
                            }
                            tokens.push_back(TOKEN{ TOKEN_TYPE::IDENTIFIER, this->buff });    
                            this->buff.clear();
                            skip_whitespace();
                            // gets the next token; expected: assignment operator [<-]
                            int i = 0; // counter because we should only need to check the next 2 characters
                            while (i < 2 && (c = next()) != nullptr && !std::isspace(*c)) { 
                                this->buff.push_back(consume());
                                free(c);
                                i++;
                            }
                            // if we got the expected token [<-]
                            if (this->buff.compare("<-") == 0) {
                                // insert token into [tokens]
                                if (DEBUG) {
                                    std::cout << "\tnew token: " << TOKEN_TYPE_toString(TOKEN_TYPE::ASSIGNMENT) << std::endl;
                                }
                                tokens.push_back(TOKEN{ TOKEN_TYPE::ASSIGNMENT, this->buff });    
                                this->buff.clear();
                            } else {
                                std::stringstream ss;
                                if (DEBUG) {
                                    ss << "(tokenize_factor(), line=356) ";
                                }
                                ss << "LET keyword expected [ASSIGNMENT] `<-`. Got: " << this->buff << ".";
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
                        this->s_index -= this->buff.length();
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
            } else if (*c == '{') {
                this->buff.push_back(consume());
                this->tokens.push_back(TOKEN{ TOKEN_TYPE::OPEN_CURLY, this->buff });
                this->buff.clear();
                skip_whitespace();

                tokenize_statSequence(); // should be looking for '}' as indicator to return
                if (*(c = next()) == '}') {
                    // then we know we're done with the [statSequence]
                    // - expected next char is last [.]
                    this->buff.push_back(consume());
                    this->tokens.push_back(TOKEN{ TOKEN_TYPE::CLOSE_CURLY, this->buff });
                    this->buff.clear();
                    skip_whitespace();

                    if (((c = next()) != nullptr)  && (*(c = next()) == '.')) {
                        // std::string end(1, *c);
                        this->buff.push_back(consume());
                        if (DEBUG) {
                            std::cout << "\tnew token: " << TOKEN_TYPE_toString(TOKEN_TYPE::END_OF_FILE) << std::endl;
                        }
                        this->tokens.push_back(TOKEN{ TOKEN_TYPE::END_OF_FILE, this->buff });
                        this->buff.clear();
                    } else {
                        std::stringstream ss;
                        if (c == nullptr) { // indicates EOF
                            ss << "End of main Expected `.`; Got: EOF.";
                        } else {
                            ss << "End of main Expected `.`; Got: " << *c << ".";
                        }
                        throw Incomplete_MAIN_LexException(ss.str());
                    }
                } else {
                    std::stringstream ss;
                    if (c == nullptr) { // indicates EOF
                        ss << "End of statSequence Expected `}`. Got: EOF.";
                    } else {
                        ss << "End of statSequence Expected `}`. Got: " << *c << ".";
                    }
                    throw Incomplete_statSequence_LexException(ss.str());
                }
            } else if (c == nullptr) { break; }
        }
        this->s_index = 0; // default behavior: reset to 0; should be done tokenizing now
    }
};

#endif