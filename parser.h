
#ifndef PARSER_H_
#define PARSER_H_
#include "lexAnalyser.h"
#include <stack>
#include "TreeStandardizer.h"
#include "CSEMachine.h"

using namespace std;

class parser {
public:

	parser (lexAnalyser*);
    void process();
	void process_ast();
	virtual ~parser();
	void printNode(treeNode*,int);
	

private:
    lexAnalyser* lexical_analyser;
    stack <treeNode*> stk_of_tree;
    tokenData *nxtTok;


	void sym_E();
	void sym_Ew();
	void sym_T();
	void sym_Ta();
	void sym_Tc();
	void sym_B();
	void sym_Bt();
	void sym_Bs();
	void sym_Bp();
	void sym_A();
	void sym_At();
	void sym_Af();
	void sym_Ap();
	void sym_R();
	void sym_Rn();
	void sym_D();
	void sym_Da();
	void sym_Dr();
	void sym_Db();
	void sym_Vb();
	void sym_Vl();

	void parse();
	void standardize(treeNode*);
    void constructTree(string, int); 
    void constructTree(string, int, int); 
	void buildLetTree();
	void buildLambdaTree(int);
    void peruse(string); 
	bool chkKw(string); 

	void parseParenthesizedDefinition();
	void parseIdentifierDefinition();
	void parseBindingDefinition();
	void parseFunctionFormDefinition();
	void parseIdentifier();
	void parseParenthesizedVb();
};

#endif 
