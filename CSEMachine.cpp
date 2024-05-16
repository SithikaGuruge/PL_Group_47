#include "CSEMachine.h"

//Constructor
CSEMachine::CSEMachine(){
    numEnvironment = -1;
    //create the primary enviorement and add it to the environmentStack
    PE = createEnv();
    environmentStack.push(PE);
    deltas = new vector<Control *>;
    rootDelta = new Control(Control::DELTA, deltas->size());
}

CSEMachine::CSEMachine(treeNode* topNode) {
    this->top = topNode;
}

//Destructor
CSEMachine::~CSEMachine() {
}

//Creates new environment with the next available environment number. Then returns it.
Environment* CSEMachine::createEnv(){
    tempEnvironment = new Environment(numEnvironment);
    environmentMap[numEnvironment] =  tempEnvironment;
    numEnvironment++;
    return tempEnvironment;
}

//Executes code when given root node of AST
void CSEMachine::run(treeNode *root){
    init(root); 
    ExecuteCSEM();
    if(!control.empty() || environmentStack.size() != 1){
        cout << "Control Stack or Environment Stack was not emptied"<< endl;
    }
}

//Main interpreter loop - It evaluates code and operates according to "Control" vector
void CSEMachine::ExecuteCSEM(){
    Control *temp = NULL;
    Control *currControl = NULL;
    Control *rator = NULL;
    Environment *newEnv = NULL;
    int deltaIndex = -1;

    //looped until all control items are processed and control vector is empty
    while(!control.empty()){
        currControl = control.at(control.size()-1);
        temp = NULL;
        switch(currControl->type){

        //Rule 1 - Stack a Name
        case Control::INTEGER :
        case Control::STRING :
        case Control::TRUE :
        case Control::FALSE :
        case Control::DUMMY :
        case Control::YSTAR :
            control.pop_back();
            execStack.push(currControl) ;
            break;
        case Control::NAME:
            Name(temp, currControl, rator, newEnv, deltaIndex);
            break;
        case Control::NIL :
            currControl->ctrlTuples.clear();
            control.pop_back();
            execStack.push(currControl) ;
            break ;

        //Rule 2 - Stack Lambda
        case Control::LAMBDA : 
            currControl->associatedENV = currEnvironment->id;
            control.pop_back();
            execStack.push(currControl) ;
            break ;
        case Control::GAMMA:
            Gamma(temp, currControl, rator, newEnv, deltaIndex);
            break;
        
        //Rule 5~
        case Control::ENV:
            Env(temp, currControl, rator, newEnv, deltaIndex);
            break;

        //Rule 6
        case Control::GR :
        case Control::GE :
        case Control::LS :
        case Control::LE :
        case Control::EQ :
        case Control::NE :
        case Control::AUG :
        case Control::OR :
        case Control::AND_LOGICAL :
        case Control::ADD :
        case Control::SUBTRACT :
        case Control::MULTIPLY :
        case Control::DIVIDE :
        case Control::EXP :
            applyBinaryOperator(currControl->type) ;
            break ;
        case Control::TAU:
            Tau(temp, currControl, rator, newEnv, deltaIndex);
            break ;

        //CSE Rule 7 -Handles Unary Operator NOT
        case Control::NOT : 
            control.pop_back();
            temp = execStack.top() ;
            if( temp->type == Control::TRUE || temp->type == Control::FALSE ){
                temp->type =  temp->type ==  Control::TRUE ? Control::FALSE : Control::TRUE;
            }else{
                cout << "Error: Incompatible for 'NOT'" << endl;
                exit(1) ;
            }
            break ;
        //CSE Rule 7 -Handles Unary Operator NEG
        case Control::NEG : 
            Neg(temp, currControl, rator, newEnv, deltaIndex);
        break ;
        
        //Rule 8 - Conditional
        case Control::BETA:
            Beta(temp, currControl, rator, newEnv, deltaIndex);
            break;

        //Other rules are executed within these as appropriate
        default:
            cout << "Error: " << currControl->type << endl;
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////

//Initializes, calls function to flatten AST, sets up env stack
void CSEMachine::init(treeNode *root){
    deltas->push_back(rootDelta);
    toFlattenTree(root,rootDelta, deltas);
    control.push_back(new Control(Control::ENV, 0));
    execStack.push(new Control(Control::ENV, 0));
    for(int i=0; i< rootDelta->ctrlStruct->size(); i++)
        control.push_back(rootDelta->ctrlStruct->at(i));
    environmentStack.push(createEnv());
    environmentStack.top()->ParentEnvironment(PE);
    currEnvironment = environmentStack.top();
}

//To flatten A Tree
void CSEMachine::toFlattenTree(treeNode* node, Control *delta,vector<Control *> *deltas){
    Control *temp_del_ptr = NULL;
    vector<string> *variables = NULL;
    
    //If the node is a lamda node
    if(treeNode::LAMBDA == node->type){
        toFlattenLambda(node, delta, deltas);
    }
    //If the node is a conditional operator (->)
    else if(node->type == treeNode::TERNARY){
        toFlattenTernary(node, delta, deltas);
    }
    else{
        if(node->type == treeNode::TAU){
            variables = new vector<string>();
            treeNode *temp = node->child;
            while(temp!= NULL){
                variables->push_back(temp->nodeString);
                temp = temp->sibling;
            }
        }
        delta->createControlObj(node, variables, temp_del_ptr, deltas->size());
        if(NULL != node->child){
            toFlattenTree(node->child, delta, deltas);
        }
        if(NULL != node->sibling){
            toFlattenTree(node->sibling, delta,deltas);
        }
    }
}

//Flatten lambda
void CSEMachine::toFlattenLambda(treeNode* node, Control *delta,vector<Control *> *deltas){
    Control *temp = NULL;
    vector<string> *variables = NULL;
    variables = new vector<string>();

    //lamda should have either an identifier or a comma as its first child
    if(treeNode::IDENTIFIER == node->child->type){
        variables->push_back(node->child->nodeString);
    }else if(treeNode::COMMA == node->child->type){
        treeNode *temp = node->child->child;
        while(NULL != temp){
            variables->push_back(temp->nodeString);
            temp = temp->sibling;
        }
    }else{}
    temp = new Control(Control::DELTA, deltas->size());
    deltas->push_back(temp);
    delta->createControlObj(node, variables, temp, deltas->size());
    toFlattenTree(node->child->sibling, temp, deltas);

    if(NULL != node->sibling)
        toFlattenTree(node->sibling,delta, deltas);

}

//Conditional Operator
void CSEMachine::toFlattenTernary(treeNode* node, Control *delta,vector<Control *> *deltas){
    toFlattenDThen(node, delta, deltas);

    toFlattenDElse(node, delta, deltas);

    Control *beta = new Control(Control::BETA);
    delta->ctrlStruct->push_back(new Control(Control::BETA, "beta"));
    delta->createControlObj(node->child, NULL, NULL, deltas->size());
    if(node->child->child != NULL)
    toFlattenTree(node->child->child, delta, deltas);
}

// This function flattens a delta control structure, then processes its 'then' segment.
void CSEMachine::toFlattenDThen(treeNode* node, Control *delta, vector<Control *> *deltas){
    Control *deltaThen = new Control(Control::DELTA, deltas->size());
    deltas->push_back(deltaThen);
    delta->ctrlStruct->push_back(new Control(Control::DELTA, deltas->size()-1)); 

    if(node->child->sibling->type == treeNode::TERNARY){
        toFlattenTree(node->child->sibling, deltaThen, deltas);
    } else {
        vector<string> *tempvariables = NULL;
        if(node->child->sibling->type == treeNode::TAU){
            treeNode *temp = node->child->sibling->child;
            tempvariables = new vector<string>;
            while(temp!= NULL){
                tempvariables->push_back(temp->nodeString); 
                temp = temp->sibling;
            }
        }
        // Create a new Control object representing the 'then' segment of the delta control structure
        deltaThen->createControlObj(node->child->sibling, tempvariables, deltaThen, deltas->size());
        if(node->child->sibling->child != NULL)
            toFlattenTree(node->child->sibling->child, deltaThen, deltas);
    }
}



// Flattens the delta-else segment
void CSEMachine::toFlattenDElse(treeNode* node, Control *delta, vector<Control *> *deltas) {
    Control *deltaElse = new Control(Control::DELTA, deltas->size());
    deltas->push_back(deltaElse);
    delta->ctrlStruct->push_back(new Control(Control::DELTA, deltas->size()-1));

    // Check if the node's child's sibling's sibling is of type ternary
    if (node->child->sibling->sibling->type == treeNode::TERNARY) {
        toFlattenTree(node->child->sibling->sibling, deltaElse, deltas);
    } else {
        vector<string> *tempvariables = NULL;
        if (node->child->sibling->sibling->type == treeNode::TAU) {
            treeNode *temp = node->child->sibling->sibling->child;
            tempvariables = new vector<string>;
            while (temp != NULL) {
                tempvariables->push_back(temp->nodeString);
                temp = temp->sibling;
            }
        }
        deltaElse->createControlObj(node->child->sibling->sibling, tempvariables, deltaElse, deltas->size());
        if (node->child->sibling->sibling->child != NULL)
            toFlattenTree(node->child->sibling->sibling->child, deltaElse, deltas);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

//Handles Names
void CSEMachine::Name(Control* temp, Control* currControl, Control* rator, Environment* newEnv, int deltaIndex){
    if(checkReservedIdentifier(currControl->variables.front())){
        control.pop_back();
        execStack.push(currControl) ;
    } else {
        temp = currEnvironment->getVarValueFromEnv(currControl->variables.front());
        if(temp != NULL){
            control.pop_back();
            execStack.push(temp) ;
        } else {
            cout << "Error: Name Unknown :" +  currControl->ctrlVal << endl;
        }
    }
}

//To check for inbuilt functions
bool CSEMachine::checkReservedIdentifier(string funcName) {
    if (std::find(ReservedIdentifierVector.begin(), ReservedIdentifierVector.end(), funcName) != ReservedIdentifierVector.end()){
        return true;
    } else {
        return false;
    }
}

//Handles Gamma
void CSEMachine::Gamma(Control* temp, Control* currControl, Control* rator, Environment* newEnv, int deltaIndex){
    control.pop_back();
    rator = execStack.top() ;
    execStack.pop();
    if(rator->type == Control::TUPLE){ 
        applyRule10(temp, currControl, rator, newEnv, deltaIndex);
    } else if( rator->type== Control::LAMBDA ){
        applyRule4and11(temp, currControl, rator, newEnv, deltaIndex);
    } else if( rator->type == Control::YSTAR ){
        applyRule12(temp, currControl, rator, newEnv, deltaIndex);
    } else if( rator->type == Control::ETA ){
        applyRule13(temp, currControl, rator, newEnv, deltaIndex);
    
    } else {
        applyThisRator(rator);
    }
}


//Handles env on stack
void CSEMachine::Env(Control* temp, Control* currControl, Control* rator, Environment* newEnv, int deltaIndex){
    temp = execStack.top() ;
    execStack.pop();
    if( execStack.top()->type == Control::ENV && execStack.top()->index == currControl->index ){
        control.pop_back();
        execStack.pop();
        execStack.push(temp) ;

        environmentStack.pop() ;
        currEnvironment = environmentStack.top() ;
    }else{
        cout << "Error:  in handling environment" << endl;
        exit(1) ;
    }
}


//Rule 6 - Apply Binary Operation
void CSEMachine::applyBinaryOperator(int type){

    //operand retrieval and result initialization
    control.pop_back();
    Control *rand1 = execStack.top() ;
    execStack.pop();
    Control *rand2 = execStack.top() ;
    execStack.pop();
    Control *result = new Control() ;
    result->type = Control::INTEGER; //temporary initialization

    switch(type)
    {
    
    //Handling Comparisons
    case Control::GR :
        if(rand1->type == Control::INTEGER && rand2->type == Control::INTEGER){
            if( atoi(rand1->ctrlVal.c_str()) > atoi(rand2->ctrlVal.c_str()) )
                result->type = Control::TRUE;
            else
                result->type = Control::FALSE;
            break ;
        }
        else{
            cout << "Error: Operands not compatible" << endl ;
            exit(1) ;
        }
    break ;

    case Control::GE :
    if(rand1->type == Control::INTEGER && rand2->type == Control::INTEGER) {
        if( atoi(rand1->ctrlVal.c_str()) >= atoi(rand2->ctrlVal.c_str()) )
            result->type = Control::TRUE;
        else
            result->type = Control::FALSE;
        break ;
    }
    else {
        cout << "Error: Operands not compatible" << endl  ;
        exit(1) ;
    }
    break ;

    case Control::LS :
    if(rand1->type == Control::INTEGER && rand2->type == Control::INTEGER) {
        if( atoi(rand1->ctrlVal.c_str()) < atoi(rand2->ctrlVal.c_str()) )
            result->type = Control::TRUE;
        else
            result->type = Control::FALSE;
        break ;
    }
    else {
        cout << "Error: Operands not compatible" << endl ;
        exit(1) ;
    }
    break ;
    
    case Control::LE :
    if(rand1->type == Control::INTEGER && rand2->type == Control::INTEGER) {
        if( atoi(rand1->ctrlVal.c_str()) <= atoi(rand2->ctrlVal.c_str()) )
            result->type = Control::TRUE;
        else
            result->type = Control::FALSE;
        break ;
    }
    else {
        cout << "Error: Operands not compatible" << endl  ;
        exit(1) ;
    }
    break ;
    
    case Control::EQ :
    if( rand1->type == Control::STRING && rand2->type == Control::STRING) {
        if( rand1->ctrlVal.compare(rand2->ctrlVal) == 0)
            result->type = Control::TRUE;
        else
            result->type = Control::FALSE;
    }
    else if( rand1->type == Control::INTEGER && rand2->type == Control::INTEGER ) {
        if( rand1->ctrlVal == rand2->ctrlVal )
            result->type = Control::TRUE;
        else
            result->type = Control::FALSE;
    }
    else if((rand1->type == Control::TRUE || rand1->type == Control::FALSE) && (rand1->type == Control::TRUE || rand2->type == Control::FALSE)){
        result->type = rand1->type == rand2->type?Control::TRUE: Control::FALSE;
    }
    else {
        cout << "Error: Operands not compatible" << endl  ;
        exit(1) ;
    }
    break ;
    
    case Control::NE :
    if( rand1->type == Control::STRING && rand2->type == Control::STRING ) {
        if( rand1->ctrlVal.compare(rand2->ctrlVal) != 0)
            result->type = Control::TRUE;
        else
            result->type = Control::FALSE;
    }
    else if( rand1->type == Control::INTEGER && rand2->type == Control::INTEGER ) {
        if( rand1->ctrlVal != rand2->ctrlVal)
            result->type = Control::TRUE;
        else
            result->type = Control::FALSE;
    }
    else if((rand1->type == Control::TRUE || rand1->type == Control::FALSE) && (rand1->type == Control::TRUE || rand2->type == Control::FALSE)){
        result->type = rand1->type != rand2->type?Control::TRUE: Control::FALSE;
    }
    else {
        cout << "Error: Operands not compatible'" << endl  ;
        exit(1) ;
    }
    break ;
    
    //Handling logical operations OR,AND
    case Control::OR :
    if( (rand1->type == Control::TRUE || rand1->type == Control::FALSE ) &&
        (rand2->type == Control::TRUE || rand2->type == Control::FALSE) ) {
        bool op1 = (rand1->type == Control::TRUE )? true : false ;
        bool op2 = ( rand2->type == Control::TRUE )? true : false ;
        bool res = op1 | op2 ;
        result->type =res? Control::TRUE : Control::FALSE;
    }
    else {
        cout << "Error: Operands not compatible" << endl  ;
        exit(1) ;
    }
    break ;
    
    case Control::AND_LOGICAL :
    if( (rand1->type == Control::TRUE || rand1->type == Control::FALSE ) &&
        (rand2->type == Control::TRUE || rand2->type == Control::FALSE) ) {
        bool op1 = (rand1->type == Control::TRUE)?true:false;
        bool op2 = (rand2->type == Control::TRUE)?true:false;
        bool res = op1 & op2;
        result->type = res? Control::TRUE : Control::FALSE;
    }
    else {
        cout << "Error: Operands not compatible" << endl  ;
        exit(1) ;
    }
    break ;
    
    //Handling Arithmethic Binary Operators +.-,*,/,**
    case Control::ADD :
    if(rand1->type == Control::INTEGER && rand2->type == Control::INTEGER){
        result->ctrlVal = to_string(atoi(rand1->ctrlVal.c_str()) + atoi(rand2->ctrlVal.c_str()));
        break ;
    }
    else{
        cout << "Error: Operands not compatible" << endl  ;
        exit(1) ;
    }
    break ;

    case Control::SUBTRACT :
    if(rand1->type == Control::INTEGER && rand2->type == Control::INTEGER){
        result->ctrlVal = to_string(atoi(rand1->ctrlVal.c_str()) - atoi(rand2->ctrlVal.c_str()));
        break ;
    }
    else {
        cout << "Error: Operands not compatible" << endl ;
        exit(1) ;
    }
    break ;
    
    case Control::MULTIPLY :
    if(rand1->type == Control::INTEGER && rand2->type == Control::INTEGER){
        result->ctrlVal = to_string(atoi(rand1->ctrlVal.c_str()) * atoi(rand2->ctrlVal.c_str()));
        break ;
    }
    else{
        cout << "Error: Operands not compatible" << endl ;
        exit(1) ;
    }
    break ;

    case Control::DIVIDE :
    if(rand1->type == Control::INTEGER && rand2->type == Control::INTEGER) {
        result->ctrlVal = to_string(atoi(rand1->ctrlVal.c_str()) / atoi(rand2->ctrlVal.c_str()));
        break ;
    }
    else {
        cout << "Error: Operands not compatible" << endl ;
        exit(1) ;
    }
    break ;

    case Control::EXP :
        if(rand1->type == Control::INTEGER && rand2->type == Control::INTEGER) {
            result->ctrlVal = to_string(pow(atoi(rand1->ctrlVal.c_str()), atoi(rand2->ctrlVal.c_str())));
            break ;
        }
        else {
            cout << "Error: Operands not compatible" << endl  ;
            exit(1) ;
        }
    break ;
    
    //Handling Binary Operation AUG
    case Control::AUG :
        result->type = Control::TUPLE;
        result->ctrlTuples;
        if( rand1->type != Control::NIL ){
            if( rand1->type == Control::TUPLE){
            for(int i=0;i<rand1->ctrlTuples.size(); i++)
            result->ctrlTuples.push_back(rand1->ctrlTuples.at(i));
            }
            else{
            result->ctrlTuples.push_back(rand1) ;
            }
        }
        if( rand2->type != Control::NIL ){
            if( rand2->type == Control::TUPLE){
            for(int i=0;i<rand2->ctrlTuples.size(); i++)
            result->ctrlTuples.push_back(rand2->ctrlTuples.at(i));
            }
            else
            result->ctrlTuples.push_back(rand2);
        }
    break ;

    default:
        cout << "Error: Couldn't handle binary operator  " << type << endl;
    }

    //Result is pushed to stack
    execStack.push(result) ;

}

//Handles Tau
void CSEMachine::Tau(Control* temp, Control* currControl, Control* rator, Environment* newEnv, int deltaIndex){
    control.pop_back();
    temp = new Control() ;
    temp->type = Control::TUPLE;
    temp->ctrlTuples.clear();
    for(int i=0; i<currControl->index; i++ )
    {
        temp->ctrlTuples.push_back(execStack.top()) ;
        execStack.pop() ;
    }
    execStack.push(temp) ;

}

//Handles NEG
void CSEMachine::Neg(Control* temp, Control* currControl, Control* rator, Environment* newEnv, int deltaIndex){
    control.pop_back();
    if(execStack.top()->type == Control::INTEGER )
        execStack.top()->ctrlVal = to_string(-1*atoi(execStack.top()->ctrlVal.c_str()));
    else
    {
        cout << "Error: in handling NEG" << endl;
        exit(1) ;
    }
}


//Handles Beta - Conditionals
void CSEMachine::Beta(Control* temp, Control* currControl, Control* rator, Environment* newEnv, int deltaIndex){
    control.pop_back();
    if(execStack.top()->type == Control::TRUE){
        control.pop_back();
        if(control.at(control.size() - 1)->type == Control::DELTA){
            deltaIndex = control.at(control.size() -1)->index;
            control.pop_back();
        }else{
            cout << "Error: Control type is not Delta " << endl;
            exit(1);
        }
    }else if(execStack.top()->type == Control::FALSE){
        if(control.at(control.size() - 1)->type == Control::DELTA){
            deltaIndex = control.at(control.size() -1)->index;
            control.pop_back();
            control.pop_back();
        }else{
            cout << "Error: Control type is not Delta" << endl;
            exit(1);
        }
    }else{
        cout << " Error: Boolean Value Expected"  << endl;
        exit(1);
    }
    execStack.pop();
    for(int i=0; i< deltas->at(deltaIndex)->ctrlStruct->size() ; i++){
        control.push_back(deltas->at(deltaIndex)->ctrlStruct->at(i));
    }
}


///////////////////////////////////////////////
//Following Rules are used when handling Gamma

void CSEMachine::applyRule10(Control* temp, Control* currControl, Control* rator, Environment* newEnv, int deltaIndex){
    if( execStack.top()->type == Control::INTEGER ){
    temp = rator->ctrlTuples.at(atoi(execStack.top()->ctrlVal.c_str()) - 1) ;
    execStack.pop();
    execStack.push(temp) ;
    } else{
        cout << "Error:";
        exit(1) ;
    }
}

// Function to apply Rule 4 and Rule 11 of the CSEMachine
void CSEMachine::applyRule4and11(Control* temp, Control* currControl, Control* rator, Environment* newEnv, int deltaIndex){
    newEnv = createEnv();
    newEnv->ParentEnvironment(environmentMap.find(rator->associatedENV)->second);
    currEnvironment = newEnv ;
    if( rator->variables.size() == 1 ){
        currEnvironment->variables_to_actualVAlues_mapping[rator->variables.at(0)] = execStack.top();
        execStack.pop();
    } else {
        temp = execStack.top(); 
        execStack.pop() ;
        if( temp->type == Control::TUPLE && rator->variables.size() == temp->ctrlTuples.size() )
        {
            for( int i = 0 ; i < rator->variables.size() ; i++ )
            {
                currEnvironment->variables_to_actualVAlues_mapping[rator->variables.at(i)] = temp->ctrlTuples.at(i);
            }
        } else {
            cout << "Error: "<< endl;
            exit(1);
        }
    }
    environmentStack.push(currEnvironment);
    control.push_back(new Control(Control::ENV, currEnvironment->id));
    execStack.push(new Control(Control::ENV, currEnvironment->id));
    for(int i=0;i<deltas->at(rator->index)->ctrlStruct->size();i++){
        control.push_back(deltas->at(rator->index)->ctrlStruct->at(i));
    }
}

// This function implements Rule 12 within the context of a CSEMachine.
// It involves manipulating the execution stack by creating and pushing a new ETA control.

void CSEMachine::applyRule12(Control* temp, Control* currControl, Control* rator, Environment* newEnv, int deltaIndex) {
    // Create a new ETA control copying the top control from the execution stack
    Control *eta = new Control(execStack.top());
    execStack.pop(); 
    eta->type = Control::ETA; 
    execStack.push(eta); 
}


// This function applies a specific rule, called Rule 13, within the context of a CSEMachine.
// It involves manipulating the control stack and execution stack to execute certain operations.

void CSEMachine::applyRule13(Control* temp, Control* currControl, Control* rator, Environment* newEnv, int deltaIndex) {
    // Push two GAMMA controls onto the control stack
    control.push_back(new Control(Control::GAMMA));
    control.push_back(new Control(Control::GAMMA));
    
    
    execStack.push(rator);
    
    Control *lambda = new Control(Control::LAMBDA, &(rator->variables), NULL, rator->index);
    lambda->associatedENV = rator->associatedENV;
    execStack.push(lambda);
}


//To apply built-in operators or functions
void CSEMachine::applyThisRator(Control* rator){
    Control* temp;
    Control* toPush;
    if(rator->variables.front() == "Print"){
        string print_str = execStack.top()->toStr();
        escapePrintStr(print_str); 
        execStack.pop();
        toPush = new Control(Control::DUMMY);
    }else if(rator->variables.front() == "Order"){
        Control *tuple = execStack.top() ;
        execStack.pop();
        Control *order = new Control();
        order->type = Control::INTEGER;
        if( tuple->type == Control::TUPLE){
            order->ctrlVal = to_string(tuple->ctrlTuples.size());
        }else if( tuple->type == Control::NIL ){
            order->ctrlVal = to_string(0);
        }else{
            cout <<  "Error: Invalid Input" << endl ;
            exit(1) ;
        }
        toPush = order;
    }else if(rator->variables.front() == "Isinteger"){
        temp = execStack.top();
        execStack.pop();
        toPush = new Control(temp->type == Control::INTEGER ? Control::TRUE : Control::FALSE);
    }else if(rator->variables.front() == "Istruthvalue"){
        temp = execStack.top();
        execStack.pop();
        toPush = new Control((temp->type == Control::TRUE || temp->type == Control::FALSE) ? Control::TRUE : Control::FALSE);
    }else if(rator->variables.front() == "Isstring"){
        temp = execStack.top();
        execStack.pop();
        toPush = new Control(temp->type == Control::STRING ? Control::TRUE : Control::FALSE);
    }else if(rator->variables.front() == "Istuple"){
        temp = execStack.top();
        execStack.pop();
        toPush = new Control((temp->type == Control::TUPLE || temp->type == Control::NIL) ? Control::TRUE : Control::FALSE);
    }else if(rator->variables.front() == "Isfunction"){
        temp = execStack.top();
        execStack.pop();
        toPush = new Control(temp->type == Control::LAMBDA ? Control::TRUE : Control::FALSE);
    }else if(rator->variables.front() == "Isdummy"){
        temp = execStack.top();
        execStack.pop();
        toPush = new Control(temp->type == Control::DUMMY ? Control::TRUE : Control::FALSE);
    }else if(rator->variables.front() == "Stem"){
        if(execStack.top()->type == Control::STRING){
            Control *strControl= new Control(Control::STRING);
            strControl->ctrlVal = execStack.top()->ctrlVal.substr(0, 1);
            execStack.pop();
            toPush = strControl;
        }else{
            cout << "" << endl;
            exit(1);
        }
    }else if(rator->variables.front() == "Stern"){
        if(execStack.top()->type == Control::STRING){
            Control *strControl = new Control(Control::STRING);
            strControl->ctrlVal = execStack.top()->ctrlVal.substr(1, execStack.top()->ctrlVal.length()-1);
            execStack.pop();
            toPush = strControl;
        }else{
            cout << "Error: " << endl;
            exit(1);
        }
    }else if(rator->variables.front() == "ItoS"){
        if(execStack.top()->type == Control::INTEGER){
            Control *strControl = new Control(Control::STRING);
            strControl->ctrlVal = execStack.top()->ctrlVal;
            execStack.pop();
            toPush = strControl;
        }else{
            cout << "Error: " << endl;
            exit(1);
        }
    }else if(rator->variables.front() == "Conc"){ 
        Control *concl = new Control(Control::NAME);
        concl->variables.push_back("Conclambda");
        concl->variables.push_back(execStack.top()->ctrlVal);
        execStack.pop();
        toPush = concl;
    }else if(rator->variables.front() == "Conclambda"){
        Control *concatVars = new Control(Control::STRING, rator->variables.at(1)+ execStack.top()->ctrlVal);
        execStack.pop();
        toPush = concatVars;
    }else if(rator->variables.front() == "Null"){
        Control *boolR = new Control();
        if(execStack.top()->type == Control::NIL || (execStack.top()->type == Control::TUPLE && execStack.top()->ctrlTuples.empty()))
            boolR->type = Control::TRUE;
        else
            boolR->type = Control::FALSE;
        execStack.pop();
        toPush = boolR;
    }else{
        cout << "ERROR: value:%" << rator->ctrlVal << "%type:" << rator->type << endl;
        //printCS();   /////////////
        return;
    }
    execStack.push(toPush);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////

// This function takes a string as input and prints it to the console, 
// while interpreting escape sequences '\n' as newline and '\t' as tab.

void CSEMachine::escapePrintStr(string printStr) {
  for (int i = 0; i < printStr.length(); i++) {
    char ch1 = printStr.at(i);
    if (ch1 == '\\') {
      i++; 
      if (i < printStr.length()) {
        char ch2 = printStr.at(i);
        if (ch2 == 'n')
          cout << endl; 
        else if (ch2 == 't')
          cout << "\t"; 
        else
          cout << ch1 << ch2; 
      }
    } else {

      cout << ch1;
    }
  }
  cout.flush();
}








