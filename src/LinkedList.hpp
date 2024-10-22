#ifndef LINKED_LIST_HPP
#define LINKED_LIST_HPP

#include "SSA.hpp"

#define DEBUG

class Node {
public:
    SSA *instr;
    // Node *next;
    Node *prev;

    // Constructor
    // Node() : instr(nullptr), next(nullptr), prev(nullptr) {}
    Node() : instr(nullptr), prev(nullptr) {}

    ~Node() {
        // [10/19/2024]: If we only have references (from [Parser::instrList]), then we shouldn't delete the SSA instr here, rather in [Parser] 
        // if (this->instr) {
        //     delete this->instr; 
        // }
        this->instr = nullptr;
        // this->next = nullptr; 
        this->prev = nullptr; 
    }
};
  
class LinkedList {
public:
    int length;
    // Node *tail;
    Node *head, *tail;
    
    LinkedList() 
        : length(0) , head(nullptr), tail(nullptr)
        // : length(0) , tail(nullptr)
    {
    }

    // [09/20/2024]: ToDo - create copy constructor
    LinkedList(const LinkedList& other) {
        Node *curr = other.tail;
        
        while (curr) {
            // [10/17/2024]: Note to copy a new SSA instr as well
            this->InsertAtHead(curr->instr);

            curr = curr->prev;
        }
    }
    
    
    ~LinkedList() {
        #ifdef DEBUG
            std::cout << "\tin ~LinkedList()" << std::endl;
        #endif

        // Node *current = this->head;
        Node *current = this->tail;
        
        while (current) {
            Node *temp = current;
            // current = current->next;
            current = current->prev;

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

    // [10/03/2024]: Commenting out to make singly linked list
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
            // head->prev->next = head;
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
        if (this->tail == nullptr) {
            this->head = newNode;
            this->tail = newNode;
        } else {
            // Otherwise, insert the new node after the current tail
            // tail->next = newNode;
            newNode->prev = this->tail;
            this->tail = newNode;
        }
        length++;
    }

    SSA* contains(SSA *instruction) const {
        // Node *curr = this->head;
        Node *curr = this->tail; // [10/03/2024]: making compare start at tail to compare from 'most recently added instr'

        // while (curr != tail) {
        while (curr != this->head) {
            // ToDo: Revise this! [Old Version]; 
            // if (SSA::compare(curr->instr, instruction)) {
            //     return true;
            // }

            // curr = curr->next;
            curr = curr->prev;
            if (curr->instr->compare(instruction)) {
                return curr->instr;
            }
        }
        return nullptr;
    }

    void printList() const {
        if (this->tail == nullptr) {
            std::cout << std::endl;
            return;
        }

        // Node *curr = this->head;
        Node *curr = this->tail;
        // #ifdef DEBUG
        //     std::cout << "LinkedList::printList()" << std::endl;
        // #endif

        while (curr) {
            std::cout << curr->instr->toString();
            // if (curr != this->tail) {
            if (curr != this->head) {
                std::cout << ", " << std::endl << "\t";
            }
            // curr = curr->next;
            curr = curr->prev;
        }
        std::cout << std::endl;
        // #ifdef DEBUG
        //     std::cout << "Printed LinkedList" << std::endl;
        // #endif
    }

    std::string listToString() const {
        std::string lst = "";
        // Node *curr = this->head;
        Node *curr = this->tail;

        while (curr) {
            lst += curr->instr->toString();
            // if (curr != this->tail) {
            if (curr != this->head) {
                lst += ", \n\t";
            }
            // curr = curr->next;
            curr = curr->prev;
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