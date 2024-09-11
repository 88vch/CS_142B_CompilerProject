#ifndef LINKED_LIST_HPP
#define LINKED_LIST_HPP

#include "SSA.hpp"

class Node {
public:
    SSA *instr;
    Node *next;
    Node *prev;

    // Constructor
    Node(SSA *instruction) : instr(instruction), next(nullptr), prev(nullptr) {}

};
    
class LinkedList {
public:
    LinkedList() 
        : length(0) , head(nullptr), tail(nullptr)
    {
    }
    
    ~LinkedList() {
        Node *current = head;
        while (current != nullptr) {
            Node *temp = current;
            current = current->next;
            delete temp->instr;
            delete temp;
        }
    }

    void InsertAtHead(SSA *instruction) {
        Node* newNode = new Node(instruction);

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
        #ifdef DEBUG
            std::cout << "inserting new SSA (" << instruction->toString() << ") at tail" << std::endl;
        #endif
        Node* newNode = new Node(instruction);

        // If the list is empty, make the new node as the head
        if (head == nullptr) {
            #ifdef DEBUG
                std::cout << "list is empty" << std::endl;
            #endif
            head = newNode;
            tail = newNode;
            return;
        } else {
            // Otherwise, insert the new node after the current tail
            tail->next = newNode;
            newNode->prev = tail;
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
            if (curr->instr->compare(instruction)) {
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
            curr->instr->toString();
            curr = curr->next;
        }
    }

    // [09/09/2024]: Note - might be good to also create a function that prints the linked list
    void addVector(std::vector<SSA*> SSA_instrs) {
        #ifdef DEBUG
            std::cout << "LinkedList::addVector()" << std::endl;
        #endif
        // [09/09/2024]: iterate through vector and insert each SSA one by one
        for (SSA *instr : SSA_instrs) {
            this->InsertAtTail(instr);
        }
        #ifdef DEBUG
            std::cout << "done adding vector<SSA*> into LL" << std::endl;
        #endif
    }

private:
    int length;
    Node *head, *tail;
};

#endif