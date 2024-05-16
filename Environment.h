#ifndef RPAL_COMPILER_ENVIRONMENT_H_
#define RPAL_COMPILER_ENVIRONMENT_H_

#include <map>
#include <string>
#include "Control.h"

using namespace std;

class Environment {
public:
    //Enviorenent number (delta 0,delta 1,delta 2,...)
    int id;
    //Parent env of the current env.
    Environment *parent;
    map<string, Control *> variables_to_actualVAlues_mapping;

    Environment(int);
    virtual ~Environment();
    
    void ParentEnvironment(Environment* );
    Control* getVarValueFromEnv(string);
    
    
    
};

#endif 
