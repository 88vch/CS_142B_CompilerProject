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
        : length(0) , head(nullptr), tail(nullptr)
    {
    }

    // [09/09/2024]: Note - might be good to also create a function that prints the linked list
    LinkedList(std::vector<SSA*> SSA_instrs) {
        // [09/09/2024]: iterate through vector and insert each SSA one by one
    }
    
    ~LinkedList() {
        Node *current = head;
        while (current != nullptr) {
            Node *temp = current;
            current = current->next;
            delete temp;
        }
    }

    void InsertAtHead(SSA *instruction) {
        Node* newNode = new Node(*instruction);

        // If the list is empty, make the new node as the head
        if (head == nullptr) {
            head = newNode;
            tail = newNode;
            return;
        } else {
            // Otherwise, insert the new node after the current head
            head->prev = newNode;
            head->prev->next = head;
            head = newNode;
        }
        length++;
    }

    void InsertAtTail(SSA *instruction) {
        Node* newNode = new Node(*instruction);

        // If the list is empty, make the new node as the head
        if (head == nullptr) {
            head = newNode;
            tail = newNode;
            return;
        } else {
            // Otherwise, insert the new node after the current tail
            tail->next = newNode;
            tail->next->prev = tail;
            tail = newNode;
        }
        length++;
    }

    bool contains(SSA *instruction) const {
        Node *curr = head;

        while (curr != tail) {
            // ToDo: Revise this! [Old Version]; 
            // if (SSA::compare(curr->instr, instruction)) {
            //     return true;
            // }
            curr = curr->next;
            if (curr->instr.compare(instruction)) {
                return true;
            }
        }
        return false;
    }

    void printList() const {
        Node *curr = this->head;
        while (curr) {
            if (curr != head && curr != tail) {
                std::cout << ", " << std::endl;
            }
            curr->instr.toString();
            curr = curr->next;
        }
    }

private:
    int length;
    Node *head, *tail;
};

#endif