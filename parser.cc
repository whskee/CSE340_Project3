#include "parser.h"

#include <algorithm>
#include <iostream>  // brackets are for standard library stuff

using std::cout;

void SymbolTable::addSymbol(Symbol* symbol) {
    this->symbols.push_back(symbol);
}

void Parser::addSymbols(set<string> symbols, string scope, string visibility) {
    for (set<string>::iterator it = symbols.begin(); it != symbols.end(); it++) {
        Symbol* symbol = new Symbol();
        symbol->name = *it;
        symbol->scope = scope;
        symbol->visibility = visibility;
        symbolTable.addSymbol(symbol);
    }
}

void SymbolTable::deleteSymbol(Symbol* symbol) {
    vector<Symbol*>::iterator it = std::find(symbols.begin(), symbols.end(), symbol);
    symbols.erase(it);
}

void SymbolTable::deleteSymbolsFromScope(string scope) {
    symbols.rbegin();
    for (auto it = symbols.rbegin(); it != symbols.rend(); it++) {
        if ((*it)->scope == scope) {
            deleteSymbol(*it);
        } else {
            return;
        }
    }
}

int SymbolTable::indexOf(string symbolName) {
    // auto it = std::find(v.begin(), v.end(), K);
    // // If element was found
    // if (it != v.end()) {
    //     int index = it - v.begin();
    //     return index;
    // } else {
    //     // If element was not found
    //     return -1;
    // }
    int index = 0;
    for (vector<Symbol*>::iterator it = symbols.begin(); it != symbols.end(); it++) {
        if ((*it)->name == symbolName) {
            return index;
        }
        index++;
    }
    return -1;
}

void SymbolTable::printSymbolTable() {
    vector<Symbol*>::iterator it = symbols.begin();
    while (it != symbols.end()) {
        string name = (*it)->name;
        string scope = (*it)->scope;
        string visibility = (*it)->visibility;
        std::cout << name << ", " << scope << ", " << visibility << "\n";
        it++;
    }
}

// This function consumes the next token in the list
Token Parser::consume(TokenType expected_type) {
    Token t = lexer.GetToken();
    // cout << "Expected Type: " << expected_type << "                   " << t.lexeme << "       " << t.token_type << "\n";
    t.Print();
    // cout << "printed the statement\n";
    if (t.token_type != expected_type) {
        syntax_error();
    }
    // cout << "\n\n";
    return t;
}

void Parser::syntax_error() {
    std::cout << "Syntax Error\n";
    exit(1);
}

void Parser::printStatements(vector<Statement*> stmt_list) {
    for (vector<Statement*>::iterator it = stmt_list.begin(); it != stmt_list.end(); it++) {
        std::cout << (*it)->lhs << " = " << (*it)->rhs << "\n";
    }
}

void SymbolTable::applyScopingRule(Statement* stmt, string scope) {
    string lhs_name = stmt->lhs;
    string rhs_name = stmt->rhs;
    bool lhs_undefined = true;
    bool rhs_undefined = true;

    // std::cout << lhs_name << " = " << rhs_name << "\n";
    // printSymbolTable();

    for (auto it = symbols.rbegin(); it != symbols.rend(); it++) {
        string name = (*it)->name;
        string visibility = (*it)->visibility;
        if (visibility == "::" || visibility == "public") {
            if (stmt->lhs == name || stmt->rhs == name) {
                if (stmt->lhs == name) {
                    lhs_undefined = false;
                    // std::cout << "equals " << name << "\n";
                    if (visibility == "::") {
                        stmt->lhs = (*it)->scope + name;

                    } else {
                        stmt->lhs = (*it)->scope + "." + name;
                    }
                }
                if (stmt->rhs == name) {
                    // std::cout << "equals " << name << "\n";
                    rhs_undefined = false;
                    if (visibility == "::") {
                        stmt->rhs = (*it)->scope + name;

                    } else {
                        stmt->rhs = (*it)->scope + "." + name;
                    }
                }
            }
        } else if (visibility == "?") {
            if (stmt->lhs == name && scope == (*it)->scope) {
                lhs_undefined = false;
                // std::cout << "equals " << name << "\n";
                stmt->lhs = (*it)->scope + "." + name;
            }
            if (stmt->rhs == name && scope == (*it)->scope) {
                // std::cout << "equals " << name << "\n";
                rhs_undefined = false;
                stmt->rhs = (*it)->scope + "." + name;
            }
        }
    }

    if (lhs_undefined) {
        stmt->lhs = "?." + lhs_name;
    }
    if (rhs_undefined) {
        stmt->rhs = "?." + rhs_name;
    }
}

void SymbolTable::applyScopingRules(vector<Statement*> stmt_list, string scope) {
    vector<Statement*>::iterator it = stmt_list.begin();
    while (it != stmt_list.end()) {
        applyScopingRule(*it, scope);
        it++;
    }
}

void Parser::parse_program() {
    addSymbols(parse_global_vars(), "::", "::");
    vector<Statement*> stmt_list = parse_body();
    // printStatements(this->stmt_list);
}

set<string> Parser::parse_global_vars() {
    set<string> global_vars;

    // Check if it contains epsilon
    Token t = lexer.GetToken();
    lexer.UngetToken(t);
    if (t.token_type == ID) {
        parse_var_decl_list();
    }
    return global_vars;
}

set<string> Parser::parse_var_decl_list() {
    // cout << " Inside var_decl_list\n";
    set<string> var_decl_list;
    set<string> var_decl = parse_var_decl();
    for (set<string>::iterator it = var_decl.begin(); it != var_decl.end(); it++) {
        var_decl_list.insert(*it);
    }

    Token t = lexer.GetToken();
    lexer.UngetToken(t);
    if (t.token_type == ID) {
        var_decl = parse_var_decl_list();
        for (set<string>::iterator it = var_decl.begin(); it != var_decl.end(); it++) {
            var_decl_list.insert(*it);
        }
    }
    // cout << " Exiting var_decl_list\n";
    return var_decl_list;
}

set<string> Parser::parse_var_decl() {
    // cout << " Inside var_decl\n";
    set<string> var_decl;
    set<string> var_list = parse_var_list();
    for (set<string>::iterator it = var_list.begin(); it != var_list.end(); it++) {
        var_decl.insert(*it);
    }
    consume(COLON);
    parse_type_name();
    consume(SEMICOLON);
    return var_decl;
}

set<string> Parser::parse_var_list() {
    // cout << " Inside parse_var_list\n";
    set<string> var_list;
    var_list.insert(consume(ID).lexeme);

    Token t = lexer.GetToken();
    if (t.token_type == COMMA) {
        // t.Print();
        set<string> var_list2 = parse_var_list();
        for (set<string>::iterator it = var_list2.begin(); it != var_list2.end(); it++) {
            var_list.insert(*it);
        }
    } else {
        lexer.UngetToken(t);
    }
    // cout << " Exiting parse_var_list\n";
    return var_list;
}

// TODO: check for INT, REAL, BOOL
void Parser::parse_type_name() {
    Token t = lexer.GetToken();
    lexer.UngetToken(t);

    if (t.token_type == INT) {
        consume(INT);
    } else if (t.token_type == REAL) {
        consume(REAL);
    } else if (t.token_type == BOOL) {
        consume(BOOL);
    } else {
        syntax_error();
    }
}

vector<Statement*> Parser::parse_body() {
    // Token t = consume(ID);
    // string scope = t.lexeme;
    // consume(LBRACE);
    // addSymbols(parse_public_vars(), scope, "public");
    // addSymbols(parse_private_vars(), scope, "?");

    // vector<Statement*> stmt_list = parse_stmt_list();
    // symbolTable.applyScopingRules(stmt_list, scope);  // apply scoping rules

    // consume(RBRACE);
    // symbolTable.deleteSymbolsFromScope(scope);

    // return stmt_list;
    // cout << " Inside parse body";
    consume(LBRACE);
    vector<Statement*> stmt_list = parse_stmt_list();
    consume(RBRACE);
    return stmt_list;
}

vector<Statement*> Parser::parse_stmt_list() {
    vector<Statement*> stmt_list;
    // Statement* stmt = parse_stmt();
    // if (stmt != NULL) {
    //     stmt_list.push_back(stmt);
    //     this->stmt_list.push_back(stmt);
    // }

    // Token t = lexer.GetToken();
    // if (t.token_type == ID) {
    //     lexer.UngetToken(t);
    //     vector<Statement*> stmt_list2 = parse_stmt_list();
    //     for (int i = 0; i < stmt_list2.size(); i++) {
    //         stmt_list.push_back(stmt_list2.at(i));
    //     }
    // } else {
    //     lexer.UngetToken(t);
    // }
    Statement* stmt = parse_stmt();
    if (stmt != NULL) {
        stmt_list.push_back(stmt);
        this->stmt_list.push_back(stmt);
    }

    Token t = lexer.GetToken();
    lexer.UngetToken(t);
    if (t.token_type == IF || t.token_type == WHILE || t.token_type == SWITCH || t.token_type == WHILE || t.token_type == ID) {
        vector<Statement*> stmt_list2 = parse_stmt_list();
        for (int i = 0; i < stmt_list2.size(); i++) {
            stmt_list.push_back(stmt_list2.at(i));
        }
    }

    return stmt_list;
}

Statement* Parser::parse_stmt() {
    // cout << " Inside parse_stmt\n";
    Statement* stmt = new Statement();

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

    // Token t = lexer.GetToken();
    // if (t.token_type == EQUAL) {
    //     // t.Print();
    //     stmt->lhs = u.lexeme;
    //     stmt->rhs = consume(ID).lexeme;
    //     consume(SEMICOLON);
    // } else if (t.token_type == LBRACE) {
    //     lexer.UngetToken(t);
    //     lexer.UngetToken(u);
    //     parse_body();
    //     return NULL;  // If there is a scope, there is no statement to return
    // } else {
    //     syntax_error();
    // }
    // cout << " Exiting parse_stmt\n";
    return stmt;
}

Statement* Parser::parse_assignment_stmt() {
    Statement* stmt = new Statement();

    Token t = consume(ID);
    stmt->lhs = t.lexeme;
    consume(EQUAL);
    parse_expression();
    consume(SEMICOLON);

    return stmt;
}

// TODO: Fix
vector<Token> Parser::parse_expression() {
    // cout << " Inside parse_expression\n";

    // string expression;
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
    // cout << " Inside binary_operator\n";
    Token t = lexer.GetToken();
    lexer.UngetToken(t);
    // t.Print();
    if (t.token_type == PLUS || t.token_type == MINUS || t.token_type == MULT || t.token_type == DIV || t.token_type == GREATER || t.token_type == LESS || t.token_type == GTEQ || t.token_type == LTEQ || t.token_type == EQUAL || t.token_type == NOTEQUAL) {
        // cout << " Exiting binary_operator\n";
        // t.Print();
        consume(t.token_type);
    } else {
        syntax_error();
    }

    // if (t.token_type == PLUS) {
    //     return "+";
    // } else if (t.token_type == MINUS) {
    //     return "-";
    // } else if (t.token_type == MULT) {
    //     return "*";
    // } else if (t.token_type == DIV) {
    //     return "/";
    // } else if (t.token_type == GREATER) {
    //     return ">";
    // } else if (t.token_type == LESS) {
    //     return "<";
    // } else if (t.token_type == GTEQ) {
    //     return ">=";
    // } else if (t.token_type == LTEQ) {
    //     return "<=";
    // } else if (t.token_type == EQUAL) {
    //     return "=";
    // } else if (t.token_type == NOTEQUAL) {
    //     return "<>";
    // } else {
    //     syntax_error();
    // }

    // cout << " Exiting binary_operator\n";
    return t;
}

Token Parser::parse_primary() {
    // cout << " Inside parse_primary\n";
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
    // cout << " Exiting parse_primary\n";
}

void Parser::parse_if_stmt() {
    // cout << " Inside if_stmt\n";
    consume(IF);
    consume(LPAREN);
    vector<Token> tokens = parse_expression();

    // cout << "\nSTARTING TOKEN LIST\n";
    // for (Token i : tokens) {
    //     i.Print();
    // }
    // cout << "\nENDING TOKEN LIST\n";

    TokenType t = tokens.at(0).token_type;
    if (t != GREATER && t != LESS && t != GTEQ && t != LTEQ && t != EQUAL && t != NOTEQUAL) {
        cout << "TYPE MISMATCH " << tokens.at(0).line_no << " C4\n";
    }

    consume(RPAREN);
    parse_body();
    // cout << " Exiting if_stmt\n";
}

void Parser::parse_while_stmt() {
    // cout << " Inside while_stmt\n";
    consume(WHILE);
    consume(LPAREN);
    parse_expression();
    consume(RPAREN);
    parse_body();
    // cout << " Exiting while_stmt\n";
}

void Parser::parse_switch_stmt() {
    consume(SWITCH);
    consume(LPAREN);
    parse_expression();
    consume(RPAREN);
    consume(LBRACE);
    // cout << "       --- I AM HERE ---";
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

// set<string> Parser::parse_public_vars() {
//     set<string> public_vars;
//     // Check if it contains epsilon
//     Token t = lexer.GetToken();
//     if (t.token_type != PUBLIC) {
//         lexer.UngetToken(t);
//         return public_vars;
//     } else {
//         // It does not contain epsilon
//         lexer.UngetToken(t);
//     }

//    // If it doesn't contain epsilon
//    consume(PUBLIC);
//    consume(COLON);
//    public_vars = parse_var_list();
//    consume(SEMICOLON);

//    return public_vars;
//}

// set<string> Parser::parse_private_vars() {
//     set<string> private_vars;

//    // Check if it contains epsilon
//    Token t = lexer.GetToken();
//    if (t.token_type != PRIVATE) {
//        lexer.UngetToken(t);
//        return private_vars;
//    } else {
//        // It does not contain epsilon
//        lexer.UngetToken(t);
//    }

//    // If it doesn't contain epsilon
//    consume(PRIVATE);
//    consume(COLON);
//    private_vars = parse_var_list();
//    consume(SEMICOLON);

//    return private_vars;
//}

int main() {
    Parser parser;
    parser.parse_program();
}