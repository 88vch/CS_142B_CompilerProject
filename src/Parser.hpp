#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>
#include <sstream>
#include <cstring>
#include "Result.hpp"
#include "BasicBlock.hpp"
#include "SSA.hpp"
#include "LinkedList.hpp"
#include "TinyExceptions.hpp"

#define DEBUG

// [08/27/2024]: Simple Func Struct
// [08/31/2024]: Welp, the good news is that this constructor works
// - the bad news is that our [Tokenizer] creates the function tokens like this: [`FunctionName`, `(`, `arg1`, `arg2`, `arg3`, `)`]
// - WHICH MEANS, we probably don't need most of this stuff here (except name), but we can reuse this stuff in the [Parser.cpp] funcCall() and next()
struct Func {
    Func(std::string functionName) {
        #ifdef DEBUG
            std::cout << "[Parser::Func(functionName=" << functionName << ")] in constructor" << std::endl;
        #endif
        this->args = {};
        this->name = functionName;


        // int split_idx = 0;
        // for (char c : functionName) {
        //     if (std::isalpha(c)) {
        //         split_idx++;
        //     } else { break; }
        // }
        // #ifdef DEBUG
        //     std::cout << "functionName=[" << functionName << "]" << std::endl;
        // #endif
        // this->name = functionName.substr(0, split_idx);
        // functionName = functionName.substr(split_idx, functionName.size() - this->name.size());
    
        // #ifdef DEBUG
        //     std::cout << "split_idx=[" << split_idx << "], this->name=[" << this->name << "], functionName: [" << functionName << "]" << std::endl;
        // #endif

        // if (functionName[0] != '(') {
        //     exit(EXIT_FAILURE); // error; expected arg-start-token `(`
        // }

        // const char *delim = ", ";
        // this->args = {};
        // functionName = functionName.substr(1, functionName.size() - 2);

        // char *token = strtok(&functionName[0], delim);
        // // Keep tokenizing the string until strtok returns NULL
        // while (token != nullptr) {
        //     std::cout << "Token: " << token << std::endl;
        //     this->args.push_back(token);

        //     // Get the next token
        //     token = strtok(nullptr, delim);
        // }
    }

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

// parse(): IR through BasicBlocks

// Note: all nodes will be inherited from BasicBlock
// Note: shouldn't need keywords bc we only care about computational code
class Parser {
public:
    Parser(const std::vector<Result> &tkns)
        :source(std::move(tkns)) 
    {
        this->SSA_start = nullptr;
        this->SSA_parent = nullptr;

        this->s_index = 0;
        this->source_len = tkns.size();
        this->SSA_instrs = {};
        this->varVals = {};
        
        this->BB0 = new BasicBlock();
        this->BB_start = nullptr;
        this->BB_parent = this->BB0;

        next();
    }

    // [07/25/2024]: ToDo
    inline int getOp() const {
        #ifdef DEBUG
            std::cout << "\t[Parser::getOp()]; this->sym.kind=" << this->sym.get_kind_literal() << ", this->sym.value=" << this->sym.get_value_literal() << std::endl;
        #endif
        switch (this->sym.get_kind_literal()) {
            case 0: // const
                return SymbolTable::operator_table.at("const");
                break;
            case 1: // ident
                break;
            case 2: // keyword
                switch (this->sym.get_value_literal()) {
                    case 0: // `+`
                        return SymbolTable::operator_table.at("add");
                    case 1: // `-`
                        return SymbolTable::operator_table.at("sub");
                    case 2: // `*`
                        return SymbolTable::operator_table.at("mul");
                    case 3: // `/`
                        return SymbolTable::operator_table.at("div");
                }
                break;
        }
        return -1; // stub
    }


    inline SSA* CheckExistence(const int op, SSA *x, SSA *y) const {
        #ifdef DEBUG
            std::cout << "\t[Parser::CheckExistence(op=[" << op << "], x=" << x->toString() << ",y=" << y->toString() << "), this->SSA_instrs.size()=" << this->SSA_instrs.size() << "]" << std::endl;
        #endif
        SSA *ret = nullptr;
        
        for (SSA* instr : this->SSA_instrs) {
            #ifdef DEBUG
                std::cout << "\t\tcomparing instr=[" << instr->toString() << "]" << std::endl;
            #endif
            if (instr->compare(op, x, y)) {
                #ifdef DEBUG
                    std::cout << "\t\tgot true!!!" << std::endl;
                #endif
                ret = instr;
                break;
            }
            #ifdef DEBUG
                std::cout << "\t\tgot false" << std::endl;
            #endif
        }
        #ifdef DEBUG
            if (ret != nullptr) {
                std::cout << "\t\treturning: [" << ret->toString() << "]" << std::endl;
            } else { 
                std::cout << "\t\treturning: nullptr!" <<std::endl;
            }
        #endif
        return ret;
    }

    // [08/22/2024]: might need to revise (like CheckExistence()) to include op and curr sym's values
    inline SSA* CheckConstExistence(int val = -1) const {
        int comparisonVal;
        if (val == -1) {
            comparisonVal = this->sym.get_value_literal();
            #ifdef DEBUG
                // std::cout << "\t[Parser::CheckExistence(op=" << op << "), this->sym.get_value_literal()=" << this->sym.get_value_literal() << "]" << std::endl;
                std::cout << "\tcomparing value: [" << comparisonVal << "], [this->SSA_instrs].size() =" << this->SSA_instrs.size() << std::endl;
            #endif
        } else {
            comparisonVal = val;
        }


        SSA *ret = nullptr;
        for (SSA* instr : this->SSA_instrs) {
            #ifdef DEBUG
                std::cout << "\tthis instr: [" << instr->toString() << "]" << std::endl;
            #endif
            if (instr->compareConst(comparisonVal)) {
                ret = instr;
                break;
            }
        }
        return ret;
    }

    // [07/25/2024]: ToDo
    inline SSA* BuildIR(int op, SSA *x, SSA *y) {
        #ifdef DEBUG
            std::cout << "\t[Parser::BuildIR(op=" << op << ", this->sym=" << this->sym.to_string() << ")]: SSA x = " << x->toString() << ", SSA y = " << y->toString() << std::endl;
        #endif
        SSA *ret = CheckExistence(op, x, y); // [08/12/2024]: Note we might have to include the `op` here to check diff types (i.e. `+` vs `*`)
        // if we don't get [nullptr] from CheckExistence(), that implies there was a previous SSA of the exact same type; so rather than creating a dupe, we can simply use the previously existing one (Copy Propagation)
        //      Note: check that all previous instructions will DOM this SSA instr
        #ifdef DEBUG
            std::cout << "\treturn value from [this->CheckExistence]: ";
        #endif
        if (ret) { 
            #ifdef DEBUG
                std::cout << ret->toString() << std::endl;
            #endif
            return ret; 
        } else {
            #ifdef DEBUG
                std::cout << "nullptr!" << std::endl;
            #endif
        }

        // assume op represents the keyword on the [SymbolTable::symbol_table]
        switch (op) {
                case 0: // `+`
                    // ret = new SSA(SymbolTable::operator_table.at("add"), x, y);
                    #ifdef DEBUG
                        std::cout << "\t\tcase 0 (+): " << std::endl;
                    #endif
                    ret = new SSA(SymbolTable::operator_table.at("add"), x, y);
                    #ifdef DEBUG
                        std::cout << "\t\tgot ret: " << ret->toString() << std::endl;
                    #endif
                    break;
                case 1: // `-`
                    ret = new SSA(SymbolTable::operator_table.at("sub"), x, y);
                    break;
                case 2: // `*`
                    ret = new SSA(SymbolTable::operator_table.at("mul"), x, y);
                    break;
                case 3: // `/`
                    ret = new SSA(SymbolTable::operator_table.at("div"), x, y);
                    break;
            }
        this->SSA_instrs.push_back(ret);
        return ret;
    }

    std::vector<SSA*> getSSA() const { return this->SSA_instrs; }

    std::unordered_map<std::string, SSA*> getVarVal() const { return this->varVals; }

    BasicBlock parse(); // IR BB-representation 
    void parse_generate_SSA(); // generate all SSA Instructions
private:
    Result sym;
    std::vector<Result> source;
    std::vector<SSA*> SSA_instrs;

    // [09/02/2024]: Do we need to optimize this? (i.e. <int, int> that is a table lookup probably in [SymbolTable]?)
    // - Note: we probably should
    std::unordered_map<std::string, SSA*> varVals; 
    size_t s_index, source_len;

    // [09/02/2024]: Does this need to include the `const` SSA's? I don't think it should bc the const instr's only belong to BB0 (special)
    std::unordered_map<int, LinkedList> instrList; // current instruction list (copied for each BB)
    
    BasicBlock *BB0; // [09/02/2024]: special BB always generated at the start. Contains const SSA instrs
    BasicBlock *BB_start; // first result obj generated from this file
    BasicBlock *BB_parent; // most recent BasicBlock (or 2nd most [if/while])

    SSA *SSA_start;
    SSA *SSA_parent; // copied from above's [BasicBlock]

    std::unordered_set<int> varDeclarations; // the int in the symbol table
    std::unordered_set<int> funcDeclarations; // the int in the symbol table

    // Recursive Descent
    SSA* p_start();
    void p_varDecl();
    SSA* p_statSeq();
    SSA* p_statement(); 
    SSA* p_assignment();
    SSA* p_funcCall();
    SSA* p_ifStatement();
    SSA* p_whileStatement();
    SSA* p_return();

    SSA* p_relation();
    SSA* p_expr();
    SSA* p_term();
    SSA* p_factor();

    // peek & consume char
    inline void next() {
        #ifdef DEBUG
            std::cout << "in [next(this->sym=" << this->sym.to_string_literal() << ")]" << std::endl;
        #endif


        if (this->s_index < this->source_len) {
            this->sym = this->source.at(this->s_index++);
        } else {
            this->sym = Result(2, -1); // keyword: ["EOF": -1]
        }

        #ifdef DEBUG
            std::cout << "\tmodified [this->sym] to: " << this->sym.to_string_literal() << std::endl;
        #endif
    }

    // consumes [non-optional's]
    inline bool CheckFor(Result expected_token, bool optional = false) {
        #ifdef DEBUG
            std::cout << "Checking For [expected: " << expected_token.to_string() << "], [got: " << this->sym.to_string() << "]" << std::endl;
        #endif
        bool retVal = false;
        
        // Note: there might be some logic messed up abt this (relation: [expected_token, Result(2, -1)])
        if (!expected_token.compare(Result(2, -1))) { 
            // #ifdef DEBUG
            //     std::cout << "expected token != EOF token" << std::endl;
            // #endif
            if (optional) { // optional's don't get consumed
                #ifdef DEBUG
                    std::cout << "\toptional!" << std::endl;
                #endif
                if (this->sym.compare(expected_token)) {
                    retVal = true;
                }
            } else {
                #ifdef DEBUG
                    std::cout << "\tnot an optional! (i.e. will consume if can)" << std::endl;
                #endif
                if (!this->sym.compare(expected_token)) {
                    std::cout << "Error: CheckFor expected: [" << expected_token.to_string() << "], got: [" << this->sym.to_string() << "]! exiting prematurely..." << std::endl;
                    exit(EXIT_FAILURE);
                } else { 
                    this->next(); 
                }
                retVal = true;
            }
        }

        #ifdef DEBUG
            std::cout << "\treturning [" << retVal << "]" << std::endl;
        #endif
        return retVal;
    }

    // [09/03/2024]: Consumes if exists else does nothing
    inline void CheckFor_udf_optional_paren() {
        // optional parenthesis for functions without arguments
        if (this->CheckFor(Result(2, 13), true)) { // check for `(`
            next();
            this->CheckFor(Result(2, 14)); // check for `)`
        }
    }

    inline bool SSA_exists(SSA new_instr) const {
        SSA *instr = nullptr;


        if (new_instr.get_operator() == 0) { // const
            for (SSA* i : this->SSA_instrs) {
                instr = i;
                if (new_instr.compareConst(instr)) {
                    return true;
                }
            }
        } else {            
            for (SSA* i : this->SSA_instrs) {
                instr = i;
                if (new_instr.compare(instr)) {
                    return true;
                }
            }
        }
        return false;
    }

    inline void printVVs() { // print varVals
        std::cout << "printing [this->varVals(size=" << this->varVals.size() << ")]:" << std::endl;
        for (const auto &p : this->varVals) {
            std::cout << "[" << p.first << "], [" << p.second->toString() << "]" << std::endl;
        }
    }
};

#endif