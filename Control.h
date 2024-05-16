#include "lexAnalyser.h"
#include "treeNode.h"
#include <string>
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <vector>

class Control{    //define enum values for the control objects
public:
    enum Type{
    ENV = 1 ,
    DELTA = 2 ,
    NAME = 3 ,
    LAMBDA = 4 ,
    GAMMA = 5 ,
    AUG = 6 ,
    BETA = 7 ,
    OR = 8 ,
    AND_LOGICAL = 9 ,
    NOT = 10 ,
    GR = 11 ,
    GE = 12 ,
    LS = 13 ,
    LE = 14 ,
    EQ = 15 ,
    NE = 16 ,
    ADD = 17 ,
    SUBTRACT = 18 ,
    NEG = 19 ,
    MULTIPLY = 20 ,
    DIVIDE = 21 ,
    EXP = 22 ,
    AT = 23 ,
    TRUE = 24 ,
    FALSE = 25 ,
    NIL = 26 ,
    DUMMY = 27 ,
    YSTAR = 28 ,
    ETA = 29 ,
    TAU = 30 ,
    STRING = 31 ,
    INTEGER = 32 ,
    TUPLE = 33
    };
  
  
  Type type; //Type of the control object
 
  string ctrlVal;  //Value of the control object
  
  int associatedENV; //Enviorenment that the control object belongs to
  int index;
  
  vector<Control *> *ctrlStruct;
  vector<string> variables;
  vector<Control *> ctrlTuples;
  Control *delta;

  string toStr();
  Control();
  Control( Control *cntrl);
  Control(Control::Type type, int index);
  Control(Control::Type type, string value);
  Control(Control::Type type);

  Control(Control::Type type,vector<string> *variables, Control *del_ptr, int delta_index);
  Control(string var_value);

  void createControlObj(treeNode* node, vector<string> *, Control*, int);
  
  

};
