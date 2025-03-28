#include <iostream>
#include <vector>

// #include "tinyExceptions.hpp"
#include "Tokenizer.hpp"


// using namespace tinyExceptions_ns;

std::vector<int> Tokenizer::tokenize() {
    // #ifdef DEBUG
    //     std::cout << "in tokenize(this->sym=[" << this->sym << "])" << std::endl;
    // #endif
    int retVal = GetNext();
    #ifdef DEBUG
        std::cout << "\tGetNext() returned [" << retVal << "]";
    #endif
    while (retVal != this->get_eof_val()) {
        #ifdef DEBUG
            std::cout << "pushing back val of [" << retVal << "]" << std::endl;
        #endif
        this->tokens.push_back(retVal);
        retVal = GetNext();
        #ifdef DEBUG
            std::cout << "\tGetNext() returned [" << retVal << "]" << std::endl;
        #endif
    }
    this->tokens.push_back(retVal); // push back the EOF `.` val
    #ifdef DEBUG_RESULTS
        std::cout << "\tdone tokenize(); " << std::endl;
        SymbolTable::print_table();
        std::cout << "[Identifiers];" << std::endl;
        for (const int &it: SymbolTable::identifiers) {
            std::string val;
            for (const auto &iit: SymbolTable::symbol_table) {
                if (iit.second == it) {
                    val = iit.first;
                    break;
                }
            }
            std::cout << "id=[" << it << "]: [" << val << "]" << std::endl;
        }
    #endif
    return this->tokens;
}

// include functions here
int Tokenizer::GetNext() {
    // #ifdef DEBUG
    //     std::cout << "in GetNext(this->sym=[" << this->sym << "])" << std::endl;
    // #endif
    while (this->whitespace_symbols.find(this->sym) != this->whitespace_symbols.end()) { // skip whitespace
        next();
        // #ifdef DEBUG
        //     std::cout << "\tGetNext(WHILE): [this->sym]=[" << this->sym << "]" << std::endl;
        // #endif
    }
    #ifdef DEBUG
        std::cout << "\tafter skipping whitespace [this->sym] curr has val [" << this->sym << "]" << std::endl;
    #endif

    if (this->sym != EOF) {
        std::string str(1, this->sym);
        // #ifdef DEBUG
        //     std::cout << "current symbol: [" << this->sym << "] != EOF! string val looks like: [" << str << "]" << std::endl;
        // #endif
        switch(this->sym) {
            case '*':
                next();
                this->token = SymbolTable::keywords.at(str);
                return this->token;
            case '/':
                next();
                this->token = SymbolTable::keywords.at(str);
                return this->token;
            case '+':
                next();
                this->token = SymbolTable::keywords.at(str);
                return this->token;
            case '-':
                next();
                this->token = SymbolTable::keywords.at(str);
                return this->token;
            case '{':
                next();
                this->token = SymbolTable::keywords.at(str);
                return this->token;
            case '}':
                next();
                this->token = SymbolTable::keywords.at(str);
                return this->token;
            case '(':
                next();
                this->token = SymbolTable::keywords.at(str);
                return this->token;
            case ')':
                next();
                this->token = SymbolTable::keywords.at(str);
                return this->token;
            case ';':
                next();
                this->token = SymbolTable::keywords.at(str);
                return this->token;
            case '.':
                next();
                this->token = SymbolTable::keywords.at(str);
                return this->token;
            case '0':
                number();
                return this->token;
            case '1':
                number();
                return this->token;
            case '2':
                number();
                return this->token;
            case '3':
                number();
                return this->token;
            case '4':
                number();
                return this->token;
            case '5':
                number();
                return this->token;
            case '6':
                number();
                return this->token;
            case '7':
                number();
                return this->token;
            case '8':
                number();
                return this->token;
            case '9':
                number();
                return this->token;
            case 'A':
                identkeyword();
                return this->token;
            case 'B':
                identkeyword();
                return this->token;
            case 'C':
                identkeyword();
                return this->token;
            case 'D':
                identkeyword();
                return this->token;
            case 'E':
                identkeyword();
                return this->token;
            case 'F':
                identkeyword();
                return this->token;
            case 'G':
                identkeyword();
                return this->token;
            case 'H':
                identkeyword();
                return this->token;
            case 'I':
                identkeyword();
                return this->token;
            case 'J':
                identkeyword();
                return this->token;
            case 'K':
                identkeyword();
                return this->token;
            case 'L':
                identkeyword();
                return this->token;
            case 'M':
                identkeyword();
                return this->token;
            case 'N':
                identkeyword();
                return this->token;
            case 'O':
                identkeyword();
                return this->token;
            case 'P':
                identkeyword();
                return this->token;
            case 'Q':
                identkeyword();
                return this->token;
            case 'R':
                identkeyword();
                return this->token;
            case 'S':
                identkeyword();
                return this->token;
            case 'T':
                identkeyword();
                return this->token;
            case 'U':
                identkeyword();
                return this->token;
            case 'V':
                identkeyword();
                return this->token;
            case 'W':
                identkeyword();
                return this->token;
            case 'X':
                identkeyword();
                return this->token;
            case 'Y':
                identkeyword();
                return this->token;
            case 'Z':
                identkeyword();
                return this->token;
            case 'a':
                identkeyword();
                return this->token;
            case 'b':
                identkeyword();
                return this->token;
            case 'c':
                identkeyword();
                return this->token;
            case 'd':
                identkeyword();
                return this->token;
            case 'e':
                identkeyword();
                return this->token;
            case 'f':
                identkeyword();
                return this->token;
            case 'g':
                identkeyword();
                return this->token;
            case 'h':
                identkeyword();
                return this->token;
            case 'i':
                identkeyword();
                return this->token;
            case 'j':
                identkeyword();
                return this->token;
            case 'k':
                identkeyword();
                return this->token;
            case 'l':
                identkeyword();
                return this->token;
            case 'm':
                identkeyword();
                return this->token;
            case 'n':
                identkeyword();
                return this->token;
            case 'o':
                identkeyword();
                return this->token;
            case 'p':
                identkeyword();
                return this->token;
            case 'q':
                identkeyword();
                return this->token;
            case 'r':
                identkeyword();
                return this->token;
            case 's':
                identkeyword();
                return this->token;
            case 't':
                identkeyword();
                return this->token;
            case 'u':
                identkeyword();
                return this->token;
            case 'v':
                identkeyword();
                return this->token;
            case 'w':
                identkeyword();
                return this->token;
            case 'x':
                identkeyword();
                return this->token;
            case 'y':
                identkeyword();
                return this->token;
            case 'z':
                identkeyword();
                return this->token;
            case EOF:
                std::cerr << "GetNext() found EOF)! [EXIT_FAILURE]" << std::endl;
                exit(EXIT_FAILURE);
                break;
            default:
                // Handle other characters if needed
                identkeyword();
                return this->token;
        }
    }
    #ifdef DEBUG
        std::cout << "\tGETNEXT() [this->sym]==EOF!!!" << std::endl;
    #endif
    return get_eof_val();
}

// loads number into [this->token] 
void Tokenizer::number() {
    this->token = this->sym - '0';
    #ifdef DEBUG
        std::cout << "\tin number(this->sym=[" << this->sym << "], this->token=[" << this->token << "])" << std::endl;
    #endif
    next();
    while((this->sym <= '9') && (this->sym >= '0')) {
        this->token = (10 * this->token) + (this->sym - '0');
        #ifdef DEBUG
            std::cout << "\tnext symbol[" << this->sym << "] was also a number! curr token = [" << this->token << "]" << std::endl;
        #endif
        next();
    }
    #ifdef DEBUG
        std::cout << "\t\tdone number(token=" << this->token << ")" << std::endl;
    #endif

    // this is the first time we see this number
    if (SymbolTable::numbers.find(std::to_string(this->token)) == SymbolTable::numbers.end()) {
        SymbolTable::update_table(std::to_string(this->token), "number");
    }
    this->token = SymbolTable::numbers.at(std::to_string(this->token));
}

// ToDo a1: would be revising here
void Tokenizer::identkeyword() {
    // could be an identifier (can have num or char after first char)
    if (std::isalpha(this->sym)) {
        std::string buff = "";
        buff.push_back(this->sym);
        next();
        while (std::isalnum(this->sym)) { // identifiers only need to START with an alpha
            buff.push_back(this->sym);
            next();
        }
        auto it = SymbolTable::keywords.find(buff);
        if (it != SymbolTable::keywords.end()) {
            #ifdef DEBUG
                std::cout << "\t\t\tin identkeyword(buff=[" << buff << "], found val in [SymbolTable::identifiers]=[" << it->second << "])";
            #endif
            this->token = it->second;
        } else { // if it's not in a [keyword], it will be an identifier: 1) check if alr exists, if so use that value, else add it into [identifier] map 
            it = SymbolTable::identifiers.find(buff);
            if (it != SymbolTable::identifiers.end()) {
                this->token = it->second;
            } else {
                #ifdef DEBUG
                    std::cout << "\t\t\tin identkeyword(buff=[" << buff << "], adding NEW ENTRY to [SymbolTable::symbol_table] with val=[" << SymbolTable::symbol_table.size() << "])";
                #endif
                SymbolTable::update_table(buff, "identifier");
                this->token = SymbolTable::identifiers.at(buff);
            }
        }
    } else { // check if it's a keyword (it has to be if it cannot be an identifier)
        #ifdef DEBUG
            std::cout << "\t\tin identkeyword() checking if [this->sym]=[" << this->sym << "] is a terminal sym or not!" << std::endl;
        #endif
        // could be a terminal symbol
        std::string buff = "";
        buff.push_back(this->sym);
        next();
        #ifdef DEBUG
            std::cout << "\t\t\tnext char is [" << this->sym << "]" << std::endl;
        #endif
        // add chars while it's not a whitespace char
        while ((this->whitespace_symbols.find(this->sym) == this->whitespace_symbols.end()) && !std::isalnum(this->sym)) {
            buff.push_back(this->sym);
            next();
        }
        #ifdef DEBUG
            std::cout << "\tidentkeyword() could be a terminal! got buff=[" << buff << "]" << std::endl;
        #endif
        auto it = SymbolTable::keywords.find(buff);
        if (it != SymbolTable::keywords.end()) {
            this->token = it->second;
        } else { 
            // unknown keyword!
            std::cout << "ERROR: unknown keyword: [" << buff << "]" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    #ifdef DEBUG
        std::cout << "\t\tdone identkeyword(token=" << this->token << ")" << std::endl;
    #endif
}