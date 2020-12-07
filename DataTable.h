using namespace llvm;

class DataTable {

    typedef struct DataTableNode{
        map<string, Value*> table;
        vector<struct DataTableNode*> children;
        struct DataTableNode* parent;
        string funcName;
    } DataNode;

    DataNode* curr = new DataNode();

    public:

    void createChild(string currFunc) {
        DataNode* newChild = new DataNode();
        newChild->parent = curr;
        curr->children.push_back(newChild);
        curr = newChild;
        curr->funcName = currFunc;
    }

    void addEntry(Value* addr, string ID) {
        curr->table[ID] = addr;
    }

    string getCurrFunc() {
        return curr->funcName;
    }

    Value* getAddr(string ID) {
        DataNode* temp = curr;
        while(temp!=NULL) {
            if(temp->table.find(ID) != temp->table.end()){
                return temp->table[ID];
            }
            temp = temp->parent;
        }
    }

    void moveToPar() {
        curr = curr->parent;
    }

    bool isScopeGlobal() {
        return (curr->parent == NULL);
    }

};