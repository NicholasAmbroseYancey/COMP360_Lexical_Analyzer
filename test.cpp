// front.cpp - a lexical analyzer system for simple
//             arithmetic expressions (C++ translation)

#include <iostream>
#include <fstream>
#include <cctype>
#include <cstring>
#include <string>

using namespace std;

/* Global declarations */
// Variables
int charClass;
char lexeme[100];
char nextChar;
int lexLen;
int token;
int nextToken;
ifstream inFile;

// Function declarations
void addChar();
void getChar();
void getNonBlank();
int lex();
int lookup(char ch);

// *** NEW: Parser function declarations ***
bool parseProgram();
bool parseDeclares();
bool parseLoop();
bool parseAssignment();
bool parseKeyword();
bool parseConst();
bool parseIdent();
void match(int expectedToken);
void reportError(string message);
bool isKeyword(string lexeme);
bool isConst(string lexeme);
void resetParser();

/* Character classes */
#define LETTER 0
#define DIGIT 1
#define UNKNOWN 99
#define EOF_CHAR 98

/* Token codes */
#define INT_LIT 10
#define IDENT 11
#define ASSIGN_OP 20
#define ADD_OP 21
#define SUB_OP 22
#define MULT_OP 23
#define DIV_OP 24
#define LEFT_PAREN 25
#define RIGHT_PAREN 26
#define COMMA 27
#define LEFT_BRACE 28
#define RIGHT_BRACE 29
// *** NEW: Semicolon token ***
#define SEMICOLON 30

// *** NEW: Parser-specific tokens ***
#define VOID_KEYWORD 31
#define WHILE_KEYWORD 32
#define INT_KEYWORD 33
#define LE_OP 34  // <= operator
#define CONST_LIT 35

/******************************************************/

void addChar() {
    if (lexLen <= 98) {
        lexeme[lexLen++] = nextChar;
        lexeme[lexLen] = 0;
    }
}

void getChar() {
    if (inFile.get(nextChar)) {
        if (isalpha(nextChar))
            charClass = LETTER;
        else if (isdigit(nextChar))
            charClass = DIGIT;
        else
            charClass = UNKNOWN;
    } else {
        charClass = EOF_CHAR;
    }
}

void getNonBlank() {
    while (isspace(nextChar) && charClass != EOF_CHAR) {
        getChar();
    }
}

/*****************************************************/
/* lookup - function to lookup operators, parentheses, comma, braces, and semicolon */
int lookup(char ch) {
    switch (ch) {
        case '(':
            addChar();
            nextToken = LEFT_PAREN;
            break;
        case ')':
            addChar();
            nextToken = RIGHT_PAREN;
            break;
        case '+':
            addChar();
            nextToken = ADD_OP;
            break;
        case '-':
            addChar();
            nextToken = SUB_OP;
            break;
        case '*':
            addChar();
            nextToken = MULT_OP;
            break;
        case '/':
            addChar();
            nextToken = DIV_OP;
            break;
        case '=':
             addChar();
             nextToken = ASSIGN_OP;
             break;
        case '<':
             addChar();
             getChar();
             if (nextChar == '=') {
                 addChar();
                 nextToken = LE_OP;
             } else {
                 nextToken = UNKNOWN;
             }
             break;
        case ',':
             addChar();
             nextToken = COMMA;
             break;
        case '{':
             addChar();
             nextToken = LEFT_BRACE;
             break;
        case '}':
             addChar();
             nextToken = RIGHT_BRACE;
             break;
        // *** NEW: Semicolon case ***
        case ';':
             addChar();
             nextToken = SEMICOLON;
             break;
        default:
            addChar();
            nextToken = UNKNOWN;
            break;
    }
    return nextToken;
}

/*****************************************************/
/* lex - a simple lexical analyzer */
int lex() {
    lexLen = 0;
    getNonBlank();

    switch (charClass) {
        case LETTER:
            addChar();
            getChar();
            while (charClass == LETTER || charClass == DIGIT) {
                addChar();
                getChar();
            }
            nextToken = IDENT;
            break;

        case DIGIT:
            addChar();
            getChar();
            while (charClass == DIGIT) {
                addChar();
                getChar();
            }
            nextToken = INT_LIT;
            break;

        case UNKNOWN:
            lookup(nextChar);
            getChar();
            break;

        case EOF_CHAR:
            nextToken = EOF;
            lexeme[0] = 'E';
            lexeme[1] = 'O';
            lexeme[2] = 'F';
            lexeme[3] = 0;
            break;
    }

    cout << "Next token is: " << nextToken << ", Next lexeme is " << lexeme << endl;
    return nextToken;
}

/******************************************************/
/* Parser Implementation for Project1 EBNF Grammar */

// Global parser variables
bool parseError = false;
string errorMessage = "";

void resetParser() {
    parseError = false;
    errorMessage = "";
}

void reportError(string message) {
    if (!parseError) {
        parseError = true;
        errorMessage = message;
    }
}

void match(int expectedToken) {
    if (nextToken == expectedToken) {
        lex(); // Get next token
    } else {
        reportError("Expected token " + to_string(expectedToken) + " but found " + to_string(nextToken));
    }
}

bool isKeyword(string lexeme) {
    return (lexeme == "int" || lexeme == "while" || lexeme == "void");
}

bool isConst(string lexeme) {
    return (lexeme == "100" || lexeme == "20");
}

bool parseIdent() {
    // Handle the left recursive <ident> -> a <ident>| b<ident> … | z <ident> | e
    // We'll implement this by checking if it's a valid identifier
    if (nextToken == IDENT) {
        // Check if it starts with a letter and is valid
        string currentLexeme = string(lexeme);
        if (currentLexeme.length() > 0 && isalpha(currentLexeme[0])) {
            match(IDENT);
            return true;
        }
    }
    reportError("Expected identifier but found " + string(lexeme));
    return false;
}

bool parseConst() {
    if (nextToken == INT_LIT && isConst(string(lexeme))) {
        match(INT_LIT);
        return true;
    }
    reportError("Expected constant (100 or 20) but found " + string(lexeme));
    return false;
}

bool parseKeyword() {
    if (nextToken == IDENT && isKeyword(string(lexeme))) {
        match(IDENT);
        return true;
    }
    reportError("Expected keyword (int, while, or void) but found " + string(lexeme));
    return false;
}

bool parseAssignment() {
    // <assignment> -> <ident> = <ident> + <ident>;
    if (!parseIdent()) return false;
    match(ASSIGN_OP);
    if (!parseIdent()) return false;
    match(ADD_OP);
    if (!parseIdent()) return false;
    match(SEMICOLON);
    return !parseError;
}

bool parseLoop() {
    // <loop> -> <keyword> ( <ident > <= const )
    if (!parseKeyword()) return false;
    match(LEFT_PAREN);
    if (!parseIdent()) return false;
    match(LE_OP);
    if (!parseConst()) return false;
    match(RIGHT_PAREN);
    return !parseError;
}

bool parseDeclares() {
    // <declares> -> <keyword> <ident> = const;
    if (!parseKeyword()) return false;
    if (!parseIdent()) return false;
    match(ASSIGN_OP);
    if (!parseConst()) return false;
    match(SEMICOLON);
    return !parseError;
}

bool parseProgram() {
    // <program> -> <keyword> <ident> ( ) { <declares> <loop><assignment>}
    resetParser();
    
    if (!parseKeyword()) return false;
    if (!parseIdent()) return false;
    match(LEFT_PAREN);
    match(RIGHT_PAREN);
    match(LEFT_BRACE);
    if (!parseDeclares()) return false;
    if (!parseLoop()) return false;
    if (!parseAssignment()) return false;
    match(RIGHT_BRACE);
    
    return !parseError;
}

/******************************************************/
/* main driver */
int main() {
    cout << "COMP 360-01 Programming Languages - Project 1" << endl;
    cout << "Lexical Analyzer and Recursive-Descent Parser" << endl;
    cout << "=============================================" << endl;
    
    // Test with first file
    cout << "\n=== Testing with front.in (Test Source Code 1) ===" << endl;
    inFile.open("front.in");
    
    if (!inFile.is_open()) {
        cerr << "ERROR - cannot open front.in" << endl;
        return 1;
    }
    
    cout << "Lexical Analysis:" << endl;
    getChar();
    do {
        lex();
    } while (nextToken != EOF);
    
    inFile.close();
    
    // Reset for parsing
    inFile.open("front.in");
    if (inFile.is_open()) {
        getChar();
        lex(); // Get first token
        
        cout << "\nSyntax Analysis:" << endl;
        if (parseProgram() && nextToken == EOF) {
            cout << "The Test Source Code is generated by the grammar" << endl;
        } else {
            cout << "The Test Source Code cannot be generated by the Project1 EBNF Defined Language" << endl;
            if (parseError) {
                cout << "Error: " << errorMessage << endl;
            }
        }
        inFile.close();
    }
    
    // Test with second file
    cout << "\n=== Testing with front2.in (Test Source Code 2) ===" << endl;
    inFile.open("front2.in");
    
    if (!inFile.is_open()) {
        cerr << "ERROR - cannot open front2.in" << endl;
        return 1;
    }
    
    cout << "Lexical Analysis:" << endl;
    getChar();
    do {
        lex();
    } while (nextToken != EOF);
    
    inFile.close();
    
    // Reset for parsing
    inFile.open("front2.in");
    if (inFile.is_open()) {
        getChar();
        lex(); // Get first token
        
        cout << "\nSyntax Analysis:" << endl;
        if (parseProgram() && nextToken == EOF) {
            cout << "The Test Source Code is generated by the grammar" << endl;
        } else {
            cout << "The Test Source Code cannot be generated by the Project1 EBNF Defined Language" << endl;
            if (parseError) {
                cout << "Error: " << errorMessage << endl;
            }
        }
        inFile.close();
    }
    
    cout << "\n=== Analysis Complete ===" << endl;
    return 0;
}