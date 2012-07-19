/**
 * ------------------------------------------
 * Project : Interpreter jazyka IFJ11 
 * Name    : Syntakticky analyzator
 * File    : parser.h
 * Authors : Karel Hala (xhalak00)
 *         : Pavlina Bartikova (xbarti00)
 *         : Tomas Goldmann (xgoldm03)
 *         : Branislav Blaskovic (xblask00)
 * ------------------------------------------
 */

#define SYM_STACK_INIT_SIZE 200
#define SYM_STACK_PLUS_SIZE 100
#define IF_STACK_ALLOC 20

#define MAX_PT 17

static const char precedent_table[MAX_PT][MAX_PT] = {
	">><<<<>>>>>>><> >",
	">><<<<>>>>>>><> >",
	">>>>><>>>>>>><> >",
	">>>>><>>>>>>><> >",
	">>>>><>>>>>>><> >",
	">>>>>>>>>>>>><> >",
	"<<<<<<>>>>>>><> >",
	"<<<<<<<>>>>>><> >",
	"<<<<<<<>>>>>><> >",
	"<<<<<<<>>>>>><> >",
	"<<<<<<<>>>>>><> >",
	"<<<<<<<>>>>>><> >",
	"<<<<<<<>>>>>><> >",
	"<<<<<<<<<<<<<<=  ",
	">>>>>>><<<<<< > >",
	"                 ",
	"<<<<<<<<<<<<<<   "
};

TSymStack * sym_stack_init(TSymStack *stack);
void exp_tree_print( TExp_tree *tree, int offset );
void sym_stack_free(TSymStack *stack);
void sym_stack_push(TSymStack *stack, TSymSymbol symbol);
void sym_stack_pop(TSymStack *stack, TSymSymbol *symbol);
void sym_stack_top(TSymStack *stack, TSymSymbol *symbol);
void napis_token(char *kde, TToken token);
int func_write (FILE *F, TToken *token, TInst_list *inst_list);
int func_type(FILE *F, TToken *token, TInst_list *inst_list);
int func_substr(FILE *F, TToken *token, TInst_list *inst_list);
int func_sort(FILE *F, TToken *token, TInst_list *inst_list);
int func_find(FILE *F, TToken *token, TInst_list *inst_list);
int t_local(FILE *F, TToken *token, TInst_list *inst_list);
int t_func_body(FILE *F, TToken *token, TInst_list *inst_list);
int t_func_while (FILE *F, TToken *token, TInst_list *inst_list);
int t_cfunc_param_n(FILE *F, TToken *token, TInst_list *inst_list, int num_of_arg);
int t_expression(FILE *F, TToken *token, TInst_list *inst_list, int first_token);
int t_cfunc_param(FILE *F, TToken *token, TInst_list *inst_list, int num_of_arg);
int t_func_param_n(FILE *F, TToken *token, TInst_list *inst_list, char *function_name);
int t_func_param(FILE *F, TToken *token, TInst_list *inst_list, char *function_name);
int t_function(FILE *F, TToken *token, TInst_list *inst_list);
int t_read_sem(char *param);
int t_read(FILE *F, TToken *token, TSymbol variable, TInst_list *inst_list);
int t_function_call(FILE *F, TToken *token, TInst_list *inst_list, int num_of_arg);
int t_id_assign(FILE *F, TToken *token, TInst_list *inst_list);
int t_func_if(FILE *F, TToken *token, TInst_list *inst_list);
int start_parse(FILE *F, TInst_list *inst_list);
int trash_func_param(FILE *F, TToken *token, int start_without_comma);
int t_func_fordo(FILE *F, TToken *token, TInst_list *inst_list);
