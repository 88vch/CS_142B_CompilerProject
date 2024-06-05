#ifndef LINKED_LIST_HPP
#define LINKED_LIST_HPP

#include <iostream>


/*
enum type to store the type of data structure
- individual instruction, super-imposed basic block
void * to the data structure object itself

linked list to store this?
*/

/*
Basic Block (BB) vs Super-Imposed Basic Block (SIBB)
- SIBB allows block-reuse if multiple points in control flow use this block
    (i.e. if-statement within loop will be called multiple times)
    (i.e.2 same if-statement could be used else-where as well)

Result: encapsulates all information we care about in the subtree of this ds
- if theres any chance you could generate better code by waiting, then don't generate yet
*/

struct res_n {
    Result *r;
    res_n *next;
    res_n *prev;
};


class DLinkedList {
public:
    DLinkedList()
    {
        this->head = nullptr;
        this->tail = nullptr;
        this->length = 0;
        
        this->instr_idx = this->const_idx = 0;
    }

private:
    res_n *head;
    res_n *tail;
    size_t length;
    size_t instr_idx, const_idx;
};



#endif