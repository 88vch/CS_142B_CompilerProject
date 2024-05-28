#include <iostream>
#include <vector>

#include "Lexer.hpp"


using namespace tinyExceptions::Lexer;


std::vector<TOKEN> Lexer::lex() {
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


// return on whitespace, assume [next()] points to first char
// NOTE: on return, must check [if (c == nullptr)]
void Lexer::tokenize_ident() {
    #ifdef DEBUG    
        std::cout << "in tokenize_ident(str=[" << this->source.substr(s_index) << "]); buff=[" << this->buff << "]" << std::endl;
    #endif
    char *c;

    if (((c = next()) != nullptr) && (std::isalpha(*(c = next())))) {
        do {
            this->buff.push_back(consume());
        } while (((c = next()) != nullptr) && (std::isalnum(*(c = next()))));

        #ifdef DEBUG    
            std::cout << "\tbuff=[" << this->buff << "]; *c = [" << *c << "]" << std::endl;    
            std::cout << "\tpushing back token of type: " << TOKEN_TYPE_toString(TOKEN_TYPE::IDENTIFIER) << std::endl;
        #endif
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
    skip_whitespace();
}

void Lexer::tokenize_factor() {
    // factor = [ident] | [number] | ["(" expr ")"]
    char *c;
    // std::string buff;


    // get the first [whatever]
    if ((c = next()) == nullptr) {
        std::stringstream ss;
        #ifdef DEBUG
            ss << "(tokenize_factor(), line=91) ";
        #endif
        ss << "FACTOR expected `[ident] | [number] | ['(' expr ')']`. Got: EOF.";
        throw tinyExceptions::Lexer::Incomplete_FACTOR_LexException(ss.str());
    }

    // case: ["(" expression ")"]
    if (*c == '(') {
        // [1]: open paren
        std::string open(1, *c);
        tokens.push_back(TOKEN{ TOKEN_TYPE::OPEN_PAREN, open});
        
        // [2]: expr
        tokenize_expr();

        // [3]: close paren
        if ((c = next()) != nullptr) {
            if (*c == ')') {
                std::string close(1, *c);
                tokens.push_back(TOKEN{ TOKEN_TYPE::CLOSE_PAREN, close});
            } else {
                std::stringstream ss;
                #ifdef DEBUG
                    ss << "(tokenize_factor(), line=117) ";
                #endif
                ss << "FACTOR expected `)`. Got: " << *c << ".";
                throw tinyExceptions::Lexer::Incomplete_FACTOR_LexException(ss.str());
            }
        } else {
            std::stringstream ss;
            #ifdef DEBUG
                ss << "(tokenize_factor(), line=124) ";
            #endif
            ss << "FACTOR expected `)`. Got: EOF.";
            throw tinyExceptions::Lexer::Incomplete_FACTOR_LexException(ss.str());
        }

    } else {
        // check if it's [ident] | [number]
        if ((c = next()) == nullptr) {
            std::stringstream ss;
            #ifdef DEBUG
                ss << "(in tokenize_factor() line=135) ";
            #endif
            ss << "FACTOR expected `[ident] | [number]`. Got: EOF.";
            throw tinyExceptions::Lexer::Incomplete_FACTOR_LexException(ss.str());
        }
        if (std::isalpha(*c)) { // ident
            this->buff.push_back(consume());

            while ((c = next()) != nullptr && std::isalnum(*c)) {
                this->buff.push_back(consume());
            }
            // insert token into [tokens]
            #ifdef DEBUG
                std::cout << "\tnew token: " << TOKEN_TYPE_toString(TOKEN_TYPE::IDENTIFIER) << std::endl;
            #endif
            tokens.push_back(TOKEN{ TOKEN_TYPE::IDENTIFIER, this->buff });
            this->buff.clear();
            
        } else if (std::isdigit(*c)) { // number
            this->buff.push_back(consume());

            while ((c = next()) != nullptr && std::isdigit(*c)) {
                this->buff.push_back(consume());
            }
            // insert token into [tokens]
            #ifdef DEBUG
                std::cout << "\tnew token: " << TOKEN_TYPE_toString(TOKEN_TYPE::NUMBER) << std::endl;
            #endif
            tokens.push_back(TOKEN{ TOKEN_TYPE::NUMBER, this->buff });
            this->buff.clear();
        } else {
            std::stringstream ss;
            #ifdef DEBUG
                ss << "(tokenize_factor(), line=166) ";
            #endif
            ss << "FACTOR expected `[ident] | [number]`. Got: unexpected factor (" << *c << ").";
            throw tinyExceptions::Lexer::Incomplete_FACTOR_LexException(ss.str());
        }
    }
    skip_whitespace();
}

void Lexer::tokenize_term() {
    char *c;

    // [1]: factor1
    skip_whitespace(); // [default]: auto in case there is whitespace
    tokenize_factor(); // [assumption]: this function handles cleanup
    
    // [intermediate processing step]
    if ((c = next()) == nullptr) {
        std::stringstream ss;
        #ifdef DEBUG
            ss << "(tokenize_factor(), line=187) ";
        #endif
        ss << "TERM expected a `[*, /]` operator. Got: EOF.";
        throw tinyExceptions::Lexer::Incomplete_TERM_LexException(ss.str());
    }

    // [2]: op
    TOKEN_TYPE tmp;
    bool has_next_factor;
    if (*c == '*') {
        has_next_factor = true;
        tmp = TOKEN_TYPE::MULTIPLY;
    }
    else if (*c == '/') {
        has_next_factor = true;
        tmp = TOKEN_TYPE::DIVIDE;
    }
    else { has_next_factor = false; }

    // [3*] optional next term
    if (has_next_factor) {
        #ifdef DEBUG
            std::cout << "\tnew token: " << TOKEN_TYPE_toString(tmp) << std::endl;
        #endif
        // insert token into [tokens]
        tokens.push_back(TOKEN{ tmp, this->buff });
        this->buff.clear();

        // [3b] factor2
        skip_whitespace();
        tokenize_factor();
    }

    skip_whitespace();
    // END OF FUNCTION;
}

// break down the [expression] into  it's proper tokens and insert into [tokens]
void Lexer::tokenize_expr() { // checks will be handled later recursively by the factor
    char *c;
    
    
    // [1]: term1
    skip_whitespace(); // [default]: auto in case there is whitespace
    tokenize_term(); // [assumption]: this function handles cleanup
    
    // [intermediate processing step]
    if ((c = next()) == nullptr) {
        std::stringstream ss;
        #ifdef DEBUG
            ss << "(tokenize_factor(), line=235) ";
        #endif
        ss << "EXPRESSION expected a `+/-` operator. Got: EOF.";
        throw tinyExceptions::Lexer::Incomplete_EXPRESSION_LexException(ss.str());
    }

    // [2]: op
    TOKEN_TYPE tmp;
    bool has_next_term;
    if (*c == '+') {
        has_next_term = true;
        // insert token into [tokens]
        this->buff.push_back(consume());
        tmp = TOKEN_TYPE::PLUS;
    }
    else if (*c == '-') {
        has_next_term = true;
        // insert token into [tokens]
        this->buff.push_back(consume());
        tmp = TOKEN_TYPE::MINUS;
    }
    else { has_next_term = false; }
    
    // [3*] optional next term
    if (has_next_term) {
        #ifdef DEBUG
            std::cout << "\tnew token: " << TOKEN_TYPE_toString(tmp) << std::endl;
        #endif
        tokens.push_back(TOKEN{ tmp, this->buff });
        this->buff.clear();

        // [3b] term2
        skip_whitespace();
        tokenize_term();
    }

    skip_whitespace();
    // END OF FUNCTION;
}

void Lexer::tokenize_returnStatement() {
    // if we do get an expression, 
    // - we're going to handle it in the respective function either way so nothing needs to be done here either 
    try { tokenize_expr(); } 
    catch (const std::exception &err) {}
    // if it threw an exception, 
    // - then we did not have an [expression] (optional arg)
    // we're only catching it so our Lexer can continue to tokenize!
}

void Lexer::tokenize_whileStatement() {
    // tokenize_relation(); // should return on [whitespace] (i guess??)
    skip_whitespace();
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


void Lexer::tokenize_ifStatement() {
    // tokenize_relation(); // should return on [whitespace] (i guess??)
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

void Lexer::tokenize_assignment() { // already got first token "let"
    #ifdef DEBUG    
        std::cout << "in tokenize_assignment(str=[" << this->source.substr(s_index) << "]); buff=[" << this->buff << "]" << std::endl;
    #endif
    tokenize_ident(); // should return on `<-` (assignment)

    char *c;
    if ((c = next()) == nullptr) {
        std::stringstream ss;
        ss << "Assignemnt Expected `<-`. Got: EOF.";
        throw Incomplete_ASSIGNMENT_LexException(ss.str());
    } 
    if (*(c = next()) == '<') {
        this->buff.push_back(consume());
        if (*(c = next()) == '-') {
            #ifdef DEBUG    
                std::cout << "\tpushing back token of type: " << TOKEN_TYPE_toString(TOKEN_TYPE::ASSIGNMENT) << std::endl;
            #endif
            this->buff.push_back(consume());
            this->tokens.push_back(TOKEN{ TOKEN_TYPE::ASSIGNMENT, this->buff });
            this->buff.clear();
        } else {
            std::stringstream ss;
            ss << "Assignemnt Expected `<-`. Got: [" << *c << "].";
            throw Incomplete_ASSIGNMENT_LexException(ss.str());
        }
    } else {
        std::stringstream ss;
        ss << "Assignemnt Expected `<-`. Got: [" << *c << "].";
        throw Incomplete_ASSIGNMENT_LexException(ss.str());
    }
    skip_whitespace();
    tokenize_expr();
}

void Lexer::tokenize_statement() {
    #ifdef DEBUG    
        std::cout << "in tokenize_statement(str=[" << this->source.substr(s_index) << "]); buff=[" << this->buff << "]" << std::endl;
    #endif
    // statement = { assignment | funcCall3 | ifStatement | whileStatement | returnStatement }
    skip_whitespace(); // sanity check: skip whitespace if we accidentally forgot to skip somewhere before calling [tokenize_statement()]
    

    /*
        assignment expects "let"
        ifStatement expects "if"
        whileStatement expects "while"
        returnStatement expects "return"

        NOTE: ignoring [funcCall] for now...REMEMBER [TODO] LATER
    */
    char *c = next();
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
    } else if (*c != '}') {
        // end of statement [ `;` | `}` ]
        // - let statSequence() handle [end of statement]
        // - assume [this->buff] is currently empty
        // - throw error if we don't encounter this
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
void Lexer::tokenize_statSequence() {
    #ifdef DEBUG    
        std::cout << "in tokenize_statSequence(str=[" << this->source.substr(s_index) << "]); buff=[" << this->buff << "]" << std::endl;
    #endif
    skip_whitespace();
    tokenize_statement(); // should return on ";" (optional for terminating); we're going to push a ';' on termination regardless (for easy parsing later)

    char *c;
    if (((c = next()) != nullptr)) {
        if (*c == ';') {
            #ifdef DEBUG    
                std::cout << "in tokenize_statSequence(str=[" << this->source.substr(s_index) << "]); buff=[" << this->buff << "]" << std::endl;
                std::cout << "\tpushing back token of type: " << TOKEN_TYPE_toString(TOKEN_TYPE::SEMICOLON) << std::endl;
            #endif
            this->buff.push_back(consume());
            this->tokens.push_back(TOKEN{ TOKEN_TYPE::SEMICOLON, this->buff });
            this->buff.clear();
        } else if ((std::isalnum(*c)) || (*c == '}')) {
            #ifdef DEBUG    
                std::cout << "in tokenize_statSequence(str=[" << this->source.substr(s_index) << "]); buff=[" << this->buff << "]" << std::endl;
            #endif
            // if (*c == '}'): indicates end of [main] [computation] (we let [tokenize()] handle this)
                // if we get another char that isn't `;`, we can assume that the most recent statement was the last statement in this statSequence (since it may not necessarily return a ;)
            // let's make this a deterministic situation for our tokens (i.e. add the `;` if it doesn't exist)
            this->buff.push_back(';');
            #ifdef DEBUG    
                std::cout << "\tpushing back token of type: " << TOKEN_TYPE_toString(TOKEN_TYPE::SEMICOLON) << std::endl;
            #endif
            this->tokens.push_back(TOKEN{ TOKEN_TYPE::SEMICOLON, this->buff });
            this->buff.clear();
            skip_whitespace();
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
    skip_whitespace();
}

// TODO;
void Lexer::tokenize_var() {}

// TODO; 
void Lexer::tokenize_func() {}

void Lexer::tokenizer() {
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
        main_c++;
    }
    if (c == nullptr) {
        return; // we're done tokenizing (i guess???)
    }
    // [else]: continue execution
    if (this->buff.compare("main") != 0) {
        std::stringstream ss;
        ss << "tiny: Expected startKeyword = [main] . Got: " << this->buff << ".";
        throw tinyExceptions::Lexer::Incomplete_MAIN_LexException(ss.str());
    }
    // insert [main] token into [tokens]
    #ifdef DEBUG
        std::cout << "pushing back token of type: " << TOKEN_TYPE_toString(TOKEN_TYPE::MAIN) << std::endl;
    #endif
    tokens.push_back(TOKEN{ TOKEN_TYPE::MAIN, this->buff });    
    this->buff.clear();
    skip_whitespace();

    // check for [var] & [func] declarations
    if (std::isalpha(*c)) {
        while ((c = next()) != nullptr && std::isalnum(*c)) {
            this->buff.push_back(consume());
        }

        if (this->buff.compare("var") == 0 || this->buff.compare("VAR") == 0) {
            this->tokens.push_back(TOKEN{ TOKEN_TYPE::VAR, this->buff });
            this->buff.clear();
            tokenize_var();
        } else if (this->buff.compare("void") == 0 || this->buff.compare("VOID") == 0 ||
                   this->buff.compare("function") == 0 || this->buff.compare("FUNCTION") == 0) {
            // should we push a [void] (type) token before? (thinking abt it idk; need to learn more abt functions first)
            this->tokens.push_back(TOKEN{ TOKEN_TYPE::FUNCTION, this->buff });
            this->buff.clear();
            tokenize_func();
        }


        if (c == nullptr) {
            std::stringstream ss;
            if (c == nullptr) { // indicates EOF
                ss << "Main Expected [`{` || `var decl` || `function`]; Got: EOF.";
            } else {
                ss << "Main Expected [`{` || `var decl` || `function`]; Got: " << *c << ".";
            }
            throw tinyExceptions::Lexer::Incomplete_MAIN_LexException(ss.str());
        }
    }
    
    skip_whitespace();
    c = next();
    // checking for [statSequence()] and then [END: `.`]
    if (c != nullptr && *c == '{') {
        #ifdef DEBUG
            std::cout << "pushing back token of type: " << TOKEN_TYPE_toString(TOKEN_TYPE::OPEN_CURLY) << std::endl;
        #endif
        this->buff.push_back(consume());
        this->tokens.push_back(TOKEN{ TOKEN_TYPE::OPEN_CURLY, this->buff });
        this->buff.clear();
        skip_whitespace();

        tokenize_statSequence(); // should be looking for '}' as indicator to return
        if (*(c = next()) == '}') {
            #ifdef DEBUG
                std::cout << "pushing back token of type: " << TOKEN_TYPE_toString(TOKEN_TYPE::CLOSE_CURLY) << std::endl;
            #endif
            // then we know we're done with the [statSequence]
            // - expected next char is last [.]
            this->buff.push_back(consume());
            this->tokens.push_back(TOKEN{ TOKEN_TYPE::CLOSE_CURLY, this->buff });
            this->buff.clear();
            skip_whitespace();

            // [checking for end] include this below too!
            if (((c = next()) != nullptr)  && (*(c = next()) == '.')) {
                #ifdef DEBUG
                    std::cout << "\tnew token: " << TOKEN_TYPE_toString(TOKEN_TYPE::END_OF_FILE) << std::endl;
                #endif
                this->buff.push_back(consume());
                this->tokens.push_back(TOKEN{ TOKEN_TYPE::END_OF_FILE, this->buff });
                this->buff.clear();
            } else {
                std::stringstream ss;
                if (c == nullptr) { // indicates EOF
                    ss << "End of main Expected `.`; Got: EOF.";
                } else {
                    ss << "End of main Expected `.`; Got: " << *c << ".";
                }
                throw tinyExceptions::Lexer::Incomplete_MAIN_LexException(ss.str());
            }
        } else {
            std::stringstream ss;
            if (c == nullptr) { // indicates EOF
                ss << "End of statSequence Expected `}`. Got: EOF.";
            } else {
                ss << "End of statSequence Expected `}`. Got: " << *c << ".";
            }
            throw tinyExceptions::Lexer::Incomplete_statSequence_LexException(ss.str());
        }
    } else {
        std::stringstream ss;
        if (c == nullptr) { // indicates EOF
            ss << "End of statSequence Expected `some IDENTIFIER`. Got: EOF.";
        } else {
            ss << "End of statSequence Expected `some IDENTIFIER`. Got: " << this->buff << " & " << *c << ".";
        }
        throw tinyExceptions::Lexer::Incomplete_statSequence_LexException(ss.str());
    }
    this->s_index = 0; // default behavior: reset to 0; should be done tokenizing now
}