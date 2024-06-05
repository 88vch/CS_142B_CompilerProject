#include <iostream>
#include <vector>

#include "tinyExceptions.hpp"
#include "Lexer.hpp"


using namespace tinyExceptions_ns;


std::vector<TOKEN> Lexer::lex() {
    try {
        tokenizer();
        std::cout << "Done Tokenizing." << std::endl;
    } catch (const std::out_of_range &e) {
        std::cerr << "Out of range error: " << e.what() << std::endl;
    } catch (const Lexer_ns::Incomplete_FACTOR_LexException &e) {
        std::cerr << "Lexer Factor Error: " << e.what() << std::endl;
    } catch (const Lexer_ns::Incomplete_TERM_LexException &e) {
        std::cerr << "Lexer Term Error: " << e.what() << std::endl;
    } catch (const Lexer_ns::Incomplete_EXPRESSION_LexException &e) {
        std::cerr << "Lexer Expression Error: " << e.what() << std::endl;
    } catch (const Lexer_ns::Incomplete_ASSIGNMENT_LexException &e) {
        std::cerr << "Lexer Assignent Error: " << e.what() << std::endl;
    } catch (const Lexer_ns::Incomplete_MAIN_LexException &e) {
        std::cerr << "Lexer Main Error: " << e.what() << std::endl;
    } catch (...) {
        // Catch-all block for all other exceptions
        std::cerr << "Caught an unknown exception (which occurred during Lexing)(Lexing stopped at error)." << std::endl;
    }
    return this->tokens;
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
        this->mostRecentTokenType = TOKEN_TYPE::IDENTIFIER;
        this->buff.clear();
    } else {
        std::string err = "Identifier Expected to start with a letter [a-z;A-Z]. Got: ";
        if (c == nullptr) { err += "EOF."; } 
        else { err += std::to_string(*c) + "."; }
        throwException(numExceptions::Incomplete_IDENTIFIER_LexException, err);
    }
    skip_whitespace();
}

// [NEW];
Result Lexer::tokenize_factor() {
// void Lexer::tokenize_factor() {
    Result x;
    // factor = [ident] | [number] | ["(" expr ")"] | funcCall
    #ifdef DEBUG    
        std::cout << "in tokenize_factor(str=[" << this->source.substr(s_index) << "]); buff=[" << this->buff << "]" << std::endl;
    #endif
    char *c;
    // std::string buff;


    // get the first [whatever]
    if ((c = next()) == nullptr) {
        std::string err = "";
        #ifdef DEBUG
            err += "(tokenize_factor(), line=91) ";
        #endif
        err += "FACTOR expected `[ident] | [number] | ['(' expr ')']`. Got: EOF.";
        throwException(numExceptions::Incomplete_FACTOR_LexException, err);
    }

    // case: ["(" expression ")"]
    if (*c == '(') {
        // [1]: open paren
        this->buff.push_back(consume());
        tokens.push_back(TOKEN{ TOKEN_TYPE::OPEN_PAREN, this->buff });
        this->buff.clear();
        
        // [2]: expr
        #ifdef TOKENS
            tokenize_expr();
        #endif
        x = tokenize_expr();
        #ifdef DEBUG
            std::cout << "\treturned from tokenize_expr() in tokenize_factor(str=[" << this->source.substr(s_index) << "]); buff=[" << this->buff << "]" << std::endl;
        #endif

        // [3]: close paren
        if ((c = next()) != nullptr) {
            if (*c == ')') {
                this->buff.push_back(consume());
                tokens.push_back(TOKEN{ TOKEN_TYPE::CLOSE_PAREN, this->buff });
                this->buff.clear();
            } else {
                std::string err = "FACTOR expected `)`. Got: " + std::to_string(*c) + ".";
                throwException(numExceptions::Incomplete_FACTOR_LexException, err);
            }
        } else {
            std::string err = "FACTOR expected `)`. Got: EOF.";
            throwException(numExceptions::Incomplete_FACTOR_LexException, err);
        }
    } else {
        // check if it's [ident] | [number]
        if ((c = next()) == nullptr) {
            std::string err = "FACTOR expected `[ident] | [number]`. Got: EOF.";
            throwException(numExceptions::Incomplete_FACTOR_LexException, err);
        }
        if (std::isalpha(*c)) { // ident
            this->buff.push_back(consume());

            while ((c = next()) != nullptr && std::isalnum(*c)) {
                this->buff.push_back(consume());
            }
            // insert token into [tokens]
            #ifdef DEBUG
                std::cout << "\tpushing back token of type: " << TOKEN_TYPE_toString(TOKEN_TYPE::IDENTIFIER) << std::endl;
            #endif
            // tokens.push_back(TOKEN{ TOKEN_TYPE::IDENTIFIER, this->buff });
            /* Result: if it's a identifier we can treat it as a variable */
            x.kind = r_type::VARIABLE;
            // x.data.address = Lookup(Tokenizer_id); [Question: why does prof do this? what is lookup()? what is tokenizer_id?]
            x.data.address = 0; // temp assign to 0 atm [THIS SHOULD NOT HAPPEN LMFAO]
            this->buff.clear();
        } else if (std::isdigit(*c)) { // number
            this->buff.push_back(consume());

            while ((c = next()) != nullptr && std::isdigit(*c)) {
                this->buff.push_back(consume());
            }
            if ((c != nullptr) && (std::isalpha(*c))) {
                std::string err = "FACTOR expected `[number]`. Got: " + this->buff + std::to_string(*c);
                throwException(numExceptions::Incomplete_FACTOR_LexException, err);
            }
            // insert token into [tokens]
            #ifdef DEBUG
                std::cout << "\tpushing back token of type: " << TOKEN_TYPE_toString(TOKEN_TYPE::NUMBER) << std::endl;
            #endif
            // tokens.push_back(TOKEN{ TOKEN_TYPE::NUMBER, this->buff });
            /* Result: if it's a number we can treat it as a const */
            x.kind = r_type::CONSTANT;
            x.data.value = std::stoi(this->buff);
            this->buff.clear();
        } else {
            std::string err = "";
            err += "FACTOR expected `[ident] | [number]`. Got: EOF.";
            throwException(numExceptions::Incomplete_FACTOR_LexException, err);
        }
    }
    skip_whitespace();
    return x;
}

Result Lexer::tokenize_term() {
// void Lexer::tokenize_term() {
    Result x;
    #ifdef DEBUG    
        std::cout << "in tokenize_term(str=[" << this->source.substr(s_index) << "]); buff=[" << this->buff << "]" << std::endl;
    #endif
    char *c;

    // [1]: factor1
    skip_whitespace(); // [default]: auto in case there is whitespace
    x = tokenize_factor();
    #ifdef DEBUG
        std::cout << "\treturned from tokenize_factor() in tokenize_term(str=[" << this->source.substr(s_index) << "]); buff=[" << this->buff << "]" << std::endl;
    #endif

    while (((c = next()) != nullptr) && ((*c == '*') || (*c == '/'))) {
        Result y;
        // [2]: op
        TOKEN_TYPE tmp;
        bool has_next_factor;
        if (*c == '*') {
            has_next_factor = true;
            // insert token into [tokens]
            this->buff.push_back(consume());
            tmp = TOKEN_TYPE::MULTIPLY;
        }
        else if (*c == '/') {
            has_next_factor = true;
            // insert token into [tokens]
            this->buff.push_back(consume());
            tmp = TOKEN_TYPE::DIVIDE;
        }

        // [3*] assuming has (optional next term)
        #ifdef DEBUG
            std::cout << "\tpushing back token of type: " << TOKEN_TYPE_toString(tmp) << std::endl;
        #endif
        // insert token into [tokens]
        tokens.push_back(TOKEN{ tmp, this->buff });
        this->buff.clear();

        // [3b] factor2
        skip_whitespace();
        y = tokenize_factor();
        #ifdef DEBUG
            std::cout << "\treturned from tokenize_factor() in tokenize_term(str=[" << this->source.substr(s_index) << "]); buff=[" << this->buff << "]" << std::endl;
        #endif

        x = Compute(op, x, y); // tbd


    }
    skip_whitespace();
    return x;
    // END OF FUNCTION;
}

// break down the [expression] into  it's proper tokens and insert into [tokens]
// [NEW];
Result Lexer::tokenize_expr() {
// void Lexer::tokenize_expr() { // checks will be handled later recursively by the factor
    Result x;
    #ifdef DEBUG    
        std::cout << "in tokenize_expr(str=[" << this->source.substr(s_index) << "]); buff=[" << this->buff << "]" << std::endl;
    #endif
    char *c;
    
    // [1]: term1
    skip_whitespace(); // [default]: auto in case there is whitespace
    x = tokenize_term(); // [assumption]: this function handles cleanup
    #ifdef DEBUG
        std::cout << "\treturned from tokenize_term() in tokenize_expr(str=[" << this->source.substr(s_index) << "]); buff=[" << this->buff << "]" << std::endl;
    #endif

    // [intermediate processing step]
    if ((c = next()) == nullptr) {
        // std::string err = "EXPRESSION expected a `+/-` operator. Got: EOF.";
        // throwException(numExceptions::Incomplete_EXPRESSION_LexException, err);
        return; // not the [expr]'s job to handle this exception, so simply propagate it back
    }

    while (((c = next()) != nullptr) && ((*c == '+') || (*c == '-'))) {
        Result y;
        // [2]: op
        TOKEN_TYPE tmp;
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
        
        // [3*] assuming has (optional next term)
        #ifdef DEBUG
            std::cout << "\tpushing back token of type: " << TOKEN_TYPE_toString(tmp) << std::endl;
        #endif
        tokens.push_back(TOKEN{ tmp, this->buff });
        this->buff.clear();

        // [3b] term2
        skip_whitespace();
        y = tokenize_expr();
        #ifdef DEBUG
            std::cout << "\treturned from recursive tokenize_expr() in tokenize_expr(str=[" << this->source.substr(s_index) << "]); buff=[" << this->buff << "]" << std::endl;
        #endif

        x = Compute(op, x, y);

    }

    skip_whitespace();
    return x;
    // END OF FUNCTION;
}

void Lexer::tokenize_returnStatement() {
    #ifdef DEBUG    
        std::cout << "in tokenize_returnStatement(str=[" << this->source.substr(s_index) << "]); buff=[" << this->buff << "]" << std::endl;
    #endif
    // if we do get an expression, 
    // - we're going to handle it in the respective function either way so nothing needs to be done here either 
    try { tokenize_expr(); } 
    catch (const std::exception &err) {}
    // if it threw an exception, 
    // - then we did not have an [expression] (optional arg)
    // we're only catching it so our Lexer can continue to tokenize!
    skip_whitespace();
}

void Lexer::tokenize_relation() {
    #ifdef DEBUG    
        std::cout << "in tokenize_relationStatement(str=[" << this->source.substr(s_index) << "]); buff=[" << this->buff << "]" << std::endl;
    #endif
    tokenize_expr();
    skip_whitespace();


    char *c;
    while (((c = next()) != nullptr) && (*c != ' ')) { this->buff.push_back(consume()); }
    this->tokens.push_back(TOKEN{ TOKEN_TYPE::REL_OP, this->buff });
    this->mostRecentTokenType = TOKEN_TYPE::REL_OP;
    this->buff.clear();
    #ifdef DEBUG    
        std::cout << "\tpushing back token of type: " << TOKEN_TYPE_toString(TOKEN_TYPE::REL_OP) << std::endl;
    #endif


    if (c == nullptr) {
        std::string err = "relationStatement Expected another `EXPRESSION`. Got: EOF.";
        throwException(numExceptions::Incomplete_relation_LexException, err);
    }
    skip_whitespace();
    tokenize_expr();
    skip_whitespace();
    #ifdef DEBUG    
        std::cout << "\tdone tokenize_relation() returning;" << std::endl;
    #endif
}

void Lexer::tokenize_whileStatement() {
    #ifdef DEBUG    
        std::cout << "in tokenize_whileStatement(str=[" << this->source.substr(s_index) << "]); buff=[" << this->buff << "]" << std::endl;
    #endif
    tokenize_relation(); // should return on [whitespace] (i guess??)
    skip_whitespace();
    this->buff = get_deterministic_token(2); // [DO]
    if (this->buff == "") {
        std::string err = "whileStatement Expected `DO`. Got: EOF.";
        throwException(numExceptions::Incomplete_whileStatement_LexException, err);
    }
    if (this->buff.compare(TOKEN_TYPE_toStringLower(TOKEN_TYPE::DO)) != 0) {
        std::string err = "whileStatement Expected `DO`. Got: " + this->buff + ".";
        throwException(numExceptions::Incomplete_whileStatement_LexException, err);
    }

    this->tokens.push_back(TOKEN{ TOKEN_TYPE::DO, this->buff });
    this->mostRecentTokenType = TOKEN_TYPE::DO;
    this->buff.clear();

    skip_whitespace();
    tokenize_statSequence();
    skip_whitespace();

    this->buff = get_deterministic_token(2); // [OD]
    if (this->buff == "") {
        std::string err = "whileStatement Expected `OD`. Got: EOF.";
        throwException(numExceptions::Incomplete_whileStatement_LexException, err);
    }
    if (this->buff.compare(TOKEN_TYPE_toStringLower(TOKEN_TYPE::OD)) != 0) {
        std::string err = "whileStatement Expected `OD`. Got: " + this->buff + ".";
        throwException(numExceptions::Incomplete_whileStatement_LexException, err);
    }

    this->tokens.push_back(TOKEN{ TOKEN_TYPE::OD, this->buff });
    this->mostRecentTokenType = TOKEN_TYPE::OD;
    this->buff.clear();
}


void Lexer::tokenize_ifStatement() {
    #ifdef DEBUG    
        std::cout << "in tokenize_ifStatement(str=[" << this->source.substr(s_index) << "]); buff=[" << this->buff << "]" << std::endl;
    #endif
    tokenize_relation(); // should return on [whitespace] (i guess??)
    skip_whitespace();
    
    char *c;
    this->buff = get_deterministic_token(4); // [THEN]
    #ifdef DEBUG    
        std::cout << "\treturned from tokenize_relation(); in tokenize_ifStatement(str=[" << this->source.substr(s_index) << "]); buff=[" << this->buff << "]" << std::endl;
    #endif
    if (this->buff == "") {
        std::string err = "ifStatement Expected `THEN`. Got: EOF.";
        throwException(numExceptions::Incomplete_ifStatement_LexException, err);
    }
    if (this->buff.compare(TOKEN_TYPE_toStringLower(TOKEN_TYPE::THEN)) != 0) {
        std::string err = "ifStatement Expected `THEN`. Got: " + this->buff + ".";
        throwException(numExceptions::Incomplete_ifStatement_LexException, err);
    }

    this->tokens.push_back(TOKEN{ TOKEN_TYPE::THEN, this->buff });
    this->mostRecentTokenType = TOKEN_TYPE::THEN;
    this->buff.clear();
    #ifdef DEBUG    
        std::cout << "\tpushing back token of type: " << TOKEN_TYPE_toString(TOKEN_TYPE::FI) << std::endl;
    #endif

    skip_whitespace();
    tokenize_statSequence();

    #ifdef DEBUG    
        std::cout << "\tdone tokenize_statSequence() back in tokenize_ifStatement()" << std::endl;
    #endif

    while (((c = next()) != nullptr) && (std::isalpha(*c))) {
        this->buff.push_back(std::tolower(consume()));
    }

    #ifdef DEBUG    
        std::cout << "\tnext token is (from buff): " << this->buff << std::endl;
    #endif

    
    if (this->buff.compare(TOKEN_TYPE_toStringLower(TOKEN_TYPE::ELSE)) == 0) {
        #ifdef DEBUG    
            std::cout << "\tgot ELSE: in tokenize_ifStatement(str=[" << this->source.substr(s_index) << "]); buff=[" << this->buff << "]" << std::endl;
        #endif
        this->tokens.push_back(TOKEN{ TOKEN_TYPE::ELSE, this->buff });
        this->mostRecentTokenType = TOKEN_TYPE::ELSE;
        this->buff.clear();
        #ifdef DEBUG    
            std::cout << "\tpushing back token of type: " << TOKEN_TYPE_toString(TOKEN_TYPE::ELSE) << std::endl;
        #endif

        skip_whitespace();
        tokenize_statSequence(); // i guess we can say they end with a space

        while (((c = next()) != nullptr) && (std::isalpha(*c))) {
            this->buff.push_back(std::tolower(consume()));
        } // expecting [FI]
    }


    if (this->buff.compare(TOKEN_TYPE_toStringLower(TOKEN_TYPE::FI)) == 0) {
        #ifdef DEBUG    
            std::cout << "\tgot FI: in tokenize_ifStatement(str=[" << this->source.substr(s_index) << "]); buff=[" << this->buff << "]" << std::endl;
        #endif
        this->tokens.push_back(TOKEN{ TOKEN_TYPE::FI, this->buff });
        this->mostRecentTokenType = TOKEN_TYPE::FI;
        this->buff.clear();
        #ifdef DEBUG    
            std::cout << "\tpushing back token of type: " << TOKEN_TYPE_toString(TOKEN_TYPE::FI) << std::endl;
        #endif
        // we've successfully tokenized the [ifStatement]
    } else {
        std::string err = "End ifStatement Expected `FI`. Got: " + this->buff + ".";
        throwException(numExceptions::Incomplete_ifStatement_LexException, err);
    }
    skip_whitespace();
}

void Lexer::tokenize_assignment() { // already got first token "let"
    #ifdef DEBUG    
        std::cout << "in tokenize_assignment(str=[" << this->source.substr(s_index) << "]); buff=[" << this->buff << "]" << std::endl;
    #endif
    tokenize_ident(); // should return on `<-` (assignment)

    char *c;
    if ((c = next()) == nullptr) {
        std::string err = "Assignemnt Expected `<-`. Got: EOF.";
        throwException(numExceptions::Incomplete_ASSIGNMENT_LexException, err);
    } 
    if (*(c = next()) == '<') {
        this->buff.push_back(consume());
        if (*(c = next()) == '-') {
            #ifdef DEBUG    
                std::cout << "\tpushing back token of type: " << TOKEN_TYPE_toString(TOKEN_TYPE::ASSIGNMENT) << std::endl;
            #endif
            this->buff.push_back(consume());
            this->tokens.push_back(TOKEN{ TOKEN_TYPE::ASSIGNMENT, this->buff });
            this->mostRecentTokenType = TOKEN_TYPE::ASSIGNMENT;
            this->buff.clear();
        } else {
            std::string err = "Assignemnt Expected `<-`. Got: [" + std::to_string(*c) + "].";
            throwException(numExceptions::Incomplete_ASSIGNMENT_LexException, err);
        }
    } else {
        std::string err = "Assignemnt Expected `<-`. Got: [" + std::to_string(*c) + "].";
        throwException(numExceptions::Incomplete_ASSIGNMENT_LexException, err);
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
        while ((((c = next()) != nullptr)) && (*c != ' ')) {
            if (std::isalpha(*c)) { this->buff.push_back(std::tolower(consume())); } 
        } 

        
        if (*c == ' ') {
            skip_whitespace();
            // we should have a token in the buffer now
            if (buff.compare(TOKEN_TYPE_toStringLower(TOKEN_TYPE::LET)) == 0) {
                this->tokens.push_back(TOKEN{ TOKEN_TYPE::LET, this->buff });
                this->mostRecentTokenType = TOKEN_TYPE::LET;
                this->buff.clear();
                tokenize_assignment();
            } else if (buff.compare(TOKEN_TYPE_toStringLower(TOKEN_TYPE::IF)) == 0) {
                this->tokens.push_back(TOKEN{ TOKEN_TYPE::IF, this->buff });
                this->mostRecentTokenType = TOKEN_TYPE::IF;
                this->buff.clear();
                tokenize_ifStatement();
            } else if (buff.compare(TOKEN_TYPE_toStringLower(TOKEN_TYPE::WHILE)) == 0) {
                this->tokens.push_back(TOKEN{ TOKEN_TYPE::WHILE, this->buff });
                this->mostRecentTokenType = TOKEN_TYPE::WHILE;
                this->buff.clear();
                tokenize_whileStatement();
            } else if (buff.compare(TOKEN_TYPE_toStringLower(TOKEN_TYPE::RETURN)) == 0) {
                this->tokens.push_back(TOKEN{ TOKEN_TYPE::RETURN, this->buff });
                this->mostRecentTokenType = TOKEN_TYPE::RETURN;
                this->buff.clear();
                tokenize_returnStatement();
            } else {
                std::string err = "Invalid statement! Expected { assignment | funcCall3 | ifStatement | whileStatement | returnStatement }. Got: " + this->buff + ".";
                throwException(numExceptions::Incomplete_statement_LexException, err);
            }
        }
        if (c == nullptr) {
            std::string err = "End of statement Expected `;`. Got: " + std::to_string(*c) + ".";
            throwException(numExceptions::Incomplete_statement_LexException, err);
        }
        skip_whitespace();
    } else if (*c != '}') {
        // end of statement [ `;` | `}` ]
        // - let statSequence() handle [end of statement]
        // - assume [this->buff] is currently empty
        // - throw error if we don't encounter this
        std::string err = "Statement Expected to start with [letter]. Got: " + std::to_string(*c);
        err += ".";
        throwException(numExceptions::Incomplete_statement_LexException, err);
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
    char *c;

    skip_whitespace();
    tokenize_statement(); // should return on ";" (optional for terminating); we're going to push a ';' on termination regardless (for easy parsing later)
    #ifdef DEBUG
        std::cout << "after tokenize_statement() in tokenize_statSequence(str=[" << this->source.substr(s_index) << "]); buff=[" << this->buff << "]" << std::endl;
    #endif

    
    if (((c = next()) != nullptr)) {
        if (*c == ';') {
            #ifdef DEBUG
                std::cout << "\tpushing back token of type: " << TOKEN_TYPE_toString(TOKEN_TYPE::SEMICOLON) << std::endl;
            #endif
            this->buff.push_back(consume());
            this->tokens.push_back(TOKEN{ TOKEN_TYPE::SEMICOLON, this->buff });
            this->mostRecentTokenType = TOKEN_TYPE::SEMICOLON;
            this->buff.clear();
            
            #ifdef DEBUG
                std::cout << "\tpeeking 2 after pushing back to check for [fi || od]" << std::endl;
            #endif
            
            skip_whitespace();
            this->buff = peek_deterministic_token(2); // note this is just a peek, we must consume it
            // means that we're done with the current statSequence()
            if ((this->buff.compare("fi") == 0) || (this->buff.compare("od") == 0) || (this->buff.compare("el") == 0)) { 
                #ifdef DEBUG
                    std::cout << "\tDone tokenize_statSequence(); found [fi || od || el(se)]; returning" << std::endl;
                #endif
                if (this->buff.compare("el") == 0) {
                    this->buff = peek_deterministic_token(4);
                    if (this->buff.compare("else") == 0) { 
                        if (this->mostRecentTokenType != TOKEN_TYPE::SEMICOLON) {
                            this->buff = ";";
                            this->tokens.push_back(TOKEN{ TOKEN_TYPE::SEMICOLON, this->buff });
                        }
                        this->buff.clear(); // clearing in case we assume buff has consumed this token
                        return; 
                    }
                } else {
                    if (this->mostRecentTokenType != TOKEN_TYPE::SEMICOLON) {
                        this->buff = ";";
                        this->tokens.push_back(TOKEN{ TOKEN_TYPE::SEMICOLON, this->buff });
                    }
                    this->buff.clear(); // clearing in case we assume buff has consumed this token
                    return; 
                }
            }
            this->buff.clear();
            #ifdef DEBUG
                std::cout << "\trecursively calling tokenize_statSequence()" << std::endl;
            #endif
            return tokenize_statSequence();
        } else if (*c == '}') {
            #ifdef DEBUG
                std::cout << "\t found '}'; about to exit: in tokenize_statSequence(str=[" << this->source.substr(s_index) << "]); buff=[" << this->buff << "]" << std::endl;
            #endif
            // if (*c == '}'): indicates end of [main] [computation] (we let [tokenize()] handle this)
                // if we get another char that isn't `;`, we can assume that the most recent statement was the last statement in this statSequence (since it may not necessarily return a ;)
            
            /* Removed: DETERMINISTIC statSeq END: adding `;` */
            if (this->mostRecentTokenType != TOKEN_TYPE::SEMICOLON) { // iff the last token was not a `;` then we should deterministically add it
                this->buff.push_back(';'); // let's make this a deterministic situation for our tokens (i.e. add the `;` if it doesn't exist)
                #ifdef DEBUG
                    std::cout << "\tpushing back token of type: " << TOKEN_TYPE_toString(TOKEN_TYPE::SEMICOLON) << std::endl;
                #endif
                this->tokens.push_back(TOKEN{ TOKEN_TYPE::SEMICOLON, this->buff });
                this->mostRecentTokenType = TOKEN_TYPE::SEMICOLON;
                this->buff.clear();
            }
            skip_whitespace();
            return; // no need to continue tokenizing "statements" if we've already reached the end
        } else if (std::isalnum(*c)) {
            #ifdef DEBUG
                std::cout << "\tnext char is alnum" << std::endl;
            #endif
            this->buff = peek_deterministic_token(2);
            // means that we're done with the current statSequence()
            if ((this->buff.compare("fi") == 0) || (this->buff.compare("od") == 0) || (this->buff.compare("el") == 0)) { 
                #ifdef DEBUG
                    std::cout << "\tDone tokenize_statSequence(); found [fi || od || el(se)]; returning" << std::endl;
                #endif
                if (this->buff.compare("el") == 0) {
                    this->buff = peek_deterministic_token(4);
                    if (this->buff.compare("else") == 0) {
                        if (this->mostRecentTokenType != TOKEN_TYPE::SEMICOLON) { 
                            this->buff = ";";
                            this->tokens.push_back(TOKEN{ TOKEN_TYPE::SEMICOLON, this->buff });
                        }
                        this->buff.clear(); // clearing in case we assume buff has consumed this token
                        return; 
                    }
                } else {
                    if (this->mostRecentTokenType != TOKEN_TYPE::SEMICOLON) {
                        this->buff = ";";
                        this->tokens.push_back(TOKEN{ TOKEN_TYPE::SEMICOLON, this->buff });
                    }
                    this->buff.clear(); // clearing in case we assume buff has consumed this token
                    return; 
                }
            }
            this->buff.clear();
            #ifdef DEBUG
                std::cout << "\trecursively calling tokenize_statSequence()" << std::endl;
            #endif
            return tokenize_statSequence();
        }
    } else { // [c = next() == nullptr]
        std::string err = "End of statSequence Expected `}`. Got: EOF.";
        throwException(numExceptions::Incomplete_statSequence_LexException, err);
    }
    skip_whitespace();
}

// TODO;
void Lexer::tokenize_varDecl() {
    #ifdef DEBUG    
        std::cout << "in tokenize_varDecl(str=[" << this->source.substr(s_index) << "]); buff=[" << this->buff << "]" << std::endl;
    #endif
    tokenize_ident();

    char *c = next();
    if (c != nullptr) {
        if (*c == ',') {
            this->buff.push_back(consume());
            this->tokens.push_back(TOKEN{ TOKEN_TYPE::COMMA, this->buff });
            this->mostRecentTokenType = TOKEN_TYPE::COMMA;
            this->buff.clear();
            
            skip_whitespace();
            tokenize_varDecl();
        } else if (*c == ';') { // done w varDecl
            #ifdef DEBUG    
                std::cout << "\tgot all varDecl's, exiting tokenize_varDecl()" << std::endl;
            #endif
            this->buff.push_back(consume());
            this->tokens.push_back(TOKEN{ TOKEN_TYPE::SEMICOLON, this->buff });
            this->mostRecentTokenType = TOKEN_TYPE::SEMICOLON;
            this->buff.clear();

            skip_whitespace();
        }
    } else {
        std::string err = "varDecl Expected [`,` || `;`]; Got: EOF.";
        throwException(numExceptions::Incomplete_VAR_LexException, err);
    }
}

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
    skip_whitespace();
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
        std::string err = "tiny: Expected startKeyword = [main] . Got: " + this->buff + ".";
        throwException(numExceptions::Incomplete_MAIN_LexException, err);
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
            // insert [var] token into [tokens]
            #ifdef DEBUG
                std::cout << "pushing back token of type: " << TOKEN_TYPE_toString(TOKEN_TYPE::VAR) << std::endl;
            #endif
            this->tokens.push_back(TOKEN{ TOKEN_TYPE::VAR, this->buff });
            this->mostRecentTokenType = TOKEN_TYPE::VAR;
            this->buff.clear();
            skip_whitespace();
            tokenize_varDecl();
        } else if (this->buff.compare("void") == 0 || this->buff.compare("VOID") == 0 ||
                   this->buff.compare("function") == 0 || this->buff.compare("FUNCTION") == 0) {
            // should we push a [void] (type) token before? (thinking abt it idk; need to learn more abt functions first)
            // insert [function] token into [tokens]
            #ifdef DEBUG
                std::cout << "pushing back token of type: " << TOKEN_TYPE_toString(TOKEN_TYPE::FUNCTION) << std::endl;
            #endif
            this->tokens.push_back(TOKEN{ TOKEN_TYPE::FUNCTION, this->buff });
            this->mostRecentTokenType = TOKEN_TYPE::FUNCTION;
            this->buff.clear();
            skip_whitespace();
            tokenize_func();
        }
        if (c == nullptr) {
            // indicates EOF
            std::string err;
            if (c == nullptr) { err = "Main Expected [`{` || `var decl` || `function`]; Got: EOF."; } 
            else { err = "Main Expected [`{` || `var decl` || `function`]; Got: " + std::to_string(*c) + "."; }
            throwException(numExceptions::Incomplete_MAIN_LexException, err);
        }
        skip_whitespace();
    }
    
    c = next();
    #ifdef DEBUG    
        std::cout << "\tin main(str=[" << this->source.substr(s_index) << "]) resume: expected done parsing varDecl & funcDecl; buff=[" << this->buff << "]" << std::endl;
    #endif
    // checking for [statSequence()] and then [END: `.`]
    if (c != nullptr && *c == '{') {
        #ifdef DEBUG
            std::cout << "pushing back token of type: " << TOKEN_TYPE_toString(TOKEN_TYPE::OPEN_CURLY) << std::endl;
        #endif
        this->buff.push_back(consume());
        this->tokens.push_back(TOKEN{ TOKEN_TYPE::OPEN_CURLY, this->buff });
        this->mostRecentTokenType = TOKEN_TYPE::OPEN_CURLY;
        this->buff.clear();
        skip_whitespace();

        tokenize_statSequence(); // should be looking for '}' as indicator to return
        if (*(c = next()) == '}') {
            #ifdef DEBUG
                std::cout << "\tpushing back token of type: " << TOKEN_TYPE_toString(TOKEN_TYPE::CLOSE_CURLY) << std::endl;
            #endif
            // then we know we're done with the [statSequence]
            // - expected next char is last [.]
            this->buff.push_back(consume());
            this->tokens.push_back(TOKEN{ TOKEN_TYPE::CLOSE_CURLY, this->buff });
            this->mostRecentTokenType = TOKEN_TYPE::CLOSE_CURLY;
            this->buff.clear();
            skip_whitespace();

            // [checking for end] include this below too!
            if (((c = next()) != nullptr)  && (*(c = next()) == '.')) {
                #ifdef DEBUG
                    std::cout << "\tpushing back token of type: " << TOKEN_TYPE_toString(TOKEN_TYPE::END_OF_FILE) << std::endl;
                #endif
                this->buff.push_back(consume());
                this->tokens.push_back(TOKEN{ TOKEN_TYPE::END_OF_FILE, this->buff });
                this->mostRecentTokenType = TOKEN_TYPE::END_OF_FILE;
                this->buff.clear();
            } else {
                std::string err = "End of main Expected `.`; Got: ";
                // indicates EOF
                if (c == nullptr) { err += "EOF."; } 
                else { err += std::to_string(*c) + "."; }
                throwException(numExceptions::Incomplete_MAIN_LexException, err);
            }
        } else {
            std::string err = "End of statSequence Expected `.`; Got: ";
            // indicates EOF
            if (c == nullptr) { err += "EOF."; } 
            else { err += std::to_string(*c) + "."; }
            throwException(numExceptions::Incomplete_statSequence_LexException, err);
        }
    } else {
        std::string err = "End of statSequence Expected `some IDENTIFIER`. Got: ";
        // indicates EOF
        if (c == nullptr) { err += "EOF."; } 
        else { err += this->buff + " & " + std::to_string(*c) + "."; }
        throwException(numExceptions::Incomplete_statSequence_LexException, err);
    }
    this->s_index = 0; // default behavior: reset to 0; should be done tokenizing now
}