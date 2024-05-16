
#ifndef RPAL_COMPILER_CSEMACHINE_H_    
#define RPAL_COMPILER_CSEMACHINE_H_   
#include <vector>
#include <stack>
#include <cmath>
#include <cstring>
#include <algorithm>  
#include "Environment.h"

using namespace std;

class CSEMachine {

public:
    CSEMachine();
    CSEMachine(treeNode*);
    virtual ~CSEMachine();
    Environment* primitiveEnv;
    void run(treeNode*);
    Environment* createEnv();
    void ExecuteCSEM();

private:
    treeNode* top;
    vector<Control *> *deltas;
    Control *rootDelta;
    int numEnvironment;

    /* When running the control structures we use the following 3 items
    ------------------------------
    control | stack | Environment
    --------------------------------
    */

    //control
    vector<Control *> control;

    //stack
    stack<Control *> execStack;

    //environment
    Environment *currEnvironment;
    Environment *tempEnvironment;

    //Primary Environment (First environment in the enviornment stack)
    Environment *PE;

    //stack of environments
    stack<Environment*> environmentStack;
    map<int, Environment *> environmentMap;


    void init(treeNode*);
    void toFlattenTree(treeNode*, Control *,vector<Control *> *);
    void toFlattenLambda(treeNode*, Control *,vector<Control *> *);
    void toFlattenTernary(treeNode*, Control *,vector<Control *> *);
    void toFlattenDThen(treeNode*, Control *,vector<Control *> *);
    void toFlattenDElse(treeNode*, Control *,vector<Control *> *);
    
    void Name(Control*, Control*, Control*, Environment*, int);
    bool checkReservedIdentifier(string);
    void Gamma(Control*, Control*, Control*, Environment*, int);
    void Env(Control*, Control*, Control*, Environment*, int);
    void applyBinaryOperator(int);
    void Tau(Control*, Control*, Control*, Environment*, int);
    void Neg(Control*, Control*, Control*, Environment*, int);
    void Beta(Control*, Control*, Control*, Environment*, int);
    
    void applyRule10(Control*, Control*, Control*, Environment*, int);
    void applyRule4and11(Control*, Control*, Control*, Environment*, int);
    void applyRule12(Control*, Control*, Control*, Environment*, int);
    void applyRule13(Control*, Control*, Control*, Environment*, int);
    void applyThisRator(Control*);
    
    void escapePrintStr(string);
    
};

//Inbult functions in RPAL such as Isinteger ,Stern etc.
static vector <string> ReservedIdentifierVector = {"Order","Print","Isinteger", "Istruthvalue","Isstring","Istuple","Isfunction""Isdummy","Stem","Stern","Conc","Conc2","ItoS","Null"};


#endif /* RPAL_COMPILER_CSEMACHINE_H_ */                 
