#include "parser.h"

#include <algorithm>
#include <iostream>  // brackets are for standard library stuff

using std::cout;

void SymbolTable::addSymbol(Symbol* symbol) {
    this->symbols.push_back(symbol);
}

void SymbolTable::printSymbolTable() {
    it = symbols.begin();
    while (it != symbols.end()) {
        std::cout << (*it)->name << ", " << (*it)->type << " #\n";
        it++;
    }
}

// This function consumes the next token in the list
Token Parser::consume(TokenType expected_type) {
    Token t = lexer.GetToken();
    // t.Print();
    if (t.token_type != expected_type) {
        syntax_error();
    }
    return t;
}

void Parser::syntax_error() {
    std::cout << "Syntax Error\n";
    exit(1);
}

void Parser::parse_program() {
    parse_global_vars();
    parse_body();
    // symbolTable.printSymbolTable();
}

void Parser::parse_global_vars() {
    // Check if it contains epsilon
    Token t = lexer.GetToken();
    lexer.UngetToken(t);
    if (t.token_type == ID) {
        parse_var_decl_list();
    }
}

void Parser::parse_var_decl_list() {
    parse_var_decl();

    Token t = lexer.GetToken();
    lexer.UngetToken(t);
    if (t.token_type == ID) {
        parse_var_decl_list();
    }
}

void Parser::parse_var_decl() {
    vector<Token> var_decl = parse_var_list();
    consume(COLON);
    string type = parse_type_name();

    for (vector<Token>::iterator it = var_decl.begin(); it != var_decl.end(); it++) {
        Symbol* symbol = new Symbol();
        symbol->name = (*it).lexeme;
        symbol->type = type;
        symbolTable.addSymbol(symbol);
    }

    consume(SEMICOLON);
}

// TODO: Explicit variables are declared in an var_list of a var_decl
vector<Token> Parser::parse_var_list() {
    vector<Token> var_list;
    var_list.push_back(consume(ID));

    Token t = lexer.GetToken();
    if (t.token_type == COMMA) {
        vector<Token> var_list2 = parse_var_list();
        for (vector<Token>::iterator it = var_list2.begin(); it != var_list2.end(); it++) {
            var_list.push_back(*it);
        }
    } else {
        lexer.UngetToken(t);
    }

    return var_list;
}

// TODO: check for INT, REAL, BOOL
string Parser::parse_type_name() {
    Token t = lexer.GetToken();

    if (t.token_type == INT) {
        return "int";
    } else if (t.token_type == REAL) {
        return "real";
    } else if (t.token_type == BOOL) {
        return "bool";
    } else {
        syntax_error();
    }

    return "";
}

void Parser::parse_body() {
    consume(LBRACE);
    parse_stmt_list();
    consume(RBRACE);
}

void Parser::parse_stmt_list() {
    parse_stmt();

    Token t = lexer.GetToken();
    lexer.UngetToken(t);
    if (t.token_type == IF || t.token_type == WHILE || t.token_type == SWITCH || t.token_type == WHILE || t.token_type == ID) {
        parse_stmt_list();
    }
}

void Parser::parse_stmt() {
    Token u = lexer.GetToken();
    lexer.UngetToken(u);

    if (u.token_type == ID) {
        parse_assignment_stmt();
    } else if (u.token_type == IF) {
        parse_if_stmt();
    } else if (u.token_type == WHILE) {
        parse_while_stmt();
    } else if (u.token_type == SWITCH) {
        parse_switch_stmt();
    } else {
        syntax_error();
    }
}

void Parser::parse_assignment_stmt() {
    Token t = consume(ID);
    consume(EQUAL);
    vector<Token> expression = parse_expression();
    consume(SEMICOLON);
}

// TODO: Fix
vector<Token> Parser::parse_expression() {
    Token t = lexer.GetToken();
    lexer.UngetToken(t);

    vector<Token> expression;

    if (t.token_type == ID || t.token_type == NUM || t.token_type == REALNUM || t.token_type == TRUE || t.token_type == FALSE) {
        expression.push_back(parse_primary());
    } else if (t.token_type == PLUS || t.token_type == MINUS || t.token_type == MULT || t.token_type == DIV || t.token_type == GREATER || t.token_type == LESS || t.token_type == GTEQ || t.token_type == LTEQ || t.token_type == EQUAL || t.token_type == NOTEQUAL) {
        expression.push_back(parse_binary_operator());

        vector<Token> temp = parse_expression();
        for (int i = 0; i < temp.size(); i++) {
            expression.push_back(temp.at(i));
        }

        temp = parse_expression();
        for (int i = 0; i < temp.size(); i++) {
            expression.push_back(temp.at(i));
        }
    } else if (t.token_type == NOT) {
        expression.push_back(parse_unary_operator());

        vector<Token> temp = parse_expression();
        for (int i = 0; i < temp.size(); i++) {
            expression.push_back(temp.at(i));
        }
    } else {
        syntax_error();
    }

    return expression;
}

// The type of unary_operator is bool
Token Parser::parse_unary_operator() {
    Token t = consume(NOT);
    return t;
}

Token Parser::parse_binary_operator() {
    Token t = lexer.GetToken();
    lexer.UngetToken(t);

    if (t.token_type == PLUS || t.token_type == MINUS || t.token_type == MULT || t.token_type == DIV || t.token_type == GREATER || t.token_type == LESS || t.token_type == GTEQ || t.token_type == LTEQ || t.token_type == EQUAL || t.token_type == NOTEQUAL) {
        consume(t.token_type);
    } else {
        syntax_error();
    }
    return t;
}

Token Parser::parse_primary() {
    Token t = lexer.GetToken();
    lexer.UngetToken(t);

    if (t.token_type == ID) {
        consume(ID);
    } else if (t.token_type == NUM) {
        consume(NUM);
    } else if (t.token_type == REALNUM) {
        consume(REALNUM);
    } else if (t.token_type == TRUE) {
        consume(TRUE);
    } else if (t.token_type == FALSE) {
        consume(FALSE);
    } else {
        syntax_error();
    }

    return t;
}

// TODO
void Parser::parse_if_stmt() {
    consume(IF);
    consume(LPAREN);
    vector<Token> tokens = parse_expression();

    TokenType type = tokens.at(0).token_type;
    if (type != GREATER && type != LESS && type != GTEQ && type != LTEQ && type != EQUAL && type != NOTEQUAL) {
        cout << "TYPE MISMATCH " << tokens.at(0).line_no << " C4\n";
    } else {
        for (int i = 1; i < tokens.size(); i++) {
            string tokenType = "";
            type = tokens.at(i).token_type;
            if (type == NUM) {
                tokenType = "int";
            } else if (type == REALNUM) {
                tokenType = "real";
            } else if (type == TRUE) {
                tokenType = "true";
            } else if (type == FALSE) {
                tokenType = "false";
            } else {
                cout << type << " NOT PRINTING\n";
            }
            cout << tokens.at(i).lexeme << ": " << tokenType << "\n";
        }
    }

    consume(RPAREN);
    parse_body();
}

void Parser::parse_while_stmt() {
    consume(WHILE);
    consume(LPAREN);
    vector<Token> tokens = parse_expression();

    TokenType t = tokens.at(0).token_type;
    if (t != GREATER && t != LESS && t != GTEQ && t != LTEQ && t != EQUAL && t != NOTEQUAL) {
        cout << "TYPE MISMATCH " << tokens.at(0).line_no << " C4\n";
    }

    consume(RPAREN);
    parse_body();
}

void Parser::parse_switch_stmt() {
    consume(SWITCH);
    consume(LPAREN);
    parse_expression();
    consume(RPAREN);
    consume(LBRACE);
    parse_case_list();
    consume(RBRACE);
}

void Parser::parse_case_list() {
    parse_case();

    Token t = lexer.GetToken();
    lexer.UngetToken(t);
    if (t.token_type == CASE) {
        parse_case_list();
    }
}

void Parser::parse_case() {
    consume(CASE);
    consume(NUM);
    consume(COLON);
    parse_body();
}

int main() {
    Parser parser;
    parser.parse_program();
}