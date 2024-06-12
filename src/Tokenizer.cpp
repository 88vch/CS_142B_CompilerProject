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
        #ifdef DEBUG_RESULTS
            std::cout << "pushing back val of [" << retVal << "]" << std::endl;
        #endif
        this->tokens.push_back(retVal);
        retVal = GetNext();
        #ifdef DEBUG
            std::cout << "\tGetNext() returned [" << retVal << "]" << std::endl;
        #endif
    }
    #ifdef DEBUG_RESULTS
        std::cout << "\tdone tokenize(); [this->sym_table] looks like: " << std::endl;
        for (const auto& pair : this->sym_table) {
            std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
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
                return (this->sym_table.find(str))->second;
            case '/':
                next();
                return (this->sym_table.find(str))->second;
            case '+':
                next();
                return (this->sym_table.find(str))->second;
            case '-':
                next();
                return (this->sym_table.find(str))->second;
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
    this->token = int(this->sym);
    next();
    while((this->sym <= '9') && (this->sym >= '0')) {
        this->token = 10 * int(this->sym);
        next();
    }
    #ifdef DEBUG
        std::cout << "\t\tdone number(token=" << this->token << ")" << std::endl;
    #endif
}


void Tokenizer::identkeyword() {
    if (std::isalpha(this->sym)) {
        std::string buff = "";
        buff.push_back(this->sym);
        next();
        while (std::isalpha(this->sym)) {
            buff.push_back(this->sym);
            next();
        }
        if (std::isalnum(this->sym)) {
            // could be an identifier (can have num or char after first char)
            while (std::isalnum(this->sym)) {
                buff.push_back(this->sym);
                next();
            }
        }
        auto it = this->sym_table.find(buff);
        if (it != this->sym_table.end()) {
            #ifdef DEBUG
                std::cout << "\t\t\tin identkeyword(buff=[" << buff << "], found val in [this->sym_table]=[" << it->second << "])";
            #endif
            this->token = it->second;
        } else {
            #ifdef DEBUG
                std::cout << "\t\t\tin identkeyword(buff=[" << buff << "], adding NEW ENTRY to [this->sym_table] with val=[" << this->sym_table.size() << "])";
            #endif
            this->token = this->sym_table.size();
            this->sym_table.emplace(buff, this->sym_table.size());
        }
    } else if (this->sym == EOF) {
        std::string buff = "";
        buff.push_back(this->sym);
        this->token = this->sym_table.find(buff)->second;
    } else {
        #ifdef DEBUG
            std::cout << "\t\tin identkeyword() checking if [this->sym]=[" << this->sym << "] is a terminal sym or not!" << std::endl;
        #endif
        // could be a terminal symbol
        std::string buff = "";
        buff.push_back(this->sym);
        auto it = this->sym_table.find(buff);
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
        it = this->sym_table.find(buff);
        if (it != this->sym_table.end()) {
            this->token = it->second;
        } else {
            #ifdef DEBUG
                std::cout << "\t\tnew symbol added to table with int buff=[" << this->sym_table.size() << "]" << std::endl;
            #endif
            this->token = this->sym_table.size();
            this->sym_table.emplace(buff, this->sym_table.size());
        }
    }
    #ifdef DEBUG
        std::cout << "\t\tdone identkeyword(token=" << this->token << ")" << std::endl;
    #endif
}