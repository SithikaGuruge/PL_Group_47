

#include "parser.h"
int depthhh = 0;

parser::parser(lexAnalyser* lxr){
	this->lexical_analyser = lxr;
	nxtTok = NULL;
}

parser::~parser() {
}

void parser::process_ast(){
	parse();
	printNode(stk_of_tree.top(),0);
}

void parser::process(){
	parse();
	TreeStandardizer *ts = new TreeStandardizer(stk_of_tree.top());
	CSEMachine* cs_mac = new CSEMachine();
	cs_mac->run(stk_of_tree.top());
}
// **************************************here
void parser::parse(){
	// parsing stage
		while (true) {
    nxtTok = lexical_analyser->getNextToken();
    if (nxtTok->tokType != TOK_DELETE) {
        break;
    }
}
	sym_E(); 
}

void parser::printNode(treeNode* node, int depth = 0) {
    // Print indentation
    for (int i = 0; i < depth; ++i)
        std::cout << ".";

    // Print the node
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

bool parser::chkKw(string x) {  //check whether given string is a keyword
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
     treeNode* N_new = new treeNode();
    
    N_new->nodeString = string_for_node;
    N_new->type = node_type;
	 int temp = stk_of_tree.size() ;
	 treeNode* N_temp = nullptr;
	 int total=temp - childCount + 1;
    

    if (childCount == 0) {
        stk_of_tree.push(N_new);
    	 return;
    }

    if (!stk_of_tree.empty()) {
         while (childCount > 1) {
            if (!stk_of_tree.empty()) {
                N_temp = stk_of_tree.top();
                stk_of_tree.pop();
                if (!stk_of_tree.empty()) {
                    stk_of_tree.top()->sibling = N_temp;
                } else { 
					   throw std::runtime_error("Error"); //exit the process
                }
                childCount=childCount-1;
            } else {
               return;
            }
        }
        N_temp = stk_of_tree.top();
        N_new->child = N_temp;
        stk_of_tree.pop();
    } else {
       return;
    }

    stk_of_tree.push(N_new);

   }

/* E -> ’let’ D ’in’ E                     => ’let’
     -> ’fn’ Vb+ ’.’ E                        => ’lambda’
     ->  Ew;
*/


void parser::sym_E() {
    if (nxtTok->value_token == "let") {
        peruse("let");
        sym_D();
        peruse("in");
        sym_E();
        buildLetTree();
    } else if (nxtTok->value_token == "fn") {
        peruse("fn");
        int n = 0;
        while (nxtTok->value_token == "(" || nxtTok->tokType == TOK_IDENTIFIER) {
            sym_Vb();
            n++;
        }
        peruse(".");
        sym_E();
        buildLambdaTree(n);
    } else {
        sym_Ew();
    }
}

void parser::buildLetTree() {
    constructTree("let", 2, treeNode::LET);
}

void parser::buildLambdaTree(int numParams) {
    constructTree("lambda", numParams + 1, treeNode::LAMBDA);
}

/* Ew   -> T ’where’ Dr                       => ’where’
   -> T;*/

 void parser::sym_Ew(){
	sym_T();
	string phrase="where";
 	if (nxtTok->value_token == phrase)
 	{
 		peruse(phrase);
 	 	sym_Dr();
 	 	constructTree(phrase,2, treeNode::WHERE);
 	}
 }

  /* T    -> Ta ( ’,’ Ta )+                          => ’tau’
     -> Ta ;
  */
 void parser::sym_T(){
	  int inc = 0;
	 sym_Ta();
	 if (nxtTok->value_token == ","){
		 do {
			 peruse(",");
			 sym_Ta();
			 inc=inc+1;
		 }while (nxtTok->value_token == ",");
		 constructTree("tau", inc+1, treeNode::TAU);
	 }
 }



/* Tc   -> B ’->’ Tc ’|’ Tc                      => '->'
     -> B ;
  */
 void parser::sym_Tc(){
	 	 sym_B();
	 if (nxtTok->value_token == "->"){
		 peruse("->");
		 sym_Tc();
		 peruse("|");
		 sym_Tc();
		 constructTree("->", 3, treeNode::TERNARY);
	 }
 }


/* Ta   -> Ta ’aug’ Tc                             => ’aug’
     -> Tc ;
  */
 void parser::sym_Ta(){
	 	 sym_Tc();
	 string phrase="aug";
	 while (nxtTok->value_token == phrase){
		 peruse(phrase);
		 sym_Tc();
		 constructTree(phrase, 2, treeNode::AUG);
	 }
 }


 /* Bs   -> ’not’ Bp                                => ’not’
     -> Bp 	*/
void parser::sym_Bs(){
	string phrase="not";
	if (nxtTok->value_token == phrase){
		peruse (phrase);
		sym_Bp();
		constructTree(phrase, 1, treeNode::NOT);
	}else {
		sym_Bp();
	}
}

/* B    -> B ’or’ Bt                               => ’or’
     -> Bt ;
  */
void parser::sym_B(){
		sym_Bt();
	string phrase="or";
	while (nxtTok->value_token == phrase){
		peruse (phrase);
		sym_Bt();
		constructTree(phrase, 2, treeNode::OR);
	}
}

/*  Bt  -> Bt ’&’ Bs                               => ’&’
     -> Bs 	*/
void parser::sym_Bt(){
		sym_Bs();
		string phrase="&";
	while (nxtTok->value_token == phrase){
		peruse(phrase);
		sym_Bs();
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
void parser::sym_Bp(){
		sym_A();
	if (nxtTok->value_token == "gr" || nxtTok->value_token == ">"){
		if (nxtTok->value_token == "gr")
			peruse("gr");
		else if (nxtTok->value_token == ">")
			peruse(">");
		sym_A();
		constructTree("gr", 2, treeNode::GR);
	}else if (nxtTok->value_token == "ge" || nxtTok->value_token == ">="){
		if (nxtTok->value_token == "ge")
			peruse("ge");
		else if (nxtTok->value_token == ">=")
			peruse(">=");
		sym_A();
		constructTree("ge", 2, treeNode::GE);
	}else if (nxtTok->value_token == "ls" || nxtTok->value_token == "<"){
		if (nxtTok->value_token == "ls")
			peruse("ls");
		else if (nxtTok->value_token == "<")
			peruse("<");
		sym_A();
		constructTree("ls", 2, treeNode::LS);
	}else if (nxtTok->value_token == "le" || nxtTok->value_token == "<="){
		if (nxtTok->value_token == "le")
			peruse("le");
		else if (nxtTok->value_token == "<=")
			peruse("<=");
		sym_A();
		constructTree("le", 2, treeNode::LE);
	}else if (nxtTok->value_token == "eq"){
		peruse("eq");
		sym_A();
		constructTree("eq", 2, treeNode::EQ);
	}else if (nxtTok->value_token == "ne"){
		peruse("ne");
		sym_A();
		constructTree("ne", 2, treeNode::NE);
	}
}



/* At   -> At ’*’ Af                               => ’*’
     -> At ’/’ Af                               => ’/’
     -> Af    */
void parser::sym_At(){
		string treeStr;
	sym_Af();
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
		sym_Af();
		constructTree(treeStr, 2, treeStr == str_star ? treeNode::MULTIPLY: treeNode::DIVIDE);
	}
}

/* R    -> R Rn                                    => ’gamma’
     -> Rn   */
void parser::sym_R() {
    sym_Rn();

    while ((nxtTok->tokType == TOK_IDENTIFIER || nxtTok->tokType == TOK_INTEGER ||
            nxtTok->tokType == TOK_STRING || nxtTok->value_token == "(" ||
            nxtTok->value_token == "false" || nxtTok->value_token == "true" ||
            nxtTok->value_token == "nil" || nxtTok->value_token == "dummy") && !chkKw(nxtTok->value_token)) {
        sym_Rn();
        constructTree("gamma", 2, treeNode::GAMMA);
    }
}


/* A    -> A ’+’ At                                => ’+’
     -> A ’-’ At                                => ’-’
     ->   ’+’ At
     ->   ’-’ At                                => ’neg’
     -> At 			*/
void parser::sym_A(){
		string treeStr;
	if (nxtTok->value_token == "+"){
		peruse("+");
		sym_At();
	} else if (nxtTok->value_token == "-"){
		peruse("-");
		sym_At();
		constructTree ("neg", 1, treeNode::NEG);
	} else {
		sym_At();
	}
	while (nxtTok->value_token == "+" || nxtTok->value_token == "-"){
		if (nxtTok->value_token == "+"){
			peruse("+");
			treeStr = "+";
		} else {
			peruse("-");
			treeStr = "-";
		}
		sym_At();
		constructTree(treeStr, 2, treeStr == "+" ? treeNode::ADD: treeNode::SUBTRACT);
	}
}


/* Af   -> Ap ’**’ Af                              => ’**’
     -> Ap 		*/
void parser::sym_Af(){
		sym_Ap();
		string phr="**";
	if (nxtTok->value_token == phr){
		peruse(phr);
		sym_Af();
		constructTree(phr, 2, treeNode::EXPONENTIAL);
	}
}

/* Ap   -> Ap ’@’ ’<IDENTIFIER>’ R                 => ’@’
     -> R 		*/
void parser::sym_Ap(){
		sym_R();
		string phrase="@";
	while (nxtTok->value_token == phrase){
		peruse(phrase);
		constructTree(nxtTok->value_token, 0, treeNode::IDENTIFIER);
		peruse(nxtTok->value_token);
		sym_R();
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
void parser::sym_Rn(){
	if("(" == nxtTok->value_token){
		peruse("(");
		sym_E();
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
void parser::sym_Dr(){
		string phrase="rec";
		if (nxtTok->value_token == phrase){
		peruse(phrase);
		sym_Db();
		constructTree(phrase, 1, treeNode::REC);
	} else {
		sym_Db();
	}
}

/* D    -> Da ’within’ D                           => ’within’
     -> Da   */
void parser::sym_D(){
		sym_Da();
		string phr="within";
	if (nxtTok->value_token == phr){
		peruse(phr);
		sym_D();
		constructTree(phr, 2, treeNode::WITHIN);
	}
}

/*     Da   -> Dr ( ’and’ Dr )+                        => ’and’
         -> Dr 		*/
void parser::sym_Da(){
		int inc = 0;
		string phr="and";
	sym_Dr();
	while (nxtTok->value_token == phr){
		peruse(phr);
		sym_Dr();
		inc=inc+1;
	}
	if (inc >0)
		constructTree("and", inc+1, treeNode::AND);
}



/* Db   -> Vl ’=’ E                             => ’=’
     -> ’<IDENTIFIER>’ Vb+ ’=’ E              => ’fcn_form’
     -> ’(’ D ’)’ 		*/
void parser::sym_Db() {
    if (nxtTok->value_token == "(") {
        parseParenthesizedDefinition();
    } else if (nxtTok->tokType == TOK_IDENTIFIER) {
        parseIdentifierDefinition();
    }
}

void parser::parseParenthesizedDefinition() {
    peruse("(");
    sym_D();
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
    sym_Vl();
    peruse("=");
    sym_E();
    constructTree("=", 2, treeNode::BINDING);
}

void parser::parseFunctionFormDefinition() {
    int numArgs = 0;
    do {
        sym_Vb();
        numArgs++;
    } while (nxtTok->value_token == "(" || nxtTok->tokType == TOK_IDENTIFIER);

    peruse("=");
    sym_E();
    constructTree("function_form", numArgs + 2, treeNode::FCN_FORM);
}



/*    Vb   -> ’<IDENTIFIER>’
         -> ’(’ Vl ’)’
         -> ’(’ ’)’                                 => ’()’    */

void parser::sym_Vb() {
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
        sym_Vl();
        peruse(")");
    }
}



/*   Vl   -> ’<IDENTIFIER>’ list ’,’                 => ’,’?     */
void parser::sym_Vl(){
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
