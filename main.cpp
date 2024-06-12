#include <iostream>
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>

#include "src/FileReader.hpp"
#include "src/Tokenizer.hpp"
// #include "src/Lexer.hpp"

// std::string get_file_contents(const std::string &f) {
//     // Open a file for reading
//     std::ifstream file(f);

//     // Check if the file was opened successfully
//     if (!file.is_open()) {
//         std::cerr << "Failed to open the file." << std::endl;
//         return ""; // Return empty str to indicate failure
//     }

//     // Read data from the file using std::istream
//     std::string line, buff = "";
//     while (std::getline(file, line)) {
//         // std::cout << line << std::endl; // Print each line read from the file
//         buff.append(line);
//     }

//     // Close the file
//     file.close();
//     return buff;
// }

// bool write_file_contents(const std::string &f, const std::string& content) {
//     // Open the file for writing
//     std::ofstream file(f);

//     // Check if the file was opened successfully
//     if (!file.is_open()) {
//         std::cerr << "Failed to open the file." << std::endl;
//         return false; // Return false to indicate failure
//     }

//     // Write content to the file
//     file << content;

//     // Close the file
//     file.close();
//     return true; // Return true to indicate success
// }

// input file: [tst/temp.ty]
// input file: [tst/Lexer_results.txt]
int main() {
    const char *in_f = "tst/temp.ty";
    const std::string out_f = "tst/Lexer_results.txt";

    FileReader fr = FileReader(in_f);
    fr.read_file();
    std::string contents = fr.get_inFile_contents();
    // #ifdef DEBUG
    //     std::cout << "done reading file contents. got:\n" << contents << std::endl;
    // #endif

    // Tokenizer will be here
    Tokenizer tokenizer = Tokenizer(contents);
    #ifdef DEBUG
        std::cout << "after tokenizer initializer, [this->sym]=[" << tokenizer.sym << "]" << std::endl;
    #endif

    std::vector<int> tokens = tokenizer.tokenize();
    #ifdef DEBUG
        std::cout << "after tokenizer tokenize(), [tokens] size = [" << tokens.size() << "]" << std::endl;
    #endif

    // Lexer lexer = Lexer(contents);
    // std::vector<TOKEN> tokens = lexer.lex();

    std::string out_str = "Syntax;\n[TOKEN_TYPE::DIGIT]: \t[STRING]\n";
    // // for (auto iit = tokens.begin(); iit != tokens.end(); iit++) {
    // //     std::string tmp;
    // //     if (TOKEN_TYPE_toString(iit->type).length() <= 3) { tmp = "[" + TOKEN_TYPE_toString(iit->type) + "]: \t\t\t[" + iit->lexeme + "]\n"; }
    // //     else if (TOKEN_TYPE_toString(iit->type).length() <= 6) { tmp = "[" + TOKEN_TYPE_toString(iit->type) + "]: \t\t[" + iit->lexeme + "]\n"; }
    // //     else { tmp = "[" + TOKEN_TYPE_toString(iit->type) + "]: \t[" + iit->lexeme + "]\n"; }
    // //     out_str += tmp;
    // // }
    for (int token : tokens) {
        std::string tmp;
        // Access each integer token here using the variable 'token'
        // For example:
        std::string val = "";
        for (const auto& pair : tokenizer.get_sym_table()) {
            if (pair.second == token) {
                val = pair.first;
                break;
            }
        }
        if (val == "") {
            // digit
            tmp = "[DIGIT::NULL]: \t\t\t[" + std::to_string(token) + "]\n";
        } else {
            if (token < tokenizer.keyword_identifier_separator) {
                // keyword
                tmp = "[KEYWORD::" + std::to_string(token) + "]: \t\t\t[" + val + "]\n";
            } else {
                // identifier
                tmp = "[IDENTIFIER::" + std::to_string(token) + "]: \t\t[" + val + "]\n";
            }
        }
        out_str += tmp;
    }

    bool res = fr.write_file_contents(out_f, out_str);
    if (res) { std::cout << "Results have successfully been written to: " << out_f << std::endl; }
    else { std::cout << "Error occured when trying to write results!" << std::endl; }

    return 0;
}