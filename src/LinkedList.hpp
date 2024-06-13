#ifndef LINKED_LIST_HPP
#define LINKED_LIST_HPP

#include "SSA.hpp"

class Node {
public:
    SSA *instr;
    Node *next;

    // Constructor
    Node(SSA instruction) {
        instr = new SSA(instruction);
        next = nullptr;
    }
    ~Node() {
        delete instr;
    }
};
    
class LinkedList {
public:
    LinkedList() {
        head = nullptr;
        tail = nullptr;
    }
    ~LinkedList() {
        Node *current = head;
        while (current != nullptr) {
            Node *temp = current;
            current = current->next;
            delete temp;
        }
    }

    void InsertAtTail(SSA instruction) {
        Node* newNode = new Node(instruction);

        // If the list is empty, make the new node as the head
        if (head == nullptr) {
            head = newNode;
            tail = newNode;
            return;
        } else {
            // Otherwise, insert the new node after the current tail
            tail->next = newNode;
            tail = newNode;
        }
        length++;
    }

private:
    int length;
    Node *head, *tail;
};

#endif