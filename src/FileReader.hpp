#ifndef FILEREADER_HPP
#define FILEREADER_HPP


#include <stdio.h>
#include <cstdio>


class FileReader {
public:
    FileReader(const char *in_file)
        : file_name(std::move(in_file))
    {
        this->file_contents = "";
    }

    void read_file() {
        // Open a file for reading
        FILE * pFile;
        pFile = std::fopen(this->file_name,"r");

        // Check if the file was opened successfully
        if (pFile == NULL) {
            perror ("Error opening file");
            return;
        }

        // Read data from the file using std::istream
        char c = std::fgetc(pFile);
        while (c != EOF) {
            this->file_contents.push_back(static_cast<char>(c));
            c = std::fgetc(pFile);
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


private:
    const char *file_name;
    std::string file_contents;
};

#endif