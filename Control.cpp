
#include "Control.h"

/*Generate a Control object (comprising mainly the type and value of the node) for a specified node within the Standardize Tree,
subsequently incorporating it into the appropriate control structure (e.g., delta 0, delta 1, delta 2, etc.).
*/

void Control::createControlObj(treeNode *node, vector<string> *variables, Control *del_ptr, int deltas_size)
{
  Control *control_object = NULL;
  if (node->type == treeNode::LAMBDA)
  {
    control_object = new Control(Control::LAMBDA, variables, del_ptr, deltas_size - 1);
  }

  else if (node->type == treeNode::ADD)
  {
    control_object = new Control(Control::ADD, node->nodeString);
  }
  else if (node->type == treeNode::OR)
  {
    control_object = new Control(Control::OR);
  }

  else if (node->type == treeNode::IDENTIFIER)
  {
    control_object = new Control(node->nodeString);
  }

  else if (node->type == treeNode::INTEGER)
  {
    control_object = new Control(Control::INTEGER, node->nodeString);
  }

  else if (node->type == treeNode::AND_LOGICAL)
  {
    control_object = new Control(Control::AND_LOGICAL);
  }

  else if (node->type == treeNode::SUBTRACT)
  {
    control_object = new Control(Control::SUBTRACT, node->nodeString);
  }

  else if (node->type == treeNode::MULTIPLY)
  {
    control_object = new Control(Control::MULTIPLY, node->nodeString);
  }

  else if (node->type == treeNode::DIVIDE)
  {
    control_object = new Control(Control::DIVIDE, node->nodeString);
  }

  else if (node->type == treeNode::GAMMA)
  {
    control_object = new Control(Control::GAMMA, node->nodeString);
  }

  else if (node->type == treeNode::STRING)
  {
    control_object = new Control(Control::STRING, node->nodeString.substr(1, node->nodeString.length() - 2));
  }

  else if (node->type == treeNode::AUG)
  {
    control_object = new Control(Control::AUG);
  }

  else if (node->type == treeNode::LS)
  {
    control_object = new Control(Control::LS);
  }

  else if (node->type == treeNode::LE)
  {
    control_object = new Control(Control::LE);
  }

  else if (node->type == treeNode::YSTAR)
  {
    control_object = new Control(Control::YSTAR);
  }

  else if (node->type == treeNode::NE)
  {
    control_object = new Control(Control::NE);
  }
  else if (node->type == treeNode::EQ)
    control_object = new Control(Control::EQ);

  else if (node->type == treeNode::TAU)
  {
    int tau_count;
    if (variables != NULL)
      tau_count = variables->size();
    control_object = new Control(Control::TAU, tau_count);
  }

  else if (node->type == treeNode::GR)
  {
    control_object = new Control(Control::GR);
  }

  else if (node->type == treeNode::GE)
  {
    control_object = new Control(Control::GE);
  }

  else if (node->type == treeNode::NEG)
  {
    control_object = new Control(Control::NEG);
  }

  else if (node->type == treeNode::DUMMY)
  {
    control_object = new Control(Control::DUMMY);
  }
  else if (node->type == treeNode::FALSE)
  {
    control_object = new Control(Control::FALSE);
  }

  else if (node->type == treeNode::NIL)
  {
    control_object = new Control(Control::NIL);
  }
  else if (node->type == treeNode::TRUE)
  {
    control_object = new Control(Control::TRUE);
  }

  else if (node->type == treeNode::NOT)
  {
    control_object = new Control(Control::NOT);
  }

  ctrlStruct->push_back(control_object);
}

string Control::toStr()
{
  string temp;
  switch (type)
  {
  case GAMMA:
    return "Gamma";
  case AUG:
    return "AUG";
  case BETA:
    return "BETA";
  case OR:
    return "OR";
  case AND_LOGICAL:
    return "AND";
  case NOT:
    return "NOT";
  case GR:
    return ">";
  case GE:
    return ">=";
  case LS:
    return "<";
  case LE:
    return "<=";
  case EQ:
    return "=";
  case NE:
    return "!=";
  case ADD:
    return "+";
  case SUBTRACT:
    return "-";
  case NEG:
    return "NEG";
  case MULTIPLY:
    return "*";
  case DIVIDE:
    return "/";
  case EXP:
    return "**";
  case AT:
    return "@";
  case TRUE:
    return "true";
  case FALSE:
    return "false";
  case NIL:
    return "nil";
  case DUMMY:
    return "dummy";
  case YSTAR:
    return "Y";
  case TAU:
    temp = "<T" + to_string(index) + ">";
    return temp;
  case STRING:
    return ctrlVal + "";
  case INTEGER:
    temp = ctrlVal + "";
    return temp;
  case DELTA:
    temp = "<D" + std::to_string(index) + ">";
    return temp;
  case NAME:
    return variables.at(0);
  case LAMBDA:
    temp = "[lambda closure: ";
    for (int i = 0; i < variables.size(); i++)
      temp += variables.at(i) + ": ";
    temp += to_string(index) + "]";
    return temp;
  case ENV:
    temp = "e" + to_string(index);
    return temp;
  case ETA:
    temp = "<ETA," + to_string(index);
    for (int i = 0; i < variables.size(); i++)
      temp += "," + variables.at(i);
    temp += ">";
    return temp;
  case TUPLE:
    temp = "(";
    for (int i = 0; i < ctrlTuples.size(); i++)
    {
      temp += ctrlTuples.at(i)->toStr();
      if (i != ctrlTuples.size() - 1)
        temp += ", ";
    }
    temp += ")";
    return temp;
  default:
    return "Error";
  }
}

Control::Control()
{
}

// Special constructor class for creating control objects variable names
Control::Control(string var_value)
{
  this->type = Control::NAME;
  variables.push_back(var_value);
}

// Create control objects for delta or tau Nodes
Control::Control(Control::Type type, int index)
{
  if (Control::DELTA == type)
  {
    this->type = type; // DELTA
    this->index = index;
    ctrlStruct = new vector<Control *>();
  }
  else if (type == Control::TAU | type == Control::ENV)
  {
    this->type = type;
    this->index = index;
  }
}
// Create control objects for lamda Nodes
Control::Control(Control::Type type, vector<string> *vars, Control *del_ptr, int deltaindex)
{
  this->type = type;
  index = deltaindex;
  if (vars != NULL)
  {
    for (int i = 0; i < vars->size(); i++)
    {
      variables.push_back(vars->at(i));
    }
  }
}

// Create control objects for other types of nodes
Control::Control(Control::Type type, string value)
{
  this->type = type;
  ctrlVal = value;
}

Control::Control(Control::Type type)
{
  this->type = type;
}

// This method is used to create a copy of a given control object
Control::Control(Control *cntrlObj)
{
  // Copy the value of the control object
  ctrlVal = cntrlObj->ctrlVal;
  // Copy the type of the control object
  type = cntrlObj->type;
  // Copy the enviorenment number to which the control object belongs
  associatedENV = cntrlObj->associatedENV;
  index = cntrlObj->index;

  // create a copy of the variable names in the control objects
  if (!cntrlObj->variables.empty())
  {
    for (int i = 0; i < cntrlObj->variables.size(); i++)
      variables.push_back(cntrlObj->variables.at(i));
  }
  // create a copy of tuples in the control object
  if (!cntrlObj->ctrlTuples.empty())
  {
    for (int i = 0; i < cntrlObj->ctrlTuples.size(); i++)
      ctrlTuples.push_back(cntrlObj->ctrlTuples.at(i));
  }
}
