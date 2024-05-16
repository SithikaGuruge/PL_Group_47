
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


	void procedure_E();
	void procedure_Ew();
	void procedure_T();
	void procedure_Ta();
	void procedure_Tc();
	void procedure_B();
	void procedure_Bt();
	void procedure_Bs();
	void procedure_Bp();
	void procedure_A();
	void procedure_At();
	void procedure_Af();
	void procedure_Ap();
	void procedure_R();
	void procedure_Rn();
	void procedure_D();
	void procedure_Da();
	void procedure_Dr();
	void procedure_Db();
	void procedure_Vb();
	void procedure_Vl();

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
