#include <iostream>
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>

#include "src/Lexer.hpp"


int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Error: no input file." << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string contents, inf = argv[1];
    {
        std::stringstream file_contents;
        std::fstream input(argv[1], std::ios::in);
        file_contents << input.rdbuf();
        contents = file_contents.str();
    }

    Lexer lexer = Lexer(contents);
    std::vector<TOKEN> tokens = lexer.lex();

    std::cout << "Syntax;" << std::endl << "[TOKEN_TYPE]: \t[STRING]" << std::endl;
    for (auto iit = tokens.begin(); iit != tokens.end(); iit++) {
        std::cout << "[" << TOKEN_TYPE_toString(iit->type) << "]: \t[" << iit->lexeme << "]" << std::endl;
    }


    // {
    //     std::stringstream outfile_contents;
    //     std::fstream
    // }


    return 0;
}