#include <bits/stdc++.h>
#include "llvm/IR/Module.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/IR/LLVMContext.h"
#include <llvm/IRReader/IRReader.h>
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/IR/CFG.h"

using namespace std;
using namespace llvm;

void dfs(int v, vector<int> g[], set<int> EBBLeaders, map<int, string> nameMap) {

    cout << nameMap[v] << " ";

    for(int u : g[v]) {
        if(EBBLeaders.find(u)==EBBLeaders.end()) {
            dfs(u, g, EBBLeaders, nameMap);
        }
    }
}

void printEBBs(Function &f) {
    
    cout << "\nProceesing Function: " << string(f.getName()) << endl;

    // Creating vertex mapping first
    map<string, int> vertexMap;
    map<int, string> nameMap;
    int count = 0;
    for(auto iter=f.getBasicBlockList().begin(); iter!=f.getBasicBlockList().end(); iter++) {
        BasicBlock &bb = *iter;
        vertexMap[string(bb.getName())] = count;
        nameMap[count] = string(bb.getName());
        count++;
    }

    // Creating inDegree array and the adjacency list
    vector<int> g[count];
    set<int> EBBLeaders;
    for(auto iter=f.getBasicBlockList().begin(); iter!=f.getBasicBlockList().end(); iter++) {
        
        BasicBlock &bb = *iter;
        int v = vertexMap[string(bb.getName())];
        
        int inDegree = 0;
        for(BasicBlock *Pred : predecessors(&bb)) {
            inDegree++;
        }
        if(inDegree != 1){
            EBBLeaders.insert(v);
        }

        for(BasicBlock *Succ : successors(&bb)) {
            g[v].push_back(vertexMap[string(Succ->getName())]);
        }
    }

    // Printing out EBBs by DFS
    count = 1;
    for(int v : EBBLeaders) {
        cout << "\nExtended Basic Block: " << count << endl;
        dfs(v, g, EBBLeaders, nameMap);
        cout << "\n";
        count++;
    }

    return;
}

int main(){

    LLVMContext context;
    SMDiagnostic error;
    unique_ptr<Module> m = parseIRFile("temp.bc", error, context);
    cout << "Name of the module: " << m->getName().str() << std::endl;

    // Iterating over member functions and processing
    for(auto iter = m->getFunctionList().begin(); iter != m->getFunctionList().end(); iter++){
        printEBBs(*iter);
    }
    return 0;
}
