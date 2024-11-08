#ifndef LINKED_LIST_HPP
#define LINKED_LIST_HPP

#include "Node.hpp"

#define DEBUG
  
class LinkedList {
public:
    int length;
    Node *tail;
    // Node *head, *tail;
    
    LinkedList() 
        // : length(0) , head(nullptr), tail(nullptr)
        : length(0) , tail(nullptr)
    {
    }

    // [09/20/2024]: ToDo - create copy constructor
    // LinkedList(const LinkedList& other) {
    //     Node *curr = other.tail;
        
    //     while (curr) {
    //         // [10/17/2024]: Note to copy a new SSA instr as well
    //         this->InsertAtHead(curr->instr);

    //         curr = curr->prev;
    //     }
    // }
    
    
    ~LinkedList() {
        // #ifdef DEBUG
        //     std::cout << "\tin ~LinkedList()" << std::endl;
        // #endif

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
        // head = nullptr;
        // if (tail) { delete tail; }
        tail = nullptr;
        
        // #ifdef DEBUG
        //     std::cout << "\tdone ~LinkedList()" << std::endl;
        // #endif
    }

    // [10/03/2024]: Commenting out to make singly linked list
    // void InsertAtHead(SSA *instruction) {
    //     Node* newNode = new Node();
    //     newNode->instr = instruction;

    //     // If the list is empty, make the new node as the head
    //     if (head == nullptr) {
    //         head = newNode;
    //         tail = newNode;
    //     } else {
    //         // Otherwise, insert the new node after the current head
    //         head->prev = newNode;
    //         // head->prev->next = head;
    //         head = newNode;
    //     }
    //     length++;
    // }

    Node* InsertAtTail(SSA *instruction) {
        #ifdef DEBUG
            std::cout << "inserting new SSA (" << instruction->toString() << ") at tail" << std::endl;
        #endif
        Node* newNode = new Node();
        newNode->instr = instruction;

        #ifdef DEBUG
            std::cout << "\tcreated new node" << std::endl;
        #endif

        // If the list is empty, make the new node as the head
        if (this->tail == nullptr) {
            // this->head = newNode;
            this->tail = newNode;
        } else {
            // Otherwise, insert the new node after the current tail
            // tail->next = newNode;
            newNode->prev = this->tail;
            this->tail = newNode;
        }

        this->length++;
        
        #ifdef DEBUG
            std::cout << "\tdone inserting at tail" << std::endl;
        #endif

        return newNode;
    }

    SSA* contains(SSA *instruction) const {
        // Node *curr = this->head;
        Node *curr = this->tail; // [10/03/2024]: making compare start at tail to compare from 'most recently added instr'

        // while (curr != tail) {
        // while (curr != this->head) {
        while (curr) {
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
            // if (curr != this->head) {
                std::cout << ", " << std::endl << "\t";
            // }
            // curr = curr->next;
            curr = curr->prev;
        }
        std::cout << std::endl;
        // #ifdef DEBUG
        //     std::cout << "Printed LinkedList" << std::endl;
        // #endif
    }

    std::string listToString() const {
        std::string lst = "\t";
        // Node *curr = this->head;
        Node *curr = this->tail;

        while (curr) {
            lst += curr->instr->toString();
            // if (curr != this->tail) {
            // if (curr != this->head) {
                lst += ", \n\t";
            // }
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