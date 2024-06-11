#include <iostream>
#include <vector>

// #include "tinyExceptions.hpp"
#include "Tokenizer.hpp"


// using namespace tinyExceptions_ns;

std::vector<int> Tokenizer::tokenize() {
    int retVal = GetNext();
    while (retVal != get_eof_val()) {
        this->tokens.push_back(retVal);
        retVal = GetNext();
    }
    return this->tokens;
}

// include functions here
int Tokenizer::GetNext() {
    while (*(this->sym) == ' ') { // skip whitespace
        next();
    }

    if (*(this->sym) != EOF) {
        std::string str(1, *(this->sym));
        switch(*(this->sym)) {
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
            default:
                // Handle other characters if needed
                std::cerr << "GetNext() found unknown char(" << *(this->sym) << ")! [EXIT_FAILURE]" << std::endl;
                exit(EXIT_FAILURE);
                break;
        }
    }
    return get_eof_val();
}

// loads number into [this->token] 
void Tokenizer::number() {
    this->token = int(*(this->sym));
    next();
    while((*(this->sym) <= '9') && (*(this->sym) >= '0')) {
        this->token = 10 * int(*(this->sym));
        next();
    }
}


void Tokenizer::identkeyword() {
    std::string buff = "";
    buff.push_back(*(this->sym));
    next();
    while (std::isalpha(*(this->sym))) {
        buff.push_back(*(this->sym));
        next();
    }
    auto it = this->sym_table.find(buff);
    if (it != this->sym_table.end()) {
        this->token = it->second;
    } else {
        this->sym_table.emplace(buff, this->sym_table.size() - 1);
    }
}