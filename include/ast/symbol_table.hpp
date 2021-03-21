#ifndef symbol_table_hpp
#define symbol_table_hpp

#include <cassert>

class SymbolTable;
class StackPtr;

class StackPtr{
    private:
    int incr;
    int scopeincr[50];
    int current_scope = 0;
    public:
    StackPtr() {
    }

    int getIncr() {
        return incr;
    }

    void setIncr(int _incr) {
        incr = _incr;
    }

    int getscopeIncr() {
        return scopeincr[current_scope];
    }

    void setscopeIncr(int _scopeincr) {
        scopeincr[current_scope] = _scopeincr;
    }

    int getcurrentscope() {
        return current_scope;
    }

    void setcurrentscope(int _current_scope) {
        current_scope = _current_scope;
    }

};

class Node {
private:
    std::string type, format ,name, address;
    Node* next;
public:
    Node() {
        next = nullptr;
    }

    Node(std::string _type, std::string _format, std::string _name, std::string _address) {
        type = _type;
        format = _format;
        name = _name;
        address = _address;
        next = nullptr;
    }

    ~Node() {
        delete next;
    }

    void print() {
        std::cout << "Type: " << type << std::endl;
        std::cout << "Format: " << format << std::endl;
        std::cout << "Name: " << name << std::endl;
        std::cout << "Address: " << address << std::endl;
        std::cout << '\n';
    }

    std::string getType() {
        return type;
    }

    void setType(std::string _type) {
        type = _type;
    }

    std::string getFormat() {
        return format;
    }

    void setFormat(std::string _format) {
        format = _format;
    }

    std::string getName() {
        return name;
    }

    std::string getAddress() {
        return address;
    }

    void setAddress(std::string _address) {
        address = _address;
    }

    Node *getNext() {
        return next;
    }

    void setNext(Node *element) {
        next = element;
    }
};

class SymbolTable {
private:
    Node *head[50];
    int current_scope;
public:
    SymbolTable() {
        for(int i = 0; i < sizeof(head)/sizeof(head[0]); i++) {
            head[i] = nullptr;
        }
        current_scope = 0;
    }

    int getScope(){
        return current_scope;
    }

    bool insert(std::string type, std::string format, std::string name, std::string address) {
        if(head[current_scope] == nullptr) {
            head[current_scope] = new Node(type, format, name, address);
            return true;
        }
        else {
            Node *start = head[current_scope];
            while(start->getNext() != nullptr) {
                start = start->getNext();
            }
            start->setNext(new Node(type, format, name, address));
            return true;
        }
        return false;
    }

    // modify function should only be called when there is a declaration with an existing variable name
    bool modify(std::string type, std::string format, std::string name, std::string address) {
        if(head[current_scope] == nullptr) {
            return false;
        }
        else {
            Node *start = head[current_scope];
            while(start != nullptr) {
                if(start->getName() == name) {
                    start->setType(type);
                    start->setFormat(format);
                    start->setAddress(address);
                    return true;
                }
                start = start->getNext();
            }
        }
        return false;
    }

    std::string lookUp(std::string name) { // returns address
        Node *start = head[current_scope];
        if(start == nullptr) {
            return "Error: undefined reference";
        }
        while(start != nullptr) {
            if(start->getName() == name) {
                return start->getAddress();
            }
            else {
                start = start->getNext();
            }
        }
        return "Error: undefined reference";
    }

    bool newScope() {
        current_scope = current_scope + 1;
        assert(head[current_scope] == nullptr); // new scope must be empty
        Node *new_list = nullptr;
        Node **first_node = &new_list;
        Node *old_list = head[current_scope-1];
        while(old_list != nullptr) {
            new_list = new Node(old_list->getType(), old_list->getFormat(), old_list->getName(), old_list->getAddress());
            new_list = new_list->getNext();
            old_list = old_list->getNext();
        }
        head[current_scope] = *first_node;
        return true;
    }

    bool endScope() {
        if(head[current_scope] == nullptr) {
            current_scope = current_scope - 1;
            return true;
        }
        Node **list = &head[current_scope];
        Node *current = *list;
        Node *next = nullptr;
        if (current != nullptr) {
            next = current->getNext();
            delete(current);
            current = next;
        }
        *list = nullptr;
        if(head[current_scope] != nullptr) {
            std::cout << "this is wrong" << std::endl;
            return false;
        }
        current_scope = current_scope - 1;
        return true;
    }
};

#endif