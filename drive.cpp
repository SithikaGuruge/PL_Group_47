#include <stdio.h>
#include <string.h>
#include <fstream>
#include "lexAnalyser.h"
#include "parser.h"

using namespace std;

ifstream src;


void exec() {
	// get the output of lexical analyser and start parsing
	lexAnalyser* rpalLexer = new lexAnalyser(&src);
	parser* rpalParser = new parser(rpalLexer);
	 rpalParser->process();
	
}

void printAst() {
	lexAnalyser* rpalLexer1 = new lexAnalyser(&src);
	parser* rpalParser1 = new parser(rpalLexer1);
	rpalParser1->process();
	rpalParser1->printAST();
}

int main (int argc, char *argv[]){
	if (argc == 2){  // console arguments : rpal20, filename
	    char* fname = argv[argc-1];
	    src.open(fname);
		// check the input file is okay
	    if (!src.good()){			
	        printf ("File \"%s\" is not available in the directory\n", fname);
	        return 0;
	    }	   
		exec();
		src.close();
	}
	
	else if(argc == 3){
		if(strcmp(argv[1], "-ast") == 0){
				printAst();
		}
		else{
			printf ("Prompt: ./rpal20 -ast <filename>");
		}
	}
	else {
		printf ("Prompt: ./rpal20 <filename>\n\n");
	}
	
}

