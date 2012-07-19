/**
 * ------------------------------------------
 * Project : Interpreter jazyka IFJ11 
 * Name    : Interpreter
 * File    : interpreter.h
 * Authors : Karel Hala (xhalak00)
 *         : Pavlina Bartikova (xbarti00)
 *         : Tomas Goldmann (xgoldm03)
 *         : Branislav Blaskovic (xblask00)
 * ------------------------------------------
 */


 //velikost zasobniku
#define VAR_STACK_INIT_SIZE 1000
#define VAR_STACK_PLUS_SIZE 100

#define DEBUG_STACK_TOP var_stack.top+1
#define COUNT_CHAR_READ_LINE 10
#define COUNT_CHAR_READ_NUMBER 10
#define COUNT_CHAR_READ_EOF 100
#define MAX_LENGHT_DATA_TYPE 10

enum FUNC {
	F_NONE,
	F_FIND,
	F_SORT,
	F_SUBSTR,
	F_TYPE
};


// typy promenne, specialni typ je pocetek pushnutych polozek pro funkci,
// nejdrive se udela push argumentu a na zaver se da informace o tom kolik argumetu
// pro danou funkci je na zasobniku, toto by nemelo nicemu vadi kdyz se da pohybovat
//v zasobniku od vrcholu

enum EXP_TREE_TYPES {
	STRING = 0,
	DOUBLE,
	INTEGER,
	NUMBER,
	VAR,
	BOOL,
	NIL
};

// typy instrukcii
enum INSTRUCTIONS_TYPES {
	INST_NULL = 0, // akoze len nejake drzatko
	
	INST_READ,
	INST_CALL_FUNCTION, // pozre sa do stromu a skoci v instrukcnej paske na deklaraciu
	INST_INIT_FUNCTION, // deklaracia funkcie - sem sa skace pri volani
	INST_END_OF_FUNCTION, // ak pride, tak vo funkcii nebol return
	INST_END_OF_MAIN,	//konec hlavni funkce main
	
	INST_CALL_TYPE,	//knihovni funkce type vrati datovy typ
	INST_CALL_SORT,	//knihovni funkce sort pro serazeni zadaneho stringu
	INST_CALL_FIND,	//knihovni funkce find pro nalezeni podstringu
	
	INST_CALL_SUBSTR,	//knihovni funkce substr pro nalezeni podstringu
	INST_CALL_SUBSTR_NUM1,	// substr cislo 1
	INST_CALL_SUBSTR_NUM2,	// substr cislo 2
	INST_CALL_WRITE,	//knihovni funkce na vypis expression
	
	INST_ID_ASSIGN,		//prirazeni do promenne
	INST_CALL_WHILE, 	//zacatek while
	INST_CALL_END_WHILE,	//konec while
	INST_CALL_IF,		//zacatek IF
	INST_CALL_ELSE,		//vetev else
	INST_CALL_END_IF,	//konec if
	
	INST_ARG_VAR, // argument funkcie, treba pushnut na zasobnik
	INST_LOCAL_VAR, // zadane local premenna; treba pushnut na zasobnik
	
	INST_CALL_RETURN,	//navratova hodnota funkce
	
	INST_CALL_ARG, // vola sa funkcia s argumentom
	
	INST_JUMP_BACK, // skace na inst_list->last_jump_instruction
	
	INST_LAST, // drzatko konca
};

static const char INST_TEXTY[INST_LAST][25] = {
	"INST_NULL",
	
	"INST_READ",
	"INST_CALL_FUNCTION",
	"INST_INIT_FUNCTION",
	"INST_END_OF_FUNCTION",
	"INST_END_OF_MAIN",
	
	"INST_CALL_TYPE",
	"INST_CALL_SORT",
	"INST_CALL_FIND",
	"INST_CALL_SUBSTR",
	"INST_CALL_SUBSTR_NUM1",
	"INST_CALL_SUBSTR_NUM2",
	"INST_CALL_WRITE",
	
	"INST_ID_ASSIGN",
	"INST_CALL_WHILE", 	
	"INST_CALL_END_WHILE",
	"INST_CALL_IF",
	"INST_CALL_ELSE",
	"INST_CALL_END_IF",
	
	"INST_ARG_VAR",
	"INST_LOCAL_VAR",
	
	"INST_CALL_RETURN",
	
	"INST_CALL_ARG",
	
	"INST_JUMP_BACK",
};


// prototypy
void init_exp_data(TData *data);
void inst_init_list(TInst_list *inst_list);
void inst_print(TInst_list *inst_list);
void inst_free(TInst_list *inst_list);
TInstruction *  inst_add_last(TInst_list *inst_list, int operator, TSymbol operand1, TSymbol operand2, TExp_tree *tree);
TSymbol create_operand(TToken token);
int start_interpret(TInst_list *inst_list,Tbin_tree *bin_tree);
TInstruction *inst_update_instruction_jump(TInstruction *instruction, TInstruction *jump_instruction);
TInstruction * inst_update_var_order(TInstruction *instruction, int num1, int num2);
TInstruction * inst_update_var_total(TInstruction *instruction, int var_total);

TExpResult evaluate_exp_tree(TExp_tree* tree,TVal_mem val_mem);