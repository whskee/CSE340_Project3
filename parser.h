#include <set>
#include <string>
#include <vector>

#include "lexer.h"
using std::set;
using std::string;
using std::vector;

struct Symbol {
    string name;
    string type;
};

class SymbolTable {
   public:
    Symbol *symbolExists(string symbolName);
    void addSymbol(Symbol *symbol);
    void printSymbolTable();

   private:
    vector<Symbol *> symbols;
    vector<Symbol *>::iterator it;
};

class Parser {
   public:
    LexicalAnalyzer lexer;
    SymbolTable symbolTable;
    void parse_program();

   private:
    // Parse Functions
    void parse_global_vars();
    void parse_var_decl_list();
    void parse_var_decl();
    vector<Token> parse_var_list();
    string parse_type_name();
    void parse_body();
    void parse_stmt_list();
    void parse_stmt();
    void parse_assignment_stmt();
    vector<Token> parse_expression();
    Token parse_unary_operator();
    Token parse_binary_operator();
    Token parse_primary();
    void parse_if_stmt();
    void parse_while_stmt();
    void parse_switch_stmt();
    void parse_case_list();
    void parse_case();

    // Helper Functions
    void syntax_error();
    Token consume(TokenType expected_type);
};