#ifndef FILEREADER_HPP
#define FILEREADER_HPP


#include <stdio.h>
#include <cstdio>
#include <string>
#include <vector>
#include <unordered_map>

#include "Result.hpp"
#include "SSA.hpp"

#define DEBUG

class FileReader {
public:
    FileReader(const char *in_file)
        : file_name(std::move(in_file))
    {
        this->file_contents = "";
        #ifdef DEBUG
            std::cout << "[FileReader]: Read input. Got filename: " << this->file_name << std::endl;
        #endif
    }

    void read_file() {
        // Open a file for reading
        FILE * pFile;
        pFile = std::fopen(this->file_name,"r");

        // Check if the file was opened successfully
        if (pFile == NULL) {
            std::cerr << "Error opening file" << std::endl;
            return;
        }

        // Read data from the file using std::istream
        char c = std::fgetc(pFile);
        while (c != EOF) {
            this->file_contents.push_back(static_cast<char>(c));
            c = std::fgetc(pFile); // Read the next character
        }
        
        // Close the file
        std::fclose (pFile);
    }

    std::string get_inFile_contents() {
        return this->file_contents;
    }

    static bool write_file_contents(const std::string &out_f, const std::string& content) {
        // Open the file for writing
        std::ofstream file(out_f);

        // Check if the file was opened successfully
        if (!file.is_open()) {
            std::cerr << "Failed to open the file." << std::endl;
            return false; // Return false to indicate failure
        }

        // Write content to the file
        file << content;

        // Close the file
        file.close();
        return true; // Return true to indicate success
    }

    static bool write_file_contents(const std::string &out_f, const std::vector<int>& content, const std::string &tableName) {
        // Open the file for writing
        std::ofstream file(out_f);

        // Check if the file was opened successfully
        if (!file.is_open()) {
            std::cerr << "Failed to open the file." << std::endl;
            return false; // Return false to indicate failure
        }

        // Write content to the file
        file << tableName << ";" << std::endl << "[INT_VAL]" << std::endl;
        for (int c : content) {
            file << std::to_string(c) << std::endl;
        }

        // Close the file
        file.close();
        return true; // Return true to indicate success
    }

    static bool write_file_contents(const std::string &out_f, const std::unordered_map<int, std::string>& content, const std::string &tableName) {
        // Open the file for writing
        std::ofstream file(out_f);

        // Check if the file was opened successfully
        if (!file.is_open()) {
            std::cerr << "Failed to open the file." << std::endl;
            return false; // Return false to indicate failure
        }

        const int content_size = content.size();

        // Write content to the file
        file << tableName << ";" << std::endl << "[KEYWORD/TERMINAL]: \t[INT_VAL]" << std::endl;
        for (int i = -1; i < content_size - 1; i++) {
            for (const auto &pair : content) {
                if (pair.first != i) { continue; } 
                file << "[" << pair.first << "]" << ": \t\t\t\t\t[" << pair.second << "]" << std::endl;
            }
        }

        // Close the file
        file.close();
        return true; // Return true to indicate success
    }
    
    static bool write_file_contents(const std::string &out_f, const std::unordered_map<std::string, int>& content, const std::string &tableName) {
        // Open the file for writing
        std::ofstream file(out_f);

        // Check if the file was opened successfully
        if (!file.is_open()) {
            std::cerr << "Failed to open the file." << std::endl;
            return false; // Return false to indicate failure
        }

        // Write content to the file
        file << tableName << ";" << std::endl << "[KEYWORD/TERMINAL Literal]: \t[SymbolTable INT_VAL]" << std::endl;
        for (const auto &pair : content) { 
            if ((pair.first).length() > 7) {
                file << "[" << pair.first << "]" << ": \t\t\t\t\t[" << std::to_string(pair.second) << "]" << std::endl;
            } else if ((pair.first).length() > 3) {
                file << "[" << pair.first << "]" << ": \t\t\t\t\t\t[" << std::to_string(pair.second) << "]" << std::endl;
            } else {
                file << "[" << pair.first << "]" << ": \t\t\t\t\t\t\t[" << std::to_string(pair.second) << "]" << std::endl;
            }
        }

        // Close the file
        file.close();
        return true; // Return true to indicate success
    }

    static bool write_file_contents(const std::string &out_f, const std::vector<Result>& content, const std::string &tableName) {
        // Open the file for writing
        std::ofstream file(out_f);

        // Check if the file was opened successfully
        if (!file.is_open()) {
            std::cerr << "Failed to open the file." << std::endl;
            return false; // Return false to indicate failure
        }

        // Write content to the file
        file << tableName << ";" << std::endl << "[KIND]: \t\t\t\t[VALUE]" << std::endl;
        for (const Result &res : content) { 
            std::string kind = res.get_kind(), value = res.get_value();
            if ((kind).length() > 7) {
                file << "[" << kind << "]" << ": \t\t\t[" << value << "]" << std::endl;
            } else if ((kind).length() > 3) {
                file << "[" << kind << "]" << ": \t\t\t\t[" << value << "]" << std::endl;
            } else {
                file << "[" << kind << "]" << ": \t\t\t\t\t[" << value << "]" << std::endl;
            }
        }

        // Close the file
        file.close();
        return true; // Return true to indicate success
    }

    static bool write_file_contents(const std::string &out_f, const std::vector<SSA*>& content, const std::string &tableName) {
        // Open the file for writing
        std::ofstream file(out_f);

        // Check if the file was opened successfully
        if (!file.is_open()) {
            std::cerr << "Failed to open the file." << std::endl;
            return false; // Return false to indicate failure
        }

        // Write content to the file
        file << tableName << ";" << std::endl << "[NUM]: \t`OP`\t | [x_val], \t\t[y_val]" << std::endl;
        for (const SSA* res : content) {
            file << "[" << res->toString() << "]" << std::endl;
        }

        // Close the file
        file.close();
        return true; // Return true to indicate success
    }

    static bool write_file_contents(const std::string &out_f, const std::unordered_map<std::string, SSA*>& content, const std::string &tableName) {
        // Open the file for writing
        std::ofstream file(out_f);

        // Check if the file was opened successfully
        if (!file.is_open()) {
            std::cerr << "Failed to open the file." << std::endl;
            return false; // Return false to indicate failure
        }

        // Write content to the file
        file << tableName << ";" << std::endl << "[KEYWORD/TERMINAL Literal]: \t[SymbolTable INT_VAL]" << std::endl;
        for (const auto &pair : content) { 
            if ((pair.first).length() > 7) {
                file << "[" << pair.first << "]" << ": \t\t\t\t\t[" << pair.second->toString() << "]" << std::endl;
            } else if ((pair.first).length() > 3) {
                file << "[" << pair.first << "]" << ": \t\t\t\t\t\t[" << pair.second->toString() << "]" << std::endl;
            } else {
                file << "[" << pair.first << "]" << ": \t\t\t\t\t\t\t[" << pair.second->toString() << "]" << std::endl;
            }
        }

        // Close the file
        file.close();
        return true; // Return true to indicate success
    }


private:
    const char *file_name;
    std::string file_contents;
};

#endif