#ifndef symbol_table_hpp
#define symbol_table_hpp

#include <cassert>

class SymbolTable;
class StackPtr;

class StackPtr{
    private:
    int incr;
    int argc;
    int scopeincr[50];
    int current_scope = 0;
    int nullfunc = 0;
    int freturn = 0;
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

    int getscopeIncr(int scope) {
        return scopeincr[scope];
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

    int getArgc() {
        return argc;
    }

    void setArgc(int _argc) {
        argc = _argc;
    }

    int getNullfunc() {
        return nullfunc;
    }

    void setNullfunc(int _nullfunc) {
        nullfunc = _nullfunc;
    }

    int getfreturn() {
        return freturn;
    }

    void setfreturn(int _freturn) {
        freturn = _freturn;
    }
};

class Node {
private:
    std::string type, format ,name, address;
    Node* next;
    friend class SymbolTable;
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
    std::string funcstart[50];
    std::string funcend[50];
    std::string loopstart[50];
    std::string loopend[50];
    int funcscope;
    int loopscope;
public:
    SymbolTable() {
        for(int i = 0; i < sizeof(head)/sizeof(head[0]); i++) {
            head[i] = nullptr;
        }
        current_scope = 0;
    }

    std::string getfuncstart(){
        return funcstart[funcscope];
    }

    std::string getfuncend(){
        return funcend[funcscope];
    }

    std::string getloopstart(){
        return loopstart[loopscope];
    }

    std::string getloopend(){
        return loopend[loopscope];
    }

    int getfuncscope(){
        return funcscope;
    }

    int getloopscope(){
        return loopscope;
    }

    void setfuncstart(std::string _funcstart){
        funcstart[funcscope] = _funcstart;
    }

    void setfuncend(std::string _funcend){
        funcend[funcscope] = _funcend;
    }

    void setloopstart(std::string _loopstart){
        loopstart[loopscope] = _loopstart;
    }

    void setloopend(std::string _loopend){
        loopend[loopscope] = _loopend;
    }

    int getScope(){
        return current_scope;
    }

    void setfuncscope(int _funcscope){
        funcscope = _funcscope;
    }

    void setloopscope(int _loopscope){
        loopscope = _loopscope;
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

    std::string getType(std::string name) { // returns address
        Node *start = head[current_scope];
        if(start == nullptr) {
            return "Error: undefined reference";
        }
        while(start != nullptr) {
            if(start->getName() == name) {
                return start->getType();
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
        Node *old_list = head[current_scope-1];
        if (old_list != nullptr) {
            head[current_scope] = new Node(old_list->getType(), old_list->getFormat(), old_list->getName(), old_list->getAddress());
            Node *start = head[current_scope];
            while(old_list->next != nullptr) {
                old_list = old_list->next;
                head[current_scope]->next = new Node(old_list->getType(), old_list->getFormat(), old_list->getName(), old_list->getAddress());
                head[current_scope] = head[current_scope]->next;
            }
            head[current_scope] = start;
            return true;
        }
        else {
            head[current_scope] = nullptr;
            return true;
        }
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