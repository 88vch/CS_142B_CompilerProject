// #include "BasicBlock.hpp"

// // Q: isn't this just constructor?
// BasicBlock BasicBLock::MakeTree(NodeVariant base) {
//     BasicBLock tree(base);
//     return tree;
// }


// // don't really need to combien any tree except for ident & number (on expr, term, factor)
// // compute result & store in returned BasicBLock
// BasicBlock BasicBLock::CombineTree(BasicBLock x, BasicBLock y, int op) {
//     BasicBLock tree = x;

//     if (SymbolTable::symbol_table.at("+") == op) {
//         // 4 comparisons: [ident, ident], [ident, num], [num, ident], [num, num]

//     } else if (SymbolTable::symbol_table.at("-") == op) {

//     } else if (SymbolTable::symbol_table.at("*") == op) {

//     } else if (SymbolTable::symbol_table.at("/") == op) {

//     }




//     if (std::shared_ptr<main_Node>* mainPtr = std::get_if<std::shared_ptr<main_Node>>(&x.root)) {
//         // Handle main_Node
//         if (std::shared_ptr<varDecl_Node>* yPtr = std::get_if<std::shared_ptr<varDecl_Node>>(&y.root)) {
            
//         } else if (std::shared_ptr<funcDecl_Node>* yPtr = std::get_if<std::shared_ptr<funcDecl_Node>>(&y.root)) {
            
//         } else if (std::shared_ptr<statSequence_Node>* yPtr = std::get_if<std::shared_ptr<statSequence_Node>>(&y.root)) {
            
//         }
//     } else if (std::shared_ptr<funcBody_Node>* funcBodyPtr = std::get_if<std::shared_ptr<funcBody_Node>>(&x.root)) {
//         // Handle funcBody_Node
//     } else if (std::shared_ptr<formalParam_Node>* formalParamPtr = std::get_if<std::shared_ptr<formalParam_Node>>(&x.root)) {
//         // Handle formalParam_Node
//     } else if (std::shared_ptr<funcDecl_Node>* funcDeclPtr = std::get_if<std::shared_ptr<funcDecl_Node>>(&x.root)) {
//         // Handle funcDecl_Node
//     } else if (std::shared_ptr<varDecl_Node>* varDeclPtr = std::get_if<std::shared_ptr<varDecl_Node>>(&x.root)) {
//         // Handle varDecl_Node
//     } else if (std::shared_ptr<statSequence_Node>* statSequencePtr = std::get_if<std::shared_ptr<statSequence_Node>>(&x.root)) {
//         // Handle statSequence_Node
//     } else if (std::shared_ptr<statement_Node>* statementPtr = std::get_if<std::shared_ptr<statement_Node>>(&x.root)) {
//         // Handle statement_Node
//     } else if (std::shared_ptr<return_Node>* returnPtr = std::get_if<std::shared_ptr<return_Node>>(&x.root)) {
//         // Handle return_Node
//     } else if (std::shared_ptr<while_Node>* whilePtr = std::get_if<std::shared_ptr<while_Node>>(&x.root)) {
//         // Handle while_Node
//     } else if (std::shared_ptr<if_Node>* ifPtr = std::get_if<std::shared_ptr<if_Node>>(&x.root)) {
//         // Handle if_Node
//     } else if (std::shared_ptr<funcCall_Node>* funcCallPtr = std::get_if<std::shared_ptr<funcCall_Node>>(&x.root)) {
//         // Handle funcCall_Node
//     } else if (std::shared_ptr<assignment_Node>* assignmentPtr = std::get_if<std::shared_ptr<assignment_Node>>(&x.root)) {
//         // Handle assignment_Node
//     } else if (std::shared_ptr<relation_Node>* relationPtr = std::get_if<std::shared_ptr<relation_Node>>(&x.root)) {
//         // Handle relation_Node
//     } else if (std::shared_ptr<expression_Node>* expressionPtr = std::get_if<std::shared_ptr<expression_Node>>(&x.root)) {
//         // Handle expression_Node
//     } else if (std::shared_ptr<term_Node>* termPtr = std::get_if<std::shared_ptr<term_Node>>(&x.root)) {
//         // Handle term_Node
//     } else if (std::shared_ptr<factor_Node>* factorPtr = std::get_if<std::shared_ptr<factor_Node>>(&x.root)) {
//         // Handle factor_Node
//     } else {
//         std::cerr << "Unknown node type" << std::endl;
//     }
//     return tree;
// }