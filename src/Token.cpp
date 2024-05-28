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
        case TOKEN_TYPE::EXPRESSION:
            ret = "EXPRESSION";
            break;
        case TOKEN_TYPE::OPEN_PAREN:
            ret = "OPEN_PAREN";
            break;
        case TOKEN_TYPE::CLOSE_PAREN:
            ret = "CLOSE_PAREN";
            break;
        case TOKEN_TYPE::OPEN_CURLY:
            ret = "OPEN_CURLY";
            break;
        case TOKEN_TYPE::CLOSE_CURLY:
            ret = "CLOSE_CURLY";
            break;
        case TOKEN_TYPE::IF:
            ret = "IF";
            break;
        case TOKEN_TYPE::THEN:
            ret = "THEN";
            break;
        case TOKEN_TYPE::ELSE:
            ret = "ELSE";
            break;
        case TOKEN_TYPE::FI:
            ret = "IF";
            break;
        case TOKEN_TYPE::WHILE:
            ret = "WHILE";
            break;
        case TOKEN_TYPE::DO:
            ret = "DO";
            break;
        case TOKEN_TYPE::OD:
            ret = "OD";
            break;
        case TOKEN_TYPE::RETURN:
            ret = "RETURN";
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


const std::string TOKEN_TYPE_toStringLower(TOKEN_TYPE token) {
    std::string str = TOKEN_TYPE_toString(token);
    std::transform(str.begin(), str.end(), str.begin(),
        [](unsigned char c){ return std::tolower(c); });

    return str;
}