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

/*   Search a value of a variable in the current enviorenment and
     if that variable cannot be found in the current env. search for it in the parent enviorenments  
*/
Control* Environment::getVarValueFromEnv(string str) {
    map<string, Control *>::iterator temp_iterator = variables_to_actualVAlues_mapping.find(str);
    //If the variable could not be found
    if(variables_to_actualVAlues_mapping.end() == temp_iterator)
        return parent->getVarValueFromEnv(str);
    //If the symbol is found in the current environment,
    else
        return (*temp_iterator).second;
}


