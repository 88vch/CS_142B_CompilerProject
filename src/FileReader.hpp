#ifndef FILEREADER_HPP
#define FILEREADER_HPP


#include <stdio.h>
#include <cstdio>
#include <string>
#include <vector>
#include <unordered_map>

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
        std::cout << tableName << ";" << std::endl << "[INT_VAL]" << std::endl;
        for (int c : content) {
            file << std::to_string(c) << std::endl;
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
        std::cout << tableName << ";" << std::endl << "[KEYWORD/TERMINAL]: \t[INT_VAL]" << std::endl;
        for (const auto &pair : content) {
            if ((pair.first).length() > 3) {
                std::cout << "[" << pair.first << "]" << ": \t\t[" << pair.second << "]" << std::endl;
            } else {
                std::cout << "[" << pair.first << "]" << ": \t\t\t[" << pair.second << "]" << std::endl;
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