#include <vector>
#include <string>
#include <iostream>

#define DEBUG

// [08/27/2024]: Simple Func Struct
// [08/31/2024]: Welp, the good news is that this constructor works
// - the bad news is that our [Tokenizer] creates the function tokens like this: [`FunctionName`, `(`, `arg1`, `arg2`, `arg3`, `)`]
// - WHICH MEANS, we probably don't need most of this stuff here (except name), but we can reuse this stuff in the [Parser.cpp] funcCall() and next()
struct Func {
    Func(std::string functionName, bool ret = false) {
        #ifdef DEBUG
            std::cout << "[Parser::Func(functionName=" << functionName << ")] in constructor" << std::endl;
        #endif
        this->args = {};
        this->name = functionName;
        this->hasReturn = ret;
    }

    bool hasReturn;
    std::string name;
    std::vector<std::string> args;
    // [08/27/2024]: How to handle args?

    const std::string getName() const {
        return this->name;
    }

    const std::string toString() const {
        std::string ret = this->name + "(";
        
        for (const auto &arg : this->args) {
            ret += arg + ", ";
        }
        ret = ret.substr(0, ret.size() - 3);
        ret += ")"; // [08/31/2024]: Add args when we have them for later
        return ret;
    } 
};