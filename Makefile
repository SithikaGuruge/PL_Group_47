all:	
	g++ -std=c++11 tokenData.cpp TreeStandardizer.cpp lexAnalyser.cpp Control.cpp parser.cpp Environment.cpp CSEMachine.cpp drive.cpp -o rpal
cl: 
	rm -f p1;
