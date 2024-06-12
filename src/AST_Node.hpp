#ifndef AST_NODE_HPP
#define AST_NODE_HPP

#include <memory>
#include <iostream>

std::unordered_map<std::string, int> func_retType = {
    {"void", 0},
    {"int", 1},
    {"char", 2}
    // [TODO]: figure out function return types
};

std::unordered_map<std::string, int> operations = {
    {"void", 0},
    {"int", 1},
    {"char", 2}
    // [TODO]: figure out function return types
};


class main_Node; // root
class funcBody_Node;
class formalParam_Node;
class funcDecl_Node;
class varDecl_Node;

class statSequence_Node;
class statement_Node;

class return_Node;
class while_Node;
class if_Node;
class funcCall_Node;
class assignment_Node;

class relation_Node;
class expression_Node;
class term_Node;
class factor_Node;

class identNum_Node;

class main_Node {
public:
    main_Node() : varDecl(nullptr), funcDecl(nullptr), statSequence(nullptr) {}
    main_Node(std::shared_ptr<statSequence_Node> s) : varDecl(nullptr), funcDecl(nullptr), statSequence(s) {}
    main_Node(std::shared_ptr<varDecl_Node> v, std::shared_ptr<statSequence_Node> s) : varDecl(v), funcDecl(nullptr), statSequence(nullptr) {}
    main_Node(std::shared_ptr<funcDecl_Node> f, std::shared_ptr<statSequence_Node> s) : varDecl(nullptr), funcDecl(f), statSequence(nullptr) {}
    main_Node(std::shared_ptr<varDecl_Node> v, std::shared_ptr<funcDecl_Node> f, std::shared_ptr<statSequence_Node> s) : varDecl(v), funcDecl(f), statSequence(nullptr) {}

    virtual void print() const {
        std::cout << "main: ";
        if (varDecl == nullptr) {
            std::cout << "varDecl(nullptr) ";
        } else {
            varDecl->print();
            std::cout << " ";
        }
        if (funcDecl == nullptr) {
            std::cout << "funcDecl(nullptr) ";
        } else {
            funcDecl->print();
            std::cout << " ";
        }
        if (statSequence == nullptr) {
            std::cout << "statSequence(nullptr) ";
        } else {
            statSequence->print();
            std::cout << " ";
        }
        std::cout << "." << std::endl;
    }
private:
    std::shared_ptr<varDecl_Node> varDecl;
    std::shared_ptr<funcDecl_Node> funcDecl;
    std::shared_ptr<statSequence_Node> statSequence;
};

class funcBody_Node : main_Node {
public:
    funcBody_Node() : varDecl(nullptr), statSequence(nullptr) {}

    // [TODO]
    void print() const override {
        std::cout << "funcBody:" << std::endl;
    }
private:
    std::shared_ptr<varDecl_Node> varDecl;
    std::shared_ptr<statSequence_Node> statSequence;
};

class formalParam_Node : main_Node {
public:
    formalParam_Node() : identifiers() {}

    // [TODO]
    void print() const override {
        std::cout << "formalParam:" << std::endl;
    }
private:
    std::vector<int> identifiers;
};

class funcDecl_Node : main_Node {
public:
    funcDecl_Node(int ret) : retType(ret), formalParams(nullptr), funcBody(nullptr) {}

    // [TODO]
    void print() const override {
        std::cout << "funcDecl:" << std::endl;
    }
private:
    int retType;
    std::shared_ptr<formalParam_Node> formalParams;
    std::shared_ptr<funcBody_Node> funcBody;
};

class varDecl_Node : main_Node {
public:
    varDecl_Node() : identifiers() {}

    // [TODO]
    void print() const override {
        std::cout << "varDecl:" << std::endl;
    }
private:
    std::vector<int> identifiers;
};

class statSequence_Node : main_Node {
public:
    statSequence_Node(int ret) : statements() {}

    // [TODO]
    void print() const override {
        std::cout << "statSequence:" << std::endl;
    }
private:
    std::vector<statement_Node> statements;
};

class statement_Node : main_Node {
public:
    statement_Node(std::shared_ptr<assignment_Node> a)
        : assignment(a), funcCall(nullptr), ifStat(nullptr), whileStat(nullptr), returnStat(nullptr) {}
    statement_Node(std::shared_ptr<funcCall_Node> a)
        : assignment(nullptr), funcCall(a), ifStat(nullptr), whileStat(nullptr), returnStat(nullptr) {}
    statement_Node(std::shared_ptr<if_Node> a)
        : assignment(nullptr), funcCall(nullptr), ifStat(a), whileStat(nullptr), returnStat(nullptr) {}
    statement_Node(std::shared_ptr<while_Node> a)
        : assignment(nullptr), funcCall(nullptr), ifStat(nullptr), whileStat(a), returnStat(nullptr) {}
    statement_Node(std::shared_ptr<return_Node> a)
        : assignment(nullptr), funcCall(nullptr), ifStat(nullptr), whileStat(nullptr), returnStat(a) {}
    


    // [TODO]
    void print() const override {
        std::cout << "statement:" << std::endl;
    }
private:
    std::shared_ptr<assignment_Node> assignment;
    std::shared_ptr<funcCall_Node> funcCall;
    std::shared_ptr<if_Node> ifStat;
    std::shared_ptr<while_Node> whileStat;
    std::shared_ptr<return_Node> returnStat;
};

class return_Node : main_Node {
public:
    return_Node() : expression(nullptr) {}
    
    // [TODO]
    void print() const override {
        std::cout << "return:" << std::endl;
    }
private:
    std::shared_ptr<expression_Node> expression;
};

class while_Node : main_Node {
public:
    while_Node() : relation(nullptr), statSequence(nullptr) {}
    
    // [TODO]
    void print() const override {
        std::cout << "while:" << std::endl;
    }
private:
    std::shared_ptr<relation_Node> relation;
    std::shared_ptr<statSequence_Node> statSequence;
};

class if_Node : main_Node {
public:
    if_Node() : relation(nullptr), then_statSequence(nullptr), else_statSequence(nullptr) {}
    
    // [TODO]
    void print() const override {
        std::cout << "if:" << std::endl;
    }
private:
    std::shared_ptr<relation_Node> relation;
    std::shared_ptr<statSequence_Node> then_statSequence;
    std::shared_ptr<statSequence_Node> else_statSequence;
};

class funcCall_Node : main_Node {
public:
    funcCall_Node() : expressions() {}
    
    // [TODO]
    void print() const override {
        std::cout << "funcCall:" << std::endl;
    }
private:
    int ident;
    std::vector<std::shared_ptr<expression_Node>> expressions;
};

class assignment_Node : main_Node {
public:
    assignment_Node() : expression(nullptr) {}
    
    // [TODO]
    void print() const override {
        std::cout << "assignment:" << std::endl;
    }
private:
    int ident;
    std::shared_ptr<expression_Node> expression;
};

class relation_Node : main_Node {
public:
    relation_Node() : expressionA(nullptr), relation(nullptr), expressionB(nullptr) {}
    
    // [TODO]
    void print() const override {
        std::cout << "relation:" << std::endl;
    }
private:
    std::shared_ptr<expression_Node> expressionA;
    std::shared_ptr<relation_Node> relation;
    std::shared_ptr<expression_Node> expressionB;
};
// should not be necessary (i think since we're computing results)
class expression_Node : main_Node {
public:
    expression_Node() : term(nullptr), nextExpr(nullptr) {}
    
    // [TODO]
    void print() const override {
        std::cout << "expression:" << std::endl;
    }
private:
    std::shared_ptr<term_Node> term;
    int op;
    std::shared_ptr<expression_Node> nextExpr;
};
// should not be necessary (i think since we're computing results)
class term_Node : main_Node {
public:
    term_Node() : factor(nullptr), nextTerm(nullptr) {}
    
    // [TODO]
    void print() const override {
        std::cout << "term:" << std::endl;
    }
private:
    std::shared_ptr<factor_Node> factor;
    int op;
    std::shared_ptr<term_Node> nextTerm;
};

class factor_Node : main_Node {
public:
    factor_Node(int x) : expression(nullptr), funcCall(nullptr)
    {
        // check whether [x] is [ident] or [number] (symbol_table)
    }
    factor_Node(std::shared_ptr<expression_Node> a) : expression(a), funcCall(nullptr) {}
    factor_Node(std::shared_ptr<funcCall_Node> a) : expression(nullptr), funcCall(a) {}
    
    // [TODO]
    void print() const override {
        std::cout << "factor:" << std::endl;
    }
private:
    std::shared_ptr<identNum_Node> ident_num;
    std::shared_ptr<expression_Node> expression;
    std::shared_ptr<funcCall_Node> funcCall;
};

class identNum_Node : main_Node {
public:
    // check whether [x] is [ident] or [number] (symbol_table)
    identNum_Node(int x, bool isIdent) : val(x), isIdent(isIdent) {}

    // [TODO]
    void print() const override {
        std::cout << "[ident/num]:" << std::endl;
    }
private:
    int val;
    bool isIdent;
};

#endif