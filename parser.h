#include <set>
#include <string>
#include <vector>

#include "lexer.h"
using std::set;
using std::string;
using std::vector;

struct Statement {
    string lhs;
    string rhs;
};

struct Symbol {
    string name;
    string scope;
    string visibility;
};

class SymbolTable {
   public:
    vector<Symbol *> symbols;
    Symbol *symbolExists(string symbolName);
    void applyScopingRules(vector<Statement *>, string scope);
    void addSymbol(Symbol *symbol);
    void deleteSymbolsFromScope(string scope);
    void printSymbolTable();

   private:
    int indexOf(string symbol);
    void applyScopingRule(Statement *stmt, string scope);
    void deleteSymbol(Symbol *symbol);
};

class Parser {
   public:
    LexicalAnalyzer lexer;
    SymbolTable symbolTable;
    void parse_program();
    vector<Statement *> stmt_list;

   private:
    // Parse Functions
    set<string> parse_global_vars();
    set<string> parse_var_decl_list();  // TODO
    set<string> parse_var_decl();       // TODO
    set<string> parse_var_list();
    void parse_type_name();            // TODO
    vector<Statement *> parse_body();  // TODO
    vector<Statement *> parse_stmt_list();
    Statement *parse_stmt();
    Statement *parse_assignment_stmt();
    void parse_expression();
    void parse_unary_operator();
    void parse_binary_operator();
    void parse_primary();
    void parse_if_stmt();
    void parse_while_stmt();
    void parse_switch_stmt();
    void parse_case_list();       // TODO
    void parse_case();       // TODO
    set<string> parse_public_vars();    //??
    set<string> parse_private_vars();   //??

    // Helper Functions
    void syntax_error();
    Token consume(TokenType expected_type);
    void printStatements(vector<Statement *> stmt_list);
    void addSymbols(set<string> symbols, string scope, string visibility);
};