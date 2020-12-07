#include<bits/stdc++.h>
using namespace std;

class SymbolTable {

    typedef struct SymbolTableNode{
        map<string, string> table;
        vector<struct SymbolTableNode*> children;
        struct SymbolTableNode* parent;
        string funcName;
    } STNode;
    
    map<string, string> funcMap;
    map<string, vector<string> > funcParMap;
    STNode* curr = new STNode();

    public:

    void addFunction(string ID, string type, vector<string> parTypes) {
        if(funcMap.find(ID) == funcMap.end()) {
            funcMap[ID] = type;
            funcParMap[ID] = parTypes;
        }
        else{
            cout << "ERROR: A function with ID " << ID << " already exists.\n";
        }
    }

    void createChild(string currFunc) {
        STNode* newChild = new STNode();
        newChild->parent = curr;
        curr->children.push_back(newChild);
        curr = newChild;
        curr->funcName = currFunc;
    }

    string getCurrFunc() {
        return curr->funcName;
    }

    string getFuncType(string ID) {
        if(funcMap.find(ID) != funcMap.end()) {
            return funcMap[ID];
        }
        else{
            cout << "ERROR: No function " << ID << "() exists\n";
        }
    }

    vector<string> getFuncParTypes(string ID) {
        if(funcParMap.find(ID) != funcParMap.end()) {
            return funcParMap[ID];
        }
        else{
            cout << "ERROR: No function " << ID << "() exists\n";
        }
    }

    bool foundReference(string ID) {
        STNode* temp = curr;
        while(temp!=NULL) {
            if(temp->table.find(ID) != temp->table.end()){
                return true;
            }
            temp = temp->parent;
        }
        return false;
    } 

    bool alreadyDeclared(string ID) {
        if(curr->table.find(ID) == curr->table.end()) {
            return false;
        }
        return true;
    }

    void addEntry(string type, string ID) {
        if(alreadyDeclared(ID)){
            cout << "ERROR: Variable " << ID << " was already declared\n";
            return;
        }
        cout << "Added Entry: " << ID << endl;
        curr->table[ID] = type;
        return;
    }

    void moveToPar() {
        curr = curr->parent;
    }

    string getType(string ID) {
        STNode* temp = curr;
        while(temp!=NULL) {
            if(temp->table.find(ID) != temp->table.end()){
                return temp->table[ID];
            }
            temp = temp->parent;
        }
    }

};
