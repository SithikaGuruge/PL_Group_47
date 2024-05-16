#include "Environment.h"

Environment::Environment(int id) {
    this->id = id;
}

Environment::~Environment() {
}
 //method to give a parent enviorenment to the current env.
void Environment::ParentEnvironment(Environment* parent){
    this->parent = parent;
}

/*  Look for a variable's value in the present environment; 
if the variable isn't found there, search for it in the enclosing parent environments.
*/
Control* Environment::getVarValueFromEnv(string str) {
    map<string, Control *>::iterator temp_iterator = variables_to_actualVAlues_mapping.find(str);
    
    if(variables_to_actualVAlues_mapping.end() == temp_iterator) //If the variable could not be found in the current environment,
        return parent->getVarValueFromEnv(str);
   
    else
        return (*temp_iterator).second;
}


