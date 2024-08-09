#include "lexAnalyser.h"

using namespace std;

// Constructor for lexAnalyser class, initializes file pointer and counts
lexAnalyser::lexAnalyser(std::ifstream* source) {
    Rpalfile = source; //change to buffer
    lineCount = 1;
    charCount = 1;
}

// This method fetches the next token from the input stream
tokenData* lexAnalyser::getNextToken(){
    // Create a new tokenData object
    tokenData* temp_token = new tokenData();
    int nextChar=Rpalfile->peek();

    // Identifier token
    if (isalpha(nextChar)){
        temp_token->value_token = tokenIdentifier();
        temp_token->tokType = TOK_IDENTIFIER;
    } 
    // Integer token
    else if (isdigit(nextChar)) {
        temp_token->value_token = tokenInteger();
        temp_token->tokType = TOK_INTEGER;
    } 
    // String token
    else if ('\'' == nextChar){
        temp_token->value_token = tokenStrings();
        temp_token->tokType = TOK_STRING;
    } 
    // Space (delete token) -- will be deleted 
    else if (isspace(nextChar)){
        temp_token->value_token = tokenSpaces();
        temp_token->tokType = TOK_DELETE;
    } 
    // Punctuation token
    else if (isPunction(nextChar)){
        char readNext;
        Rpalfile->get(readNext);
        temp_token->value_token = readNext;
        temp_token->tokType = TOK_PUNCTION;
        charCount++;
    } 
    // Check for '/'
    else if ('/' == nextChar) {
        temp_token->value_token = tokenComment();
        // Comment type token -- hence DELETE
        if (temp_token->value_token.compare("/")){
            temp_token->tokType = TOK_DELETE;
        } 
        // Slash(/) Operator
        else{
            temp_token->tokType = TOK_OPERATOR;
        }
    } 
    // Other operator without slash(/) operator
    else if (isOperatorSymbol(nextChar)){
        temp_token->value_token = tokenOperator();
        temp_token->tokType = TOK_OPERATOR;
    } 
    // End of File
    else if (EOF == nextChar) {
        temp_token->tokType = TOK_EOF;
    }
    temp_token->charCount = charCount;
    temp_token->lineCount = lineCount;
    return temp_token;
}

// This method creates a tokenData of type IDENTIFIER
string lexAnalyser::tokenIdentifier() {
    string tokStr = "";
    char nextChar;

    while (Rpalfile->get(nextChar)) {
        charCount++;
        if (isalpha(nextChar) || isdigit(nextChar) || nextChar == '_') {
            tokStr += nextChar;
        } else {
            Rpalfile->unget(); // Put the character back into the stream for future parsing
            break;
        }
    }
    return tokStr;
}

// This method creates a tokenData of type INTEGER
string lexAnalyser::tokenInteger() {
    string tokStr = "";
    char nextChar;

    while (Rpalfile->get(nextChar)) {
        charCount++;
        if (isdigit(nextChar)) {
            tokStr += nextChar;
        } else {
            Rpalfile->unget(); // Put the character back into the stream for future parsing
            break;
        }
    }
    return tokStr;
}

// Check whether a character is an operator symbol or not
bool lexAnalyser::isOperatorSymbol(char c) {
    const char operators[] = {'+', '-', '*', '<', '>', '&', '.', '@', '/', ':', '=', '~', '|','$', '!', '#', '%', '^', '_', '[', ']', '{', '}', '"', '`', '?'};
    for (char op : operators) {
        if (c == op) {
            return true;
        }
    }
    return false;
}

// This method creates a tokenData of type OPERATOR
string lexAnalyser::tokenOperator() {
    string tokStr = "";
    char nextPeek;
    char nextChar;
    Rpalfile->get(nextChar);
    charCount++;
    tokStr += nextChar;

    // Continue reading characters until a non-operator symbol or EOF is encountered
    while (!(Rpalfile->eof()) && isOperatorSymbol(nextPeek = Rpalfile->peek())) {
        Rpalfile->get(nextChar);
        charCount++;
        tokStr += nextChar;
    }
    return tokStr;
}

// This method creates a tokenData of type STRING
string lexAnalyser::tokenStrings(){
    string tokStr= "";
    char nextPeek;
    char nextChar;
    while (!(EOF == nextPeek) && !(nextPeek == '\'')){
        Rpalfile->get(nextChar);
        tokStr += nextChar;
        charCount++;
        nextPeek = Rpalfile->peek();
        if (nextChar == '\\'){
            if ((nextPeek == 't' || nextPeek == 'n' || nextPeek == '\\' || nextPeek == '\''))
            {
                continue; 
            }
            else {
                printf ("Error\n");
                exit(0);
            }
        }
    } 
    Rpalfile->get(nextChar);
    tokStr += nextChar;
    return tokStr;
}

// Check whether a character is a punctuation
bool lexAnalyser::isPunction(char c){
 const char punctuations[] = {'(', ')', ';', ','};
    for (char punc : punctuations) {
        if (c == punc) {
            return true;
        }
    }
    return false;
}

lexAnalyser::~lexAnalyser() {
}

// Reset lexer to the beginning of the file
void lexAnalyser::lexerReset(){
    Rpalfile->clear();
    Rpalfile->seekg(0, ios::beg);
}

// This method creates a tokenData of type SPACES
string lexAnalyser::tokenSpaces() {
    string tokStr = "";
    char nextPeek;
    char nextChar;
    Rpalfile->get(nextChar);
    charCount++;
    if (nextChar == '\n' || nextChar == '\r') {
        charCount = 1;
        lineCount++;
    }
    tokStr += nextChar;
    while (!(Rpalfile->eof()) && isspace(nextPeek = Rpalfile->peek())) {
        Rpalfile->get(nextChar);
        charCount++;
        if (nextChar == '\n' || nextChar == '\r') {
            charCount = 1;
            lineCount++;
        }
        tokStr += nextChar;
    }

    return tokStr;
}

// This method creates a tokenData of type COMMENT
string lexAnalyser::tokenComment(){
    // Initialize an empty string to store the comment token
    string tokStr = "";
    char nextPeek;
    char nextChar;
    
    Rpalfile->get(nextChar);
    charCount++;
    tokStr += nextChar;
    nextPeek = Rpalfile->peek();
    
    if ('/' == nextPeek){
        Rpalfile->get(nextChar);
        tokStr += nextChar;
        
        while (!(EOF == nextPeek) && !('\n' == nextPeek || '\r' == nextPeek)){
            Rpalfile->get(nextChar);
            tokStr += nextChar;
            nextPeek = Rpalfile->peek();
        } 
    } 

    return tokStr;
}
