#ifndef TINY_EXCEPTIONS_HPP
#define TINY_EXCEPTIONS_HPP


#include <iostream>
#include <string>

namespace tinyExceptions_ns {
    enum numExceptions {
        Incomplete_relation_LexException,
        Incomplete_whileStatement_LexException, 
        Incomplete_ifStatement_LexException, 
        Incomplete_ASSIGNMENT_LexException, 
        Incomplete_IDENTIFIER_LexException, 
        Incomplete_statement_LexException, 
        Incomplete_statSequence_LexException, 
        Incomplete_FACTOR_LexException, 
        Incomplete_TERM_LexException, 
        Incomplete_EXPRESSION_LexException, 
        Incomplete_VAR_LexException, 
        Incomplete_MAIN_LexException, 
        Incomplete_Token_LexException
    };


    namespace Lexer_ns {
        class Incomplete_relation_LexException : public std::exception {
            public:
                Incomplete_relation_LexException(const std::string &err) : msg(err) { }
                // Override the what() function to return the error message
                const char* what() const noexcept override { return msg.c_str(); }
            private:
                std::string msg;
        }; 

        class Incomplete_whileStatement_LexException : public std::exception {
            public:
                Incomplete_whileStatement_LexException(const std::string &err) : msg(err) { }
                // Override the what() function to return the error message
                const char* what() const noexcept override { return msg.c_str(); }
            private:
                std::string msg;
        }; 

        class Incomplete_ifStatement_LexException : public std::exception {
            public:
                Incomplete_ifStatement_LexException(const std::string &err) : msg(err) { }
                // Override the what() function to return the error message
                const char* what() const noexcept override { return msg.c_str(); }
            private:
                std::string msg;
        }; 

        class Incomplete_ASSIGNMENT_LexException : public std::exception {
            public:
                Incomplete_ASSIGNMENT_LexException(const std::string &err) : msg(err) { }
                // Override the what() function to return the error message
                const char* what() const noexcept override { return msg.c_str(); }
            private:
                std::string msg;
        }; 

        class Incomplete_IDENTIFIER_LexException : public std::exception {
            public:
                Incomplete_IDENTIFIER_LexException(const std::string &err) : msg(err) { }
                // Override the what() function to return the error message
                const char* what() const noexcept override { return msg.c_str(); }
            private:
                std::string msg;
        }; 

        class Incomplete_statement_LexException : public std::exception {
            public:
                Incomplete_statement_LexException(const std::string &err) : msg(err) { }
                // Override the what() function to return the error message
                const char* what() const noexcept override { return msg.c_str(); }
            private:
                std::string msg;
        }; 

        class Incomplete_statSequence_LexException : public std::exception {
            public:
                Incomplete_statSequence_LexException(const std::string &err) : msg(err) { }
                // Override the what() function to return the error message
                const char* what() const noexcept override { return msg.c_str(); }
            private:
                std::string msg;
        }; 

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

        class Incomplete_VAR_LexException : public std::exception {
            public:
                Incomplete_VAR_LexException(const std::string &err) : msg(err) { }
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
        
        class Incomplete_Token_LexException : public std::exception {
            public:
                Incomplete_Token_LexException(const std::string &err) : msg(err) { }
                // Override the what() function to return the error message
                const char* what() const noexcept override { return msg.c_str(); }
            private:
                std::string msg;
        }; 
    };

    


    void throwException(numExceptions eType, std::string errMsg) {
        switch (eType) {
            case numExceptions::Incomplete_relation_LexException:
                throw tinyExceptions_ns::Lexer_ns::Incomplete_relation_LexException(errMsg);
                break;
            case numExceptions::Incomplete_whileStatement_LexException:
                throw tinyExceptions_ns::Lexer_ns::Incomplete_whileStatement_LexException(errMsg);
                break;
            case numExceptions::Incomplete_ifStatement_LexException:
                throw tinyExceptions_ns::Lexer_ns::Incomplete_ifStatement_LexException(errMsg);
                break;
            case numExceptions::Incomplete_ASSIGNMENT_LexException:
                throw tinyExceptions_ns::Lexer_ns::Incomplete_ASSIGNMENT_LexException(errMsg);
                break;
            case numExceptions::Incomplete_IDENTIFIER_LexException:
                throw tinyExceptions_ns::Lexer_ns::Incomplete_IDENTIFIER_LexException(errMsg);
                break;
            case numExceptions::Incomplete_statement_LexException:
                throw tinyExceptions_ns::Lexer_ns::Incomplete_statement_LexException(errMsg);
                break;
            case numExceptions::Incomplete_statSequence_LexException:
                throw tinyExceptions_ns::Lexer_ns::Incomplete_statSequence_LexException(errMsg);
                break;
            case numExceptions::Incomplete_FACTOR_LexException:
                throw tinyExceptions_ns::Lexer_ns::Incomplete_FACTOR_LexException(errMsg);
                break;
            case numExceptions::Incomplete_TERM_LexException:
                throw tinyExceptions_ns::Lexer_ns::Incomplete_TERM_LexException(errMsg);
                break;
            case numExceptions::Incomplete_EXPRESSION_LexException:
                throw tinyExceptions_ns::Lexer_ns::Incomplete_EXPRESSION_LexException(errMsg);
                break;
            case numExceptions::Incomplete_VAR_LexException:
                throw tinyExceptions_ns::Lexer_ns::Incomplete_VAR_LexException(errMsg);
                break;
            case numExceptions::Incomplete_MAIN_LexException:
                throw tinyExceptions_ns::Lexer_ns::Incomplete_MAIN_LexException(errMsg);
                break;
            case numExceptions::Incomplete_Token_LexException:
                throw tinyExceptions_ns::Lexer_ns::Incomplete_Token_LexException(errMsg);
                break;
        }
    };
};


#endif