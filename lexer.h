/*
 * Copyright (C) Rida Bazzi, 2016
 *
 * Do not share this file with anyone
 */
#ifndef __LEXER__H__
#define __LEXER__H__

#include <string>
#include <vector>

#include "inputbuf.h"

using std::string;

// ------- token types -------------------
typedef enum { END_OF_FILE = 0,
               INT,
               REAL,
               BOOL,
               TRUE,
               FALSE,
               IF,
               WHILE,
               SWITCH,
               CASE,
               NOT,
               PLUS,
               MINUS,
               MULT,
               DIV,
               GREATER,
               LESS,
               GTEQ,
               LTEQ,
               NOTEQUAL,
               LPAREN,
               RPAREN,
               NUM,
               REALNUM,
               PUBLIC,
               PRIVATE,
               EQUAL,
               COLON,
               COMMA,
               SEMICOLON,
               LBRACE,
               RBRACE,
               ID,
               ERROR  // TODO: Add labels for new token types here
} TokenType;

class Token {
   public:
    void Print();

    std::string lexeme;
    TokenType token_type;
    int line_no;
};

class LexicalAnalyzer {
   public:
    Token GetToken();
    TokenType UngetToken(Token);
    LexicalAnalyzer();

   private:
    std::vector<Token> tokens;
    int line_no;
    Token token;
    InputBuffer input;

    bool SkipSpace();
    bool SkipComments();
    Token ScanNumber();
    bool IsKeyword(string);
    TokenType FindKeywordIndex(string);
    Token ScanIdOrKeyword();
};

#endif  //__LEXER__H__
