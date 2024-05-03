#ifndef HELPER_HPP
#define HELPER_HPP

#include "Token.hpp"

// This is a helper file containing:
// all the functions that will ONLY be used during testing
std::string TOKEN_TYPE_toString(TOKEN_TYPE token) {
    std::string ret;
    switch (token) {
        case TOKEN_TYPE::IDENTIFIER:
            ret = "IDENTIFIER";
            break;
        case TOKEN_TYPE::NUMBER:
            ret = "NUMBER";
            break;
        case TOKEN_TYPE::PLUS:
            ret = "PLUS";
            break;
        case TOKEN_TYPE::MINUS:
            ret = "MINUS";
            break;
        case TOKEN_TYPE::MULTIPLY:
            ret = "MULTIPLY";
            break;
        case TOKEN_TYPE::DIVIDE:
            ret = "DIVIDE";
            break;
        case TOKEN_TYPE::SEMICOLON:
            ret = "SEMICOLON";
            break;
        case TOKEN_TYPE::PRINT:
            ret = "PRINT";
            break;
        case TOKEN_TYPE::LET:
            ret = "LET";
            break;
        case TOKEN_TYPE::ASSIGNMENT:
            ret = "ASSIGNMENT";
            break;
        case TOKEN_TYPE::MAIN:
            ret = "MAIN";
            break;
        case TOKEN_TYPE::END_OF_FILE:
            ret = "END_OF_FILE";
            break;
        default:
            ret = "";
            break;
    }
    return ret;
}

#endif