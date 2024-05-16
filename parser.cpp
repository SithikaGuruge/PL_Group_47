#include "parser.h"
int depthhh = 0;

parser::parser(lexAnalyser* lxr){
	this->lexical_analyser = lxr;
	nxtTok = NULL;
}

parser::~parser() {
}

// process the printing abstract syntax tree
void parser::process_ast(){
	parse();
	printNode(stk_of_tree.top(),0);
}

// process the printing the value
void parser::process(){
	parse();
	TreeStandardizer *ts = new TreeStandardizer(stk_of_tree.top());
	CSEMachine* cs_mac = new CSEMachine();
	cs_mac->run(stk_of_tree.top());
}

void parser::parse(){
	// parsing stage
		while (true) {
    nxtTok = lexical_analyser->getNextToken();
    if (nxtTok->tokType != TOK_DELETE) {
        break;
    }
}
	procedure_E(); 
}

void parser::printNode(treeNode* node, int depth = 0) {
    // Print indentation
    for (int i = 0; i < depth; ++i)
        std::cout << ".";

    // Print the node
	std::cout << node->type << " ";
    std::cout << node->nodeString << std::endl;

    // Print children
    if (node->child) {
        printNode(node->child, depth + 1);
    }
    if (node->sibling) {
        printNode(node->sibling, depth);
    }
}




#include <unordered_set> // Include the required header for std::unordered_set

bool parser::check_keyword(string x) {  //check whether given string is a keyword
    static const std::unordered_set<string> keywords = {
        "in", "where", ".", "aug", "and", "or", "&", "not", "gr", "ge", "ls", "le",
        "eq", "ne", "+", "-", "*", "/", "**", "@", "within", "rec", "let", "fn"
    };

    return keywords.find(x) != keywords.end();
}



void parser::peruse(string str) {
	// reading and checking with the next token
    if (nxtTok->value_token != str) {
        throw std::runtime_error("Expected '" + str + "', but found '" + nxtTok->value_token + "'");
    }

   
   nxtTok = lexical_analyser->getNextToken();
while (nxtTok->tokType == TOK_DELETE) {
    nxtTok = lexical_analyser->getNextToken();
}

}



void parser::constructTree(string string_for_node, int childCount, int node_type) {
	// constructing the syntax tree
     treeNode* new_node = new treeNode();
    
    new_node->nodeString = string_for_node;
    new_node->type = node_type;
	int stack_size = stk_of_tree.size() ;
	treeNode* temporary_Node = nullptr;
	int total=stack_size - childCount + 1;
    

    if (childCount == 0) {
        stk_of_tree.push(new_node);
    	 return;
    }

    if (!stk_of_tree.empty()) {
         while (childCount > 1) {
            if (!stk_of_tree.empty()) {
                temporary_Node = stk_of_tree.top();
                stk_of_tree.pop();
                if (!stk_of_tree.empty()) {
                    stk_of_tree.top()->sibling = temporary_Node;
                } else { 
					   throw std::runtime_error("Error"); //exit the process
                }
                childCount=childCount-1;
            } else {
               return;
            }
        }
        temporary_Node = stk_of_tree.top();
        new_node->child = temporary_Node;
        stk_of_tree.pop();
    } else {
       return;
    }

    stk_of_tree.push(new_node);

   }

/* E -> ’let’ D ’in’ E                     => ’let’
     -> ’fn’ Vb+ ’.’ E                     => ’lambda’
     ->  Ew;
*/


void parser::procedure_E() {
    if (nxtTok->value_token == "let") {
        peruse("let");
        procedure_D();
        peruse("in");
        procedure_E();
        buildLetTree();
    } else if (nxtTok->value_token == "fn") {
        peruse("fn");
        int n = 0;
        while (nxtTok->value_token == "(" || nxtTok->tokType == TOK_IDENTIFIER) {
            procedure_Vb();
            n++;
        }
        peruse(".");
        procedure_E();
        buildLambdaTree(n);
    } else {
        procedure_Ew();
    }
}

void parser::buildLetTree() {
    constructTree("let", 2, treeNode::LET);
}

void parser::buildLambdaTree(int numberOfArgs) {
    constructTree("lambda", numberOfArgs + 1, treeNode::LAMBDA);
}

/* Ew   -> T ’where’ Dr                       => ’where’
   		-> T;	*/

 void parser::procedure_Ew(){
	procedure_T();
	string phrase="where";
 	if (nxtTok->value_token == phrase)
 	{
 		peruse(phrase);
 	 	procedure_Dr();
 	 	constructTree(phrase,2, treeNode::WHERE);
 	}
 }

  /* T    	-> Ta ( ’,’ Ta )+                          => ’tau’
     		-> Ta ;
  */
 void parser::procedure_T(){
	  int inc = 0;
	 procedure_Ta();
	 if (nxtTok->value_token == ","){
		 do {
			 peruse(",");
			 procedure_Ta();
			 inc=inc+1;
		 }while (nxtTok->value_token == ",");
		 constructTree("tau", inc+1, treeNode::TAU);
	 }
 }



/* Tc   -> B ’->’ Tc ’|’ Tc                      => '->'
     	-> B ;
  */
 void parser::procedure_Tc(){
	 	 procedure_B();
	 if (nxtTok->value_token == "->"){
		 peruse("->");
		 procedure_Tc();
		 peruse("|");
		 procedure_Tc();
		 constructTree("->", 3, treeNode::TERNARY);
	 }
 }


/* Ta   -> Ta ’aug’ Tc                             => ’aug’
     	-> Tc ;
  */
 void parser::procedure_Ta(){
	 	 procedure_Tc();
	 string phrase="aug";
	 while (nxtTok->value_token == phrase){
		 peruse(phrase);
		 procedure_Tc();
		 constructTree(phrase, 2, treeNode::AUG);
	 }
 }


 /* Bs   -> ’not’ Bp                                => ’not’
     	 ->  Bp 	*/

void parser::procedure_Bs(){
	string phrase="not";
	if (nxtTok->value_token == phrase){
		peruse (phrase);
		procedure_Bp();
		constructTree(phrase, 1, treeNode::NOT);
	}else {
		procedure_Bp();
	}
}

/* 	B    -> B ’or’ Bt                               => ’or’
    	 -> Bt ;
  */
void parser::procedure_B(){
		procedure_Bt();
	string phrase="or";
	while (nxtTok->value_token == phrase){
		peruse (phrase);
		procedure_Bt();
		constructTree(phrase, 2, treeNode::OR);
	}
}

/*  Bt  -> Bt ’&’ Bs                               => ’&’
     	-> Bs 	*/
void parser::procedure_Bt(){
		procedure_Bs();
		string phrase="&";
	while (nxtTok->value_token == phrase){
		peruse(phrase);
		procedure_Bs();
		constructTree(phrase, 2, treeNode::AND_LOGICAL);
	}
}



/* Bp   -> A (’gr’ | ’>’ ) A                       => ’gr’
     	-> A (’ge’ | ’>=’) A                       => ’ge’
     	-> A (’ls’ | ’<’ ) A                       => ’ls’
     	-> A (’le’ | ’<=’) A                       => ’le’
     	-> A ’eq’ A                                => ’eq’
     	-> A ’ne’ A                                => ’ne’
     	-> A 		*/

void parser::procedure_Bp(){
		procedure_A();
	if (nxtTok->value_token == "gr" || nxtTok->value_token == ">"){
		if (nxtTok->value_token == "gr")
			peruse("gr");
		else if (nxtTok->value_token == ">")
			peruse(">");
		procedure_A();
		constructTree("gr", 2, treeNode::GR);
	}else if (nxtTok->value_token == "ge" || nxtTok->value_token == ">="){
		if (nxtTok->value_token == "ge")
			peruse("ge");
		else if (nxtTok->value_token == ">=")
			peruse(">=");
		procedure_A();
		constructTree("ge", 2, treeNode::GE);
	}else if (nxtTok->value_token == "ls" || nxtTok->value_token == "<"){
		if (nxtTok->value_token == "ls")
			peruse("ls");
		else if (nxtTok->value_token == "<")
			peruse("<");
		procedure_A();
		constructTree("ls", 2, treeNode::LS);
	}else if (nxtTok->value_token == "le" || nxtTok->value_token == "<="){
		if (nxtTok->value_token == "le")
			peruse("le");
		else if (nxtTok->value_token == "<=")
			peruse("<=");
		procedure_A();
		constructTree("le", 2, treeNode::LE);
	}else if (nxtTok->value_token == "eq"){
		peruse("eq");
		procedure_A();
		constructTree("eq", 2, treeNode::EQ);
	}else if (nxtTok->value_token == "ne"){
		peruse("ne");
		procedure_A();
		constructTree("ne", 2, treeNode::NE);
	}
}



/* At   -> At ’*’ Af                               => ’*’
     	-> At ’/’ Af                               => ’/’
     	-> Af    */

void parser::procedure_At(){
		string treeStr;
	procedure_Af();
	string str_star="*";
	string str_slash="/";
	while(nxtTok->value_token == str_star || nxtTok->value_token == str_slash){
		if (nxtTok->value_token ==str_star){
			peruse(str_star);
			treeStr = str_star;
		} else {
			peruse(str_slash);
			treeStr = str_slash;
		}
		procedure_Af();
		constructTree(treeStr, 2, treeStr == str_star ? treeNode::MULTIPLY: treeNode::DIVIDE);
	}
}

/* R    -> R Rn                                    => ’gamma’
     	-> Rn   */

void parser::procedure_R() {
    procedure_Rn();

    while ((nxtTok->tokType == TOK_IDENTIFIER || nxtTok->tokType == TOK_INTEGER ||
            nxtTok->tokType == TOK_STRING || nxtTok->value_token == "(" ||
            nxtTok->value_token == "false" || nxtTok->value_token == "true" ||
            nxtTok->value_token == "nil" || nxtTok->value_token == "dummy") && !check_keyword(nxtTok->value_token)) {
        procedure_Rn();
        constructTree("gamma", 2, treeNode::GAMMA);
    }
}


/* A    -> A ’+’ At                                => ’+’
     	-> A ’-’ At                                => ’-’
     	->   ’+’ At
     	->   ’-’ At                                => ’neg’
     	-> At 			*/

void parser::procedure_A(){
		string treeStr;
	if (nxtTok->value_token == "+"){
		peruse("+");
		procedure_At();
	} else if (nxtTok->value_token == "-"){
		peruse("-");
		procedure_At();
		constructTree ("neg", 1, treeNode::NEG);
	} else {
		procedure_At();
	}
	while (nxtTok->value_token == "+" || nxtTok->value_token == "-"){
		if (nxtTok->value_token == "+"){
			peruse("+");
			treeStr = "+";
		} else {
			peruse("-");
			treeStr = "-";
		}
		procedure_At();
		constructTree(treeStr, 2, treeStr == "+" ? treeNode::ADD: treeNode::SUBTRACT);
	}
}


/* Af   -> Ap ’**’ Af                              => ’**’
     	-> Ap 		*/

void parser::procedure_Af(){
		procedure_Ap();
		string phr="**";
	if (nxtTok->value_token == phr){
		peruse(phr);
		procedure_Af();
		constructTree(phr, 2, treeNode::EXPONENTIAL);
	}
}

/* Ap   -> Ap ’@’ ’<IDENTIFIER>’ R                 => ’@’
     	-> R 		*/

void parser::procedure_Ap(){
		procedure_R();
		string phrase="@";
	while (nxtTok->value_token == phrase){
		peruse(phrase);
		constructTree(nxtTok->value_token, 0, treeNode::IDENTIFIER);
		peruse(nxtTok->value_token);
		procedure_R();
		constructTree(phrase, 3, treeNode::AT);
	}
}





/* Rn   -> ’<IDENTIFIER>’
     	-> ’<INTEGER>’
     	-> ’<STRING>’
     	-> ’true’                                  => ’true’
     	-> ’false’                                 => ’false’
     	-> ’nil’                                   => ’nil’
     	-> ’(’ E ’)’
     	-> ’dummy’                                 => ’dummy’  */

void parser::procedure_Rn(){
	if("(" == nxtTok->value_token){
		peruse("(");
		procedure_E();
		peruse(")");
	}
	else if(TOK_STRING == nxtTok->tokType||TOK_IDENTIFIER == nxtTok->tokType || TOK_INTEGER == nxtTok->tokType){
		string str_t="true";
		string str_f="false";
		string str_n="nil";
		string str_d="dummy";
		if(str_t== nxtTok->value_token){
			peruse(str_t);
			constructTree("<true>", 0, treeNode::TRUE);
		}
		else if(str_f == nxtTok->value_token){
			peruse(str_f);
			constructTree("<false>", 0, treeNode::FALSE);
		}
		else if(str_n == nxtTok->value_token){
			peruse(str_n);
			constructTree("<nil>", 0, treeNode::NIL);
		}
		else if(str_d == nxtTok->value_token){
			peruse(str_d);
			constructTree("<dummy>",0, treeNode::DUMMY);
		}
		else if(TOK_IDENTIFIER == nxtTok->tokType){
			constructTree(nxtTok->value_token, 0, treeNode::IDENTIFIER);
			peruse(nxtTok->value_token);
			
		}else if(TOK_INTEGER == nxtTok->tokType){
			constructTree(nxtTok->value_token, 0, treeNode::INTEGER);
			peruse(nxtTok->value_token);
		}
		else if(TOK_STRING == nxtTok->tokType){
			constructTree(nxtTok->value_token, 0, treeNode::STRING);
			peruse(nxtTok->value_token);
		}
		
	}
}

/*      Dr   -> ’rec’ Db                                => ’rec’
         	 -> Db 			*/

void parser::procedure_Dr(){
		string phrase="rec";
		if (nxtTok->value_token == phrase){
		peruse(phrase);
		procedure_Db();
		constructTree(phrase, 1, treeNode::REC);
	} else {
		procedure_Db();
	}
}

/* D    -> Da ’within’ D                           => ’within’
     	-> Da   */

void parser::procedure_D(){
		procedure_Da();
		string phr="within";
	if (nxtTok->value_token == phr){
		peruse(phr);
		procedure_D();
		constructTree(phr, 2, treeNode::WITHIN);
	}
}

/*     Da   -> Dr ( ’and’ Dr )+                        => ’and’
         	-> Dr 		*/

void parser::procedure_Da(){
		int inc = 0;
		string phr="and";
	procedure_Dr();
	while (nxtTok->value_token == phr){
		peruse(phr);
		procedure_Dr();
		inc=inc+1;
	}
	if (inc >0)
		constructTree("and", inc+1, treeNode::AND);
}



/* Db   -> Vl ’=’ E                             => ’=’
     	-> ’<IDENTIFIER>’ Vb+ ’=’ E             => ’fcn_form’
     	-> ’(’ D ’)’ 		*/

void parser::procedure_Db() {
    if (nxtTok->value_token == "(") {
        parseParenthesizedDefinition();
    } else if (nxtTok->tokType == TOK_IDENTIFIER) {
        parseIdentifierDefinition();
    }
}

void parser::parseParenthesizedDefinition() {
    peruse("(");
    procedure_D();
    peruse(")");
}

void parser::parseIdentifierDefinition() {
    constructTree(nxtTok->value_token, 0, treeNode::IDENTIFIER);
    peruse(nxtTok->value_token);

    if (nxtTok->value_token == "," || nxtTok->value_token == "=") {
        parseBindingDefinition();
    } else {
        parseFunctionFormDefinition();
    }
}

void parser::parseBindingDefinition() {
    procedure_Vl();
    peruse("=");
    procedure_E();
    constructTree("=", 2, treeNode::BINDING);
}

void parser::parseFunctionFormDefinition() {
    int numArgs = 0;
    do {
        procedure_Vb();
        numArgs++;
    } while (nxtTok->value_token == "(" || nxtTok->tokType == TOK_IDENTIFIER);

    peruse("=");
    procedure_E();
    constructTree("fcn_form", numArgs + 2, treeNode::FCN_FORM);
}



/*    Vb   	-> ’<IDENTIFIER>’
         	-> ’(’ Vl ’)’
         	-> ’(’ ’)’                                 => ’()’    */

void parser::procedure_Vb() {
    if (nxtTok->tokType == TOK_IDENTIFIER) {
        parseIdentifier();
    } else if (nxtTok->value_token == "(") {
        parseParenthesizedVb();
    }
}

void parser::parseIdentifier() {
    constructTree(nxtTok->value_token, 0, treeNode::IDENTIFIER);
    peruse(nxtTok->value_token);
}


void parser::parseParenthesizedVb() {
    peruse("(");
    if (nxtTok->value_token == ")") {
        peruse(")");
        constructTree("()", 0, treeNode::PARANTHESES);
    } else if (nxtTok->tokType == TOK_IDENTIFIER) {
        constructTree(nxtTok->value_token, 0, treeNode::IDENTIFIER);
        peruse(nxtTok->value_token);
        procedure_Vl();
        peruse(")");
    }
}



/*   Vl   -> ’<IDENTIFIER>’ list ’,’                 => ’,’?     */

void parser::procedure_Vl(){
		int inc = 0;
	while (nxtTok->value_token == ","){
		peruse(",");
		if (nxtTok->tokType == TOK_IDENTIFIER){
			constructTree(nxtTok->value_token, 0, treeNode::IDENTIFIER);
			peruse(nxtTok->value_token);
			inc=inc+1;
		} else {
			printf ("ERROR In Vl()\n");
		}
	}
		if (inc > 0){
		constructTree(",", inc+1, treeNode::COMMA);
		}
}
