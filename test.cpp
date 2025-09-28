// COMP 360 Programming Languages - Project 1
// Lexical Analyzer and Parser
// Created by: Ambrose Yancey, Ryan Bolt, Jason Boyce
// Date: 9/28/2025


#include <iostream>
#include <fstream>
#include <cctype>
#include <cstring>
#include <string>

using namespace std;

// Variables
int charClass;
char lexeme[100];
char nextChar;
int lexLen;
int token;
int nextToken;
ifstream inFile;

// Functions
void addChar();
void getChar();
void getNonBlank();
int lex();
int lookup(char ch);
bool parseProgram();
bool parseDeclares();
bool parseLoop();
bool parseAssignment();
bool parseKeyword();
bool parseConst();
bool parseIdent();
bool match(int expectedToken);
void reportError(string message);
bool isKeyword(string lexeme);
bool isConst(string lexeme);
void resetParser();

// Character classes and token codes
#define LETTER 0
#define DIGIT 1
#define UNKNOWN 99
#define EOF_CHAR 98
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
#define SEMICOLON 30
#define VOID_KEYWORD 31
#define WHILE_KEYWORD 32
#define INT_KEYWORD 33
#define LE_OP 34 
#define CONST_LIT 35

// Appends the current character
void addChar() {
    if (lexLen <= 98) {
        lexeme[lexLen++] = nextChar;
        lexeme[lexLen] = 0;
    }
}

// Reads the next character
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

// Skips whitespace
void getNonBlank() {
    while (isspace(nextChar) && charClass != EOF_CHAR) {
        getChar();
    }
}

// Cases for each token
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

// Lexical analyzer
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

// Parser functions
bool parseError = false;
string errorMessage = "";

// Resets the parser
void resetParser() {
    parseError = false;
    errorMessage = "";
}

// Reports if an error has occurred
void reportError(string message) {
    if (!parseError) {
        parseError = true;
        errorMessage = message;
    }
}

// Matches the expected token
bool match(int expectedToken) {
    if (nextToken == expectedToken) {
        lex();
        return true;
    } else {
        reportError("Expected token " + to_string(expectedToken) + " but found " + to_string(nextToken));
        return false;
    }
}

// Checks if the lexeme is a keyword
bool isKeyword(string lexeme) {
    return (lexeme == "int" || lexeme == "while" || lexeme == "void");
}

// Checks if the lexeme is a constant
bool isConst(string lexeme) {
    return (lexeme == "100" || lexeme == "20");
}

// Project 1 EBNF Grammar functions starting with <ident>
bool parseIdent() {
    cout << "Enter <ident>" << endl;
    if (nextToken == IDENT) {
        string currentLexeme = string(lexeme);
        if (currentLexeme.length() > 0 && isalpha(currentLexeme[0])) {
            match(IDENT);
            cout << "Exit <ident>" << endl;
            return true;
        }
    }
    reportError("Expected identifier but found " + string(lexeme));
    return false;
}

bool parseConst() {
    cout << "Enter <const>" << endl;
    if (nextToken == INT_LIT && isConst(string(lexeme))) {
        match(INT_LIT);
        cout << "Exit <const>" << endl;
        return true;
    }
    reportError("Expected constant (100 or 20) but found " + string(lexeme));
    return false;
}

bool parseKeyword() {
    cout << "Enter <keyword>" << endl;
    if (nextToken == IDENT && isKeyword(string(lexeme))) {
        match(IDENT);
        cout << "Exit <keyword>" << endl;
        return true;
    }
    reportError("Expected keyword (int, while, or void) but found " + string(lexeme));
    return false;
}

bool parseAssignment() {
    cout << "Enter <assignment>" << endl;
    if (!parseIdent()) return false;
    if (!match(ASSIGN_OP)) return false;
    if (!parseIdent()) return false;
    if (!match(ADD_OP)) return false;
    if (!parseIdent()) return false;
    if (!match(SEMICOLON)) return false;
    cout << "Exit <assignment>" << endl;
    return !parseError;
}

bool parseLoop() {
    cout << "Enter <loop>" << endl;
    if (!parseKeyword()) return false;
    if (!match(LEFT_PAREN)) return false;
    if (!parseIdent()) return false;
    if (!match(LE_OP)) return false;
    if (!parseConst()) return false;
    if (!match(RIGHT_PAREN)) return false;
    cout << "Exit <loop>" << endl;
    return !parseError;
}

bool parseDeclares() {
    cout << "Enter <declares>" << endl;
    if (!parseKeyword()) return false;
    if (!parseIdent()) return false;
    if (!match(ASSIGN_OP)) return false;
    if (!parseConst()) return false;
    if (!match(SEMICOLON)) return false;
    cout << "Exit <declares>" << endl;
    return !parseError;
}

bool parseProgram() {
    resetParser();
    cout << "Enter <program>" << endl;
    
    if (!parseKeyword()) return false;
    if (!parseIdent()) return false;
    if (!match(LEFT_PAREN)) return false;
    if (!match(RIGHT_PAREN)) return false;
    if (!match(LEFT_BRACE)) return false;
    if (!parseDeclares()) return false;
    if (!parseLoop()) return false;
    if (!parseAssignment()) return false;
    if (!match(RIGHT_BRACE)) return false;
    
    cout << "Exit <program>" << endl;
    return !parseError;
}

// Main function that runs the program
int main() {
    cout << "COMP 360 Programming Languages - Project 1" << endl;
    
    string filename;
    cout << "\nEnter the name of the file to parse: ";
    getline(cin, filename);
    
    cout << "\nTesting with " << filename << "...\n" << endl;
    inFile.open(filename);
    
    if (!inFile.is_open()) {
        cerr << "ERROR - cannot open " << filename << endl;
        return 1;
    }
    
    getChar();
    lex();

    // Print statements if the program is generated by the grammar or if there is an error
    if (parseProgram() && nextToken == EOF) {
        cout << "The Test Source Code is generated by the grammar" << endl;
    } 
    else {
        cout << "The Test Source Code cannot be generated by the Project1 EBNF Defined Language" << endl;
        if (parseError) {
            cout << "Error: " << errorMessage << endl;
        }
    }
    inFile.close();
    
    cout << "\nAnalysis Complete!" << endl;
    return 0;
}