#include "lexer.h"

// #include <cctype>
#include <iostream>
// #include <istream>

using std::cout;
using std::string;

string reserved[] = {
    "END_OF_FILE", "INT", "REAL", "BOOL", "TRUE", "FALSE", "IF", "WHILE", "SWITCH", "CASE", "NOT", "PLUS", "MINUS", "MULT", "DIV", "GREATER", "LESS", "GTEQ", "LTEQ", "NOTEQUAL", "LPAREN", "RPAREN", "NUM", "REALNUM", "PUBLIC", "PRIVATE", "EQUAL", "COLON", "COMMA", "SEMICOLON", "LBRACE", "RBRACE", "ID", "ERROR"  // TODO: Add labels for new token types here (as string)
};

#define KEYWORDS_COUNT 11
string keyword[] = {"int", "real", "bool", "true", "false", "if", "while", "switch", "case", "public", "private"};

void Token::Print() {
    // cout << "token.Print() is called\n";
    cout << "{" << this->lexeme << " , "
         << reserved[(int)this->token_type] << " , "
         << this->line_no << "}\n";
}

LexicalAnalyzer::LexicalAnalyzer() {
    this->line_no = 1;
    token.lexeme = "";
    token.line_no = 1;
    token.token_type = ERROR;  // if Token is not set, set it to ERROR
}

bool LexicalAnalyzer::SkipComments() {
    char c;
    bool commentEncountered = false;
    SkipSpace();

    input.GetChar(c);
    // line_no += (c == '\n');

    if (c == '/') {
        input.GetChar(c);
        if (c == '/') {
            // cout << "comment detected\n";
            while (!input.EndOfInput()) {
                commentEncountered = true;
                input.GetChar(c);
                if (c == '\n') {
                    line_no++;
                    break;
                }
            }
        } else {
            input.UngetChar(c);
            input.UngetChar('/');
            return false;
        }
    } else {
        input.UngetChar(c);
    }
    SkipSpace();
    return commentEncountered;
}

bool LexicalAnalyzer::SkipSpace() {
    char c;
    bool space_encountered = false;

    input.GetChar(c);
    line_no += (c == '\n');

    while (!input.EndOfInput() && isspace(c)) {
        space_encountered = true;
        input.GetChar(c);
        line_no += (c == '\n');
    }

    if (!input.EndOfInput()) {
        input.UngetChar(c);
    }
    return space_encountered;
}

bool LexicalAnalyzer::IsKeyword(string s) {
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return true;
        }
    }
    return false;
}

TokenType LexicalAnalyzer::FindKeywordIndex(string s) {
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return (TokenType)(i + 1);
        }
    }
    return ERROR;
}

Token LexicalAnalyzer::ScanNumber() {
    char c;
    bool realNUM = false;
    input.GetChar(c);
    if (isdigit(c)) {
        if (c == '0') {
            token.lexeme = "0";
            input.GetChar(c);
            if (c == '.') {
                // cout << "\n I am here too " << c << " \n";
                input.GetChar(c);

                if (!isdigit(c)) {
                    input.UngetChar(c);
                } else {
                    // TODO: Fix REALNUM
                    if (!input.EndOfInput() && isdigit(c)) {
                        token.lexeme += '.';
                    }
                    while (!input.EndOfInput() && isdigit(c)) {
                        token.lexeme += c;
                        input.GetChar(c);
                        realNUM = true;
                    }
                    input.UngetChar(c);
                }
            } else {
                input.UngetChar(c);
            }
        } else {
            token.lexeme = "";
            while (!input.EndOfInput() && isdigit(c)) {
                token.lexeme += c;
                input.GetChar(c);
            }
            if (c == '.') {
                // cout << "\n I am here too " << c << " \n";
                input.GetChar(c);

                if (!isdigit(c)) {
                    input.UngetChar(c);
                } else {
                    // TODO: Fix REALNUM
                    if (!input.EndOfInput() && isdigit(c)) {
                        token.lexeme += '.';
                    }
                    while (!input.EndOfInput() && isdigit(c)) {
                        token.lexeme += c;
                        input.GetChar(c);
                        realNUM = true;
                    }
                }
            }

            if (!input.EndOfInput()) {
                input.UngetChar(c);
            }
        }
        // TODO: You can check for REALNUM, BASE08NUM and BASE16NUM here!
        if (realNUM) {
            // token.lexeme += '.';
            token.token_type = REALNUM;
        } else {
            token.token_type = NUM;
        }
        token.line_no = line_no;
        return token;
    } else {
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        token.lexeme = "";
        token.token_type = ERROR;
        token.line_no = line_no;
        return token;
    }
}

Token LexicalAnalyzer::ScanIdOrKeyword() {
    char c;
    input.GetChar(c);

    //if (c == 't') {
    //    input.GetChar(c);
    //    if (c == 'r') {
    //        input.GetChar(c);
    //        if (c == 'u') {
    //            input.GetChar(c);
    //            if (c == 'e') {
    //                token.token_type = TRUE;
    //                return token;
    //            } else {
    //                input.UngetChar(c);
    //                input.UngetChar('u');
    //                input.UngetChar('r');
    //            }
    //        } else {
    //            input.UngetChar(c);
    //            input.UngetChar('r');
    //        }
    //    } else {
    //        input.UngetChar(c);
    //    }
    //}
    //else if (c == 'f') {
    //    input.GetChar(c);
    //    if (c == 'a') {
    //        input.GetChar(c);
    //        if (c == 'l') {
    //            input.GetChar(c);
    //            if (c == 's') {
    //                input.GetChar(c);
    //                if (c == 'e') {
    //                    token.token_type = FALSE;
    //                    return token;
    //                } else {
    //                    input.UngetChar(c);
    //                    input.UngetChar('s');
    //                    input.UngetChar('l');
    //                    input.UngetChar('a');
    //                }
    //            } else {
    //                input.UngetChar(c);
    //                input.UngetChar('l');
    //                input.UngetChar('a');
    //            }
    //        } else {
    //            input.UngetChar(c);
    //            input.UngetChar('a');
    //        }
    //    } else {
    //        input.UngetChar(c);
    //    }
    //}

    if (isalpha(c)) {
        token.lexeme = "";
        while (!input.EndOfInput() && isalnum(c)) {
            token.lexeme += c;
            input.GetChar(c);
        }
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        token.line_no = line_no;
        if (IsKeyword(token.lexeme))
            token.token_type = FindKeywordIndex(token.lexeme);
        else
            token.token_type = ID;
    } else {
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        token.lexeme = "";
        token.token_type = ERROR;
    }
    return token;
}

TokenType LexicalAnalyzer::UngetToken(Token token) {
    tokens.push_back(token);
    return token.token_type;
}

Token LexicalAnalyzer::GetToken() {
    char c;

    // if there are tokens that were previously
    // stored due to UngetToken(), pop a token and
    // return it without reading from input
    if (!tokens.empty()) {
        token = tokens.back();
        tokens.pop_back();
        return token;
    }

    // SkipSpace();
    while (SkipComments()) {
        SkipComments();
    }
    token.lexeme = "";
    token.line_no = line_no;
    input.GetChar(c);
    // switch (c) {
    //     case '=':
    //         token.token_type = EQUAL;
    //         return token;
    //     case ':':
    //         token.token_type = COLON;
    //         return token;
    //     case ',':
    //         token.token_type = COMMA;
    //         return token;
    //     case ';':
    //         token.token_type = SEMICOLON;
    //         return token;
    //     case '{':
    //         token.token_type = LBRACE;
    //         return token;
    //     case '}':
    //         token.token_type = RBRACE;
    //         return token;
    //     default:
    //         if (isdigit(c)) {
    //             token.token_type = ERROR;
    //         } else if (isalpha(c)) {
    //             input.UngetChar(c);
    //             return ScanIdOrKeyword();
    //         } else if (input.EndOfInput()) {
    //             token.token_type = END_OF_FILE;
    //         } else {
    //             token.token_type = ERROR;
    //         }

    //        return token;
    //}

    // cout << "\n\n"
    //      << c << "\n\n";
    switch (c) {
        case '!':
            token.token_type = NOT;
            return token;
        case '+':
            token.token_type = PLUS;
            return token;
        case '-':
            token.token_type = MINUS;
            return token;
        case '*':
            token.token_type = MULT;
            return token;
        case '/':
            token.token_type = DIV;
            return token;
        case '>':
            input.GetChar(c);
            if (c == '=') {
                token.token_type = GTEQ;
            } else {
                input.UngetChar(c);
                token.token_type = GREATER;
            }
            return token;
        case '<':
            input.GetChar(c);
            if (c == '=') {
                token.token_type = LTEQ;
            } else if (c == '>') {
                token.token_type = NOTEQUAL;
            } else {
                input.UngetChar(c);
                token.token_type = LESS;
            }
            return token;
        case '(':
            token.token_type = LPAREN;
            return token;
        case ')':
            token.token_type = RPAREN;
            return token;
        case '=':
            token.token_type = EQUAL;
            return token;
        case ':':
            token.token_type = COLON;
            return token;
        case ',':
            token.token_type = COMMA;
            return token;
        case ';':
            token.token_type = SEMICOLON;
            return token;
        case '{':
            token.token_type = LBRACE;
            return token;
        case '}':
            token.token_type = RBRACE;
            return token;
        default:
            if (isdigit(c)) {
                input.UngetChar(c);
                return ScanNumber();
            } else if (isalpha(c)) {
                input.UngetChar(c);
                return ScanIdOrKeyword();
            } else if (input.EndOfInput()) {
                token.token_type = END_OF_FILE;
            } else {
                token.token_type = ERROR;
            }

            return token;
    }
}