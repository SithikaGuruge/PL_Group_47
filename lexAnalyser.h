#ifndef LEXER_H_
#define LEXER_H_
using namespace std;
#define LEXLOGS 0
#define PARSERLOGS 0
#define STANDARDIZERLOGS 0
#define MAIN_DEBUG_LOGS 0

#include <fstream>
#include <string>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include "tokenData.h"

class lexAnalyser {
public:
	lexAnalyser(std::ifstream*);
	virtual ~lexAnalyser();
	tokenData* getNextToken();
	void lexerReset();

private:
    ifstream* Rpalfile;
    int lineCount, charCount;
    string tokenIdentifier();
    string tokenInteger();
    string tokenStrings();
    string tokenSpaces();
    string tokenComment();
    string tokenOperator();
    

	bool isPunction(char);
	bool isOperatorSymbol(char);
	bool isCommentChar(char);
	
};

#endif
