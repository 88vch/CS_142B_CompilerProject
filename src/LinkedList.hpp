#ifndef LINKED_LIST_HPP
#define LINKED_LIST_HPP

#include "SSA.hpp"

class Node {
public:
    SSA instr;
    Node *next;
    Node *prev;

    // Constructor
    Node(const SSA& instruction) : instr(instruction), next(nullptr), prev(nullptr) {}

};
    
class LinkedList {
public:
    LinkedList() 
        : head(nullptr), tail(nullptr), length(0) 
    {
    }
    
    ~LinkedList() {
        Node *current = head;
        while (current != nullptr) {
            Node *temp = current;
            current = current->next;
            delete temp;
        }
    }

    void InsertAtHead(SSA instruction) {
        Node* newNode = new Node(instruction);

        // If the list is empty, make the new node as the head
        if (head == nullptr) {
            head = newNode;
            tail = newNode;
            return;
        } else {
            // Otherwise, insert the new node after the current tail
            head->prev = newNode;
            head->prev->next = head;
            head = newNode;
        }
        length++;
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

    bool contains(SSA instruction) {
        Node *curr = head;

        while (curr != tail) {
            if (curr->instr == instruction) {
                return true;
            }
            curr = curr->next;
        }
        return false;
    }

private:
    int length;
    Node *head, *tail;
};

#endif