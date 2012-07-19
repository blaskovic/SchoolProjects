/**
 * ------------------------------------------
 * Project : Interpreter jazyka IFJ11 
 * Name    : Lexikalni analyzator
 * File    : scaner.h
 * Authors : Karel Hala (xhalak00)
 *         : Pavlina Bartikova (xbarti00)
 *         : Tomas Goldmann (xgoldm03)
 *         : Branislav Blaskovic (xblask00)
 * ------------------------------------------
 */

// chybove stavy
/*enum SCANER_ERRORS {
	E_SCAN_OK = 0,
	E_SCAN_MALLOC,
	E_SCAN_LEX,
	E_SCAN_NUMBER,
	E_SCAN_EOF,
};*/

// error statusy podla zadania
enum ERROR_CODES {
	ERR_CODE_OK = 0,
	ERR_CODE_LEX,
	ERR_CODE_SYN,
	ERR_CODE_SEM,
	ERR_CODE_INTER,
	ERR_CODE_IN_INTER, // (5)
	
	E_SCAN_OK,
	E_SCAN_MALLOC,
	E_SCAN_LEX,
	E_SCAN_NUMBER,
	E_SCAN_EOF,
	
};

// stavy automatu
enum SCANER_STATES {
	ST_START = 0,
	ST_COMMENT,
	ST_BLOCK_COMMENT,
	ST_NUMBER,
	ST_ASSIGN,
	ST_MINUS,
	ST_WAVE,
	ST_DOT,
	ST_LOWER,
	ST_GREATER,
	ST_STRING,
	ST_END_OF_STRING,
	ST_VAR,
	ST_KEY_OR_VAR
};

// typy tokenu
// nemenit poradie!
enum TOKEN_TYPES {
	// pre tabulku
	TK_PLUS,		// +
	TK_MINUS,		// -
	TK_MULTIPLY,		// *
	TK_DIVIDE,		// /
	TK_MODULO,		// %
	TK_EXP,		// ^
	TK_JOIN,		// ..
	TK_LOWER,		// <
	TK_GREATER,		// >
	TK_LOWER_EQUAL,		// <=
	TK_GREATER_EQUAL,		// >= (10)
	TK_NON_EQUAL,		// ~=	
	TK_EQUAL,		// == 
	TK_LBRACKET,		// (
	TK_RBRACKET,		// )
	TK_ID,	// identifikator var/num
	TK_DOLLAR, // $
	
	// ostatni
	TK_SEMICOLON,		// ;
	TK_COMMA,		// ,
	TK_VAR,		// promenna
	TK_QUOTE,		// " (20)
	TK_ASSIGN,		// =	
	TK_NUMBER,		// cislo
	TK_STRING,		// retezec

	// klicova a rezervovana slova
	TK_KEY_DO,
	TK_KEY_ELSE,
	TK_KEY_END,
	TK_KEY_FUNCTION,
	TK_KEY_IF,
	TK_KEY_LOCAL,
	TK_KEY_NIL,	// (30)
	TK_KEY_READ,	
	TK_KEY_RETURN,
	TK_KEY_THEN,
	TK_KEY_WHILE,
	TK_KEY_WRITE,
	TK_KEY_AND,
	TK_KEY_BREAK,
	TK_KEY_ELSEIF,
	TK_KEY_FOR,
	TK_KEY_IN,	// (40)
	TK_KEY_NOT,	
	TK_KEY_OR,
	TK_KEY_REPEAT,
	TK_KEY_UNTIL,
	TK_KEY_TRUE,
	TK_KEY_FALSE,
	TK_KEY_TYPE,
	TK_KEY_SUBSTR,
	TK_KEY_FIND,
	TK_KEY_SORT,	// (50)
	
	TK_EXP_BLOCK, // block pre zasobnik
	TK_ONE_OPERAND,
	TK_UNINIT
};


static const short NUM_KEYWORDS = 27;

static const TKeyword KEYWORD_TABLE[] = {
	{TK_KEY_DO, "do"},
	{TK_KEY_ELSE, "else"},
	{TK_KEY_END, "end"},
	{TK_KEY_FUNCTION, "function"},
	{TK_KEY_IF, "if"},
	{TK_KEY_LOCAL, "local"},
	{TK_KEY_NIL, "nil"},
	{TK_KEY_READ, "read"},
	{TK_KEY_RETURN, "return"},
	{TK_KEY_THEN, "then"},
	{TK_KEY_WHILE, "while"},
	{TK_KEY_WRITE, "write"},
	{TK_KEY_AND, "and"},
	{TK_KEY_BREAK, "break"},
	{TK_KEY_ELSEIF, "elseif"},
	{TK_KEY_FOR, "for"},
	{TK_KEY_IN, "in"},
	{TK_KEY_NOT, "not"},
	{TK_KEY_OR, "or"},
	{TK_KEY_REPEAT, "repeat"},
	{TK_KEY_UNTIL, "until"},
	{TK_KEY_TRUE, "true"},
	{TK_KEY_FALSE, "false"},
	{TK_KEY_TYPE, "type"},
	{TK_KEY_SUBSTR, "substr"},
	{TK_KEY_FIND, "find"},
	{TK_KEY_SORT, "sort"}
};

#define EOL '\n'

// prototypy funkci
int key_or_var(char *word);
int get_token(FILE *F, TToken *token);
TToken *copy_token(TToken *token);
