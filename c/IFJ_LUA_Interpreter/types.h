/**
 * ------------------------------------------
 * Project : Interpreter jazyka IFJ11 
 * Name    : Jednotlive typy
 * File    : types.h
 * Authors : Karel Hala (xhalak00)
 *         : Pavlina Bartikova (xbarti00)
 *         : Tomas Goldmann (xgoldm03)
 *         : Branislav Blaskovic (xblask00)
 * ------------------------------------------
 */
 
typedef struct init_tree Tbin_tree;
 
// Globalne premenne: pocet argumentov funkcie, globalna a lokalna tabulka symbolov
int function_num_of_var;
Tbin_tree *bin_tree;
Tbin_tree *bin_tree_local;

// Struktura pre spravu pamate
typedef struct struct_memory_item TMemory_item;
struct struct_memory_item {
	void *ptr;
	TMemory_item *next;
};

// Struktura obsahujici string a informace o nem
typedef struct
{
  char* chars;
  int length;
  int size_of_alloc;
} my_string;

// Prototyp typu struktury pre vyrazy
typedef struct struct_exp_tree TExp_tree;

// Struktura obsahujici token
typedef struct token {
	int type;
	int error;
	int next_state;
	int line;
	TExp_tree *last_tree;
	my_string name;
} TToken;

// Token ve strome
typedef struct{
	char *key;
	int type;
}TExp_tree_token;

// Hlavni datova struktura pro interpret
typedef struct{
	int type;
	char *id;
	int id_n;
	char *val_str;
	double val_num;
}TData;

// Polozka ve strome pro vyrazy
typedef struct{
	TExp_tree *child;
	TExp_tree_token *exp_token;
	TData data;
}TExp_tree_item;

// Struktura pre symbol
typedef struct{
	char *key;
	int token_type;
	int value_integer;
	TData data;
	my_string value_string;
}TSymbol;

// Struktura instrukcie
typedef struct struct_instruction TInstruction;
struct struct_instruction{
	int operator;
	int inst_num;
	TSymbol operand1;
	int var1_order;
	TSymbol operand2;
	int var2_order;
	int var_total;
	TExp_tree *tree;
	TInstruction* instruction_jump;
	TInstruction* next;
};

// Hlavicka zoznamu instrukcii
typedef struct inst_list{
	int num_of_instructions;
	TInstruction* actual_instruction;
	TInstruction* last_jump_instruction;
	TInstruction* first;
	TInstruction* last;
}TInst_list;

// Zasobnik pre konstrukciu IF (rozsirenie)
typedef struct struct_stack_if TStack_if;
struct struct_stack_if {
	TInstruction **data;
	int top;
	int size_of_alloc;
};

// Binarny vyhledavaci strom
struct init_tree{
	char* value;
	int not_declared;
	int function_arg_num;
	int order_of_var;
	TInstruction* instruction;
	Tbin_tree* right;
	Tbin_tree* left;
};
typedef struct
{
	Tbin_tree *root;
}tree;

// Symbol pouzity pri vyrazoch
typedef struct {
	int terminal_symbol;
	TToken *exp_token;
	TExp_tree *tree;
	char ltgt;
} TSymSymbol;

// Zasobnik pre vyrazy
typedef struct {
	TSymSymbol* data;
	int size;
	int top;
	int max_top;
} TSymStack;

// Vyrazovy strom
struct struct_exp_tree{
	TExp_tree_item *left;
	TExp_tree_item *right;
	int operator;
};

// Struktura pre uchovanie klucoveho slova
typedef struct keyword {
	int type;
	char name[10];
} TKeyword;

//struktury pro zasobnik promennych - interpret
typedef struct {
	TData data;
	TData *val_mem;
	TInstruction *instruction;
} TVarStackData;

// Zasobnik s premennymi
typedef struct {
	TVarStackData* data;
	int size;
	int top;
} TVarStack;

// Vysledok po vypocte vyrazoveho stromu
typedef struct {
  TData data;
  int error;
} TExpResult;

// Prototyp pre premennu
typedef TData* TVal_mem;

