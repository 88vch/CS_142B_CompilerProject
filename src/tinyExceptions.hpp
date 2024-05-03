#ifndef TINY_EXCEPTIONS_HPP
#define TINY_EXCEPTIONS_HPP


#include <iostream>
#include <string>



namespace tinyExceptions {
    namespace Lexer {
        class Incomplete_FACTOR_LexException : public std::exception {
            public:
                Incomplete_FACTOR_LexException(const std::string &err) : msg(err) { }
                // Override the what() function to return the error message
                const char* what() const noexcept override { return msg.c_str(); }
            private:
                std::string msg;
        }; 

        class Incomplete_TERM_LexException : public std::exception {
            public:
                Incomplete_TERM_LexException(const std::string &err) : msg(err) { }
                // Override the what() function to return the error message
                const char* what() const noexcept override { return msg.c_str(); }
            private:
                std::string msg;
        }; 

        class Incomplete_EXPRESSION_LexException : public std::exception {
            public:
                Incomplete_EXPRESSION_LexException(const std::string &err) : msg(err) { }
                // Override the what() function to return the error message
                const char* what() const noexcept override { return msg.c_str(); }
            private:
                std::string msg;
        }; 

        class Incomplete_LET_LexException : public std::exception {
            public:
                Incomplete_LET_LexException(const std::string &err) : msg(err) { }
                // Override the what() function to return the error message
                const char* what() const noexcept override { return msg.c_str(); }
            private:
                std::string msg;
        };

        class Incomplete_MAIN_LexException : public std::exception {
            public:
                Incomplete_MAIN_LexException(const std::string &err) : msg(err) { }
                // Override the what() function to return the error message
                const char* what() const noexcept override { return msg.c_str(); }
            private:
                std::string msg;
        };
    };



};


#endif