#include <iostream>
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>

#include "src/Lexer.hpp"

std::string get_file_contents(std::string f) {
    // Open a file for reading
    std::ifstream file(f);

    // Check if the file was opened successfully
    if (!file.is_open()) {
        std::cerr << "Failed to open the file." << std::endl;
        return ""; // Return empty str to indicate failure
    }

    // Read data from the file using std::istream
    std::string line, buff = "";
    while (std::getline(file, line)) {
        // std::cout << line << std::endl; // Print each line read from the file
        buff.append(line);
    }

    // Close the file
    file.close();
    return buff;
}

// file: [tst/temp.ty]
int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Error: no input file." << std::endl;
        exit(EXIT_FAILURE);
    }
    std::string inf = argv[1];
    std::cout << "Got file: [" << inf << "]" << std::endl;
    std::string contents = get_file_contents(inf);

    Lexer lexer = Lexer(contents);
    std::vector<TOKEN> tokens = lexer.lex();

    std::cout << "Syntax;" << std::endl << "[TOKEN_TYPE]: \t[STRING]" << std::endl;
    for (auto iit = tokens.begin(); iit != tokens.end(); iit++) {
        if (TOKEN_TYPE_toString(iit->type).length() <= 3) { std::cout << "[" << TOKEN_TYPE_toString(iit->type) << "]: \t\t[" << iit->lexeme << "]" << std::endl; }
        else { std::cout << "[" << TOKEN_TYPE_toString(iit->type) << "]: \t[" << iit->lexeme << "]" << std::endl; }
    }


    // {
    //     std::stringstream outfile_contents;
    //     std::fstream
    // }


    return 0;
}