#ifndef LINKED_LIST_HPP
#define LINKED_LIST_HPP

#include "SSA.hpp"

#define DEBUG

class Node {
public:
    SSA *instr;
    Node *next;
    Node *prev;

    // Constructor
    Node() : instr(nullptr), next(nullptr), prev(nullptr) {}

    ~Node() { 
        if (this->instr) {
            delete this->instr; 
            this->instr = nullptr; 
        }
        // this->instr = nullptr;
        this->next = nullptr; this->prev = nullptr; 
    }
};
    
class LinkedList {
public:
    int length;
    Node *head, *tail;
    
    LinkedList() 
        : length(0) , head(nullptr), tail(nullptr)
    {
    }
    
    // [09/20/2024]: ToDo - create copy constructor
    
    ~LinkedList() {
        // #ifdef DEBUG
        //     std::cout << "\tin ~LinkedList()" << std::endl;
        // #endif

        Node *current = head;
        while (current) {
            Node *temp = current;
            current = current->next;
            
            delete temp;
            temp = nullptr;
        }

        // if (head) { delete head; }
        head = nullptr;
        // if (tail) { delete tail; }
        tail = nullptr;
        
        // #ifdef DEBUG
        //     std::cout << "\tdone ~LinkedList()" << std::endl;
        // #endif
    }

    void InsertAtHead(SSA *instruction) {
        Node* newNode = new Node();
        newNode->instr = instruction;

        // If the list is empty, make the new node as the head
        if (head == nullptr) {
            head = newNode;
            tail = newNode;
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
        Node* newNode = new Node();
        newNode->instr = instruction;

        // If the list is empty, make the new node as the head
        if (head == nullptr) {
            head = newNode;
            tail = newNode;
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
        // #ifdef DEBUG
        //     std::cout << "LinkedList::printList()" << std::endl;
        // #endif

        while (curr) {
            std::cout << curr->instr->toString();
            if (curr != tail) {
                std::cout << ", " << std::endl << "\t";
            }
            curr = curr->next;
        }
        std::cout << std::endl;
        // #ifdef DEBUG
        //     std::cout << "Printed LinkedList" << std::endl;
        // #endif
    }

    std::string listToString() const {
        std::string lst = "";
        Node *curr = this->head;

        while (curr) {
            lst += curr->instr->toString();
            if (curr != tail) {
                lst += ", \n\t";
            }
            curr = curr->next;
        }
        lst += "\n";
        return lst;
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
};

#endif