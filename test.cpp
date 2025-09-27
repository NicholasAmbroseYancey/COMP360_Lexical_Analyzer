// front.cpp - a lexical analyzer system for simple
//             arithmetic expressions (C++ translation)

#include <iostream>
#include <fstream>
#include <cctype>
#include <cstring>

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
/* main driver */
int main() {
    inFile.open("front.in");

    if (!inFile.is_open()) {
        cerr << "ERROR - cannot open front.in" << endl;
        return 1;
    }
    else {
        getChar();
        do {
            lex();
        } while (nextToken != EOF);
    }

    return 0;
}