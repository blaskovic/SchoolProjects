/**
 * ------------------------------------------
 * Project : Interpreter jazyka IFJ11 
 * Name    : Syntakticky analyzator
 * File    : parser.c
 * Authors : Karel Hala (xhalak00)
 *         : Pavlina Bartikova (xbarti00)
 *         : Tomas Goldmann (xgoldm03)
 *         : Branislav Blaskovic (xblask00)
 * ------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include "types.h"
#include "my_memory.h"
#include "my_string.h"
#include "scaner.h"
#include "interpreter.h"
#include "ial.h"
#include "parser.h"
 
/**
 * Funkce prevadejici token do datove slozky.
 * @param key char* klic z tokenu
 * @param type int typ tokenu
 * @param data TData* nova polozka ve stromku
 * @return int chybovy stav.
 */
int convert_token_to_type(char *key, int type, TData *data)
{
	data->val_num=0;
	data->val_str=NULL;
	data->type=NIL;

	if(type==TK_STRING)
	{
		//priprava noveho retezce
		data->val_str = (char *) my_malloc(sizeof(char)*(strlen(key)+1));
		if(data->val_str==NULL)
		{
			return ERR_CODE_IN_INTER;
		}
		strcpy(data->val_str,key);
		data->type=STRING;
	}
	else if(type==TK_NUMBER)
	{
		data->val_num=strtod(key,NULL);
		data->type=NUMBER;
	}
	else if(type==TK_KEY_FALSE)
	{
		data->val_num=0;
		data->type=BOOL;
	}
	else if(type==TK_KEY_TRUE)
	{
		data->val_num=1;
		data->type=BOOL;
	}
	else if(type==TK_KEY_NIL)
	{
		data->val_num=1;
		data->type=NIL;
	}
	else if(type==TK_VAR)
	{
		data->id = (char *) my_malloc(sizeof(char)*(strlen(key)+1));
		
		if(data->id==NULL)
		{
			return ERR_CODE_IN_INTER;
		}
		
		strcpy(data->id,key);	
		data->type=VAR;
	}
	else
	{
		data->type=NIL;
	}	
	return ERR_CODE_OK;
}
 
/**
 * Vytvori vetvu vyrazoveho stromu
 * @param key char* klic z tokenu
 * @param type int typ tokenu
 * @param child TExp_tree* potomok - substrom
 * @return TExp_tree_item* vysledna vetva stromu
 */
TExp_tree_item *exp_tree_make_item(char *key, int type, TExp_tree *child)
{
	TExp_tree_item *new = (TExp_tree_item *)my_malloc(sizeof(TExp_tree_item));
	
	new->child = child;
	
	if(key != NULL)
	{
		new->exp_token = (TExp_tree_token *)my_malloc(sizeof(TExp_tree_token));
		
		//toto prevadi string do datove slozky (Tomas)
		convert_token_to_type(key,type, &new->data);
		
		new->data.id_n = 0;
		
		// cislo premennej
		if(bin_tree_local != NULL)
		{
			Tbin_tree *variable = bvs_get_key(bin_tree_local, key);
			if(variable != NULL) new->data.id_n = variable->order_of_var;
		}
		
		//toto se bude moc zrusit
		new->exp_token->key = (char *)my_malloc(sizeof(char)*(strlen(key)+1));
		new->exp_token->type = type;
		strcpy(new->exp_token->key, key);
	}
	else
	{
		// printf("STROM token je NULL\n");
	}
	
	return new;
}

/**
 * Vytvori vyrazovy strom z dvoch vetvi
 * @param operator int operator medzi stromami
 * @param left TExp_tree_item* lavy strom
 * @param right TExp_tree_item* pravy strom
 * @return TExp_tree* vysledny strom
 */
TExp_tree *exp_tree_make(int operator, TExp_tree_item *left, TExp_tree_item *right)
{
	TExp_tree *new = (TExp_tree*)my_malloc(sizeof(TExp_tree));
	
	new->left = left;
	new->right = right;
	new->operator = operator;
	
	return new;
}

/*
 * Pomocne funkcie
void print_space(int num)
{
	int i;
	for(i=0; i < num; i++)
	{
		if(i%4 == 0) printf(".");
		else
		printf(" ");
	}
}

void exp_tree_print(TExp_tree *tree, int offset)
{

	if(tree == NULL) { printf("Strom je NULL.\n"); return; }
	
	printf("\n");
	
	print_space(offset);
	printf(">> Operator: %d\n", tree->operator);
	
	// jeden operator
	if(tree->operator == TK_ONE_OPERAND)
	{
		print_space(offset);
		printf("Vyraz ma iba 1 operand!\n");
		print_space(offset);
		printf("Token: %s %d (order: %d)\n", tree->left->exp_token->key, tree->left->exp_token->type, tree->left->data.id_n);
		return;
	}
	
	print_space(offset);
	printf("-Koukam doleva:\n");
	// ak je tam dalsi strom, inak token
	
	if(tree->left == NULL)
	{
		print_space(offset);
		printf("Vlavo je NULL.\n");
		return;
	}
	
	if(tree->left->child != NULL)
	{
		print_space(offset);
		printf("Nasel jsem substrom vlevo!\n");
		exp_tree_print(tree->left->child, offset+4);
	}
	else if(tree->left->exp_token != NULL)
	{
		print_space(offset);
		printf("Levy token: %s %d (Order: %d)\n", tree->left->exp_token->key, tree->left->exp_token->type, tree->left->data.id_n);
	}
	else
	{
		print_space(offset);
		printf("Leva vetev je divne prazdna...\n");
	}
	
	if(tree->right == NULL)
	{
		print_space(offset);
		printf("Vpravo je NULL.\n");
		return;
	}
	
	print_space(offset);
	printf("-Koukam doprava:\n");
	
	// ak je tam dalsi strom, inak token
	if(tree->right->child != NULL)
	{
		print_space(offset);
		printf("Nasel jsem substrom vpravo!\n");
		exp_tree_print(tree->right->child, offset+4);
	}
	else if(tree->right->exp_token != NULL)
	{
		print_space(offset);
		printf("Pravy token: %s %d (Order: %d)\n", tree->right->exp_token->key, tree->right->exp_token->type, tree->right->data.id_n);
	}
	else
	{
		print_space(offset);
		printf(" Prava vetev je divne prazdna...\n");
	}

}
*/

/**
 * Inicializuje zasobnik pre pracu s vyrazmi
 * @param stack TSymStack* ukazatel na zasobnik
 * @return TSymStack* zasobnik
 */
TSymStack *sym_stack_init(TSymStack *stack)
{
	stack = my_malloc(sizeof(TSymStack));
	if(stack == NULL)
	{
		// printf("chyba");
		// return chyba
	}
	stack->top = 0;
	stack->max_top = 0;
	stack->size = SYM_STACK_INIT_SIZE;
	stack->data = my_malloc(sizeof(TSymSymbol)*SYM_STACK_INIT_SIZE);
	
	if(stack->data == NULL)
	{
		// return chyba
	}
	
	return stack;
}

/**
 * Uvolni zasobnik, s ktorym sa pracovalo vo vyrazoch
 * @param stack TSymStack* ukazatel na zasobnik
 * @return TSymStack* zasobnik
 */
void sym_stack_free(TSymStack *stack)
{
	my_free(stack->data);
	stack->top = -1;
	stack->size = 0;
}

/**
 * Vlozi hodnotu na vrchol vyrazoveho zasobniku
 * @param stack TSymStack* ukazatel na zasobnik
 * @param symbol TSymSymbol ukazatel na zasobnik
 * @return TSymStack* zasobnik
 */
void sym_stack_push(TSymStack *stack, TSymSymbol symbol)
{
	TToken *new_token = NULL;
	
	stack->top++;
	//printf("...%d -> %d \n", stack->top, symbol.type);
	
	if(stack->size <= stack->top) 
	{
		stack->data = my_realloc(stack->data, sizeof(TSymSymbol)*SYM_STACK_PLUS_SIZE);
		if(stack->data == NULL)
		{
			// return chyba
		}
		stack->size += SYM_STACK_PLUS_SIZE;
	}
	
	if(symbol.exp_token != NULL)
	{
		new_token = my_malloc(sizeof(TToken));
		new_token->type = symbol.exp_token->type;
		new_token->name.chars = my_malloc(sizeof(char)*(strlen(symbol.exp_token->name.chars)+1));
		strcpy(new_token->name.chars, symbol.exp_token->name.chars);
		symbol.exp_token = new_token;
	}
	else
	{
		symbol.exp_token = NULL;
	}
	stack->data[stack->top] = symbol;
	
	if(stack->max_top < stack->top ) stack->max_top = stack->top;
}

/**
 * Vyberie vrchol zasobnika a znizi ho
 * @param stack TSymStack* ukazatel na zasobnik
 * @param symbol TSymSymbol* ukazatel na symbol
 * @return void
 */
void sym_stack_pop(TSymStack *stack, TSymSymbol *symbol)
{
	if(stack->data[stack->top].terminal_symbol != TK_DOLLAR)
	{
		// printf("pop: %d -> %d \n", stack->top, stack->data[stack->top].terminal_symbol);
		symbol = &stack->data[stack->top];
		stack->top--;
	}
}

/**
 * Vyberie vrchol zasobnika ale nic nemeni
 * @param stack TSymStack* ukazatel na zasobnik
 * @param symbol TSymSymbol* ukazatel na symbol
 * @return TSymStack* zasobnik
 */
void sym_stack_top(TSymStack *stack, TSymSymbol *symbol)
{
	symbol = &stack->data[stack->top];
}

/**
 * Vypise token - debuggovacia funkcia
 * @param kde char* text pre debug
 * @param token TToken token, o ktorom chceme vypisat podrobnosti
 * @return TSymStack* zasobnik
 */
void napis_token(char *kde, TToken token)
{
	//printf("%s Radek: %d\t\t\tToken: %d\t\t\t%s\n", kde, token.line, token.type, token.name.chars);
}

/**
 * Overenie semantiky vyrazu write()
 * @param tree TExp_tree* strom vyrazov
 * @return int chybovy stav
 */
int check_write_seman(TExp_tree *tree)
{
	int check = 0;
	if(tree == NULL) return ERR_CODE_OK;
	// jeden operand
	if(tree->operator == TK_ONE_OPERAND)
	{		
		if(tree->left != NULL && tree->left->exp_token != NULL)
		{
			// co tam je?
			check = tree->left->exp_token->type;
			
			// ak je tam len premenna, tak je to ok
			if(check == TK_VAR)
			{
				return ERR_CODE_OK;
			}
			
			// moze tam byt len string a cislo
			if(check != TK_STRING && check != TK_NUMBER)
			{
				return ERR_CODE_SEM;
			}
			
			return ERR_CODE_OK;
		}
	}
	// vyraz
	else
	{
		// musime sa pozriet na operator
		// ten urcuje vysledok
		check = tree->operator;
		if(
			check == TK_EQUAL ||
			check == TK_NON_EQUAL ||
			check == TK_LOWER ||
			check == TK_LOWER_EQUAL ||
			check == TK_GREATER ||
			check == TK_GREATER_EQUAL
		)
		{
			return ERR_CODE_SEM;
		}
		
		return ERR_CODE_OK;
	}
	
	return ERR_CODE_OK;
}

/**
 * Gramatikaa pre funkciu write()
 * @param F FILE* deskriptor na subor
 * @param token TToken* ukazatel na token
 * @param inst_list TInst_list* ukazatel na instrukcnu pasku
 * @return int chybovy stav
 */
int t_func_write (FILE *F, TToken *token, TInst_list *inst_list)
{
	// write (expression)
	int error = ERR_CODE_OK;
	
	if((error = get_token(F, token)) != E_SCAN_OK) return error;
	napis_token("t_func_write:", *token);
	
	// ( 
	if(token->type != TK_LBRACKET) return ERR_CODE_SYN;
	
	// volani expression
	if((error = t_expression(F, token, inst_list, 0)) != ERR_CODE_OK) return error;
	
	// nieco musi prist
	if(token->last_tree != NULL)
	{
		// overenie semantiky
		if((error = check_write_seman(token->last_tree)) != ERR_CODE_OK) return error;
		
		// pozor, tady to ulozi posledni nactenou zavorku, co dostane z expression
		inst_add_last(inst_list, INST_CALL_WRITE, create_operand(*token) , create_operand(*token), token->last_tree);
	
		// dokud budou expression oddeleny carkami
		while (token->type == TK_COMMA)
		{
			if((error = t_expression(F, token, inst_list, 0)) != ERR_CODE_OK) return error;
			
			// po ciarke nam nic neprislo
			if(token->last_tree == NULL) return ERR_CODE_SYN;
			
			// overenie semantiky
			if((error = check_write_seman(token->last_tree)) != ERR_CODE_OK) return error;
			
			inst_add_last(inst_list, INST_CALL_WRITE, create_operand(*token) , create_operand(*token), token->last_tree);
		}
	}
	// )
	if(token->type != TK_RBRACKET) return ERR_CODE_SYN;
	
	if((error = get_token(F, token)) != E_SCAN_OK) return error;
	napis_token("t_func_write:", *token);
	
	if(token->type != TK_SEMICOLON) return ERR_CODE_SYN;
	
	return ERR_CODE_OK;
}

/**
 * Gramatika pre konstrukciu: local variable
 * @param F FILE* deskriptor na subor
 * @param token TToken* ukazatel na token
 * @param inst_list TInst_list* ukazatel na instrukcnu pasku
 * @return int chybovy stav
 */
int t_local(FILE *F, TToken *token, TInst_list *inst_list)
{
	// local id ;
	int error = 0;
	char *var_name = NULL;
	
	if((error = get_token(F, token)) != E_SCAN_OK) return error;
	napis_token("t_local:", *token);
	
	// existuje taka funkcia?	
	if(bvs_search(bin_tree, token->name.chars) == 1) {printf("chyba\n");return ERR_CODE_SEM;}
	
	if(bvs_search(bin_tree_local, token->name.chars) == 1) return ERR_CODE_SEM;
	
	var_name = my_malloc(sizeof(char)*(strlen(token->name.chars)+1));
	if(var_name == NULL) return ERR_CODE_IN_INTER;
	strcpy(var_name, token->name.chars);
	
	// vytvorenie
	TSymbol variable = create_operand(*token);
	
	TInstruction *inst_init = inst_add_last(inst_list, INST_LOCAL_VAR, variable, variable, NULL);
	
	// id
	if(token->type != TK_VAR) return ERR_CODE_SYN;
	
	if((error = get_token(F, token)) != E_SCAN_OK) return error;
	napis_token("t_local:", *token);
	
	// ;
	if(token->type == TK_SEMICOLON)
	{
		function_num_of_var++;
		
		bin_tree_local = bvs_make_tree(bin_tree_local, var_name, 0);
		inst_update_var_order(inst_init, function_num_of_var, function_num_of_var);
		return ERR_CODE_OK;
	}
	else
	// ak je tam priradenie
	if(token->type == TK_ASSIGN)
	{
		if((error = t_expression(F, token, inst_list, 0)) != ERR_CODE_OK) return error;
		
		if(token->last_tree == NULL) return ERR_CODE_SYN;
		
		function_num_of_var++;
		bin_tree_local = bvs_make_tree(bin_tree_local, var_name, 0);
		
		inst_update_var_order(inst_init, function_num_of_var, function_num_of_var);
		inst_add_last(inst_list, INST_ID_ASSIGN, variable, variable, token->last_tree);
		
		// ;
		if(token->type != TK_SEMICOLON) return ERR_CODE_SYN;
	}
	else
	{
		return ERR_CODE_SYN;
	}
	
	return ERR_CODE_OK;
}

/**
 * Gramatika pre telo funkcie
 * @param F FILE* deskriptor na subor
 * @param token TToken* ukazatel na token
 * @param inst_list TInst_list* ukazatel na instrukcnu pasku
 * @return int chybovy stav
 */
int t_func_body(FILE *F, TToken *token, TInst_list *inst_list)
{
	/*
	1. <body>               ;
	2. <body>               <IF>
	3. <body>               <WHILE>
	4. <body>               <CALL_FUNCTION>
	5. <body>               <ASSIGMENT>
	6. <body>               <WRITE>
	7. <body>               <RETURN>
	8. <body>               <READ>
	-------		konci		-------
	8. <body>				end;
	*/
	
	int error = ERR_CODE_OK;
	
	while(error != E_SCAN_EOF)
	{
		
		//volani jednotlivych funkci
		if(token->type == TK_KEY_WHILE)
		{
			if((error = t_func_while(F, token, inst_list)) != ERR_CODE_OK) return error;
		}
		else if(token->type == TK_KEY_FOR)
		{
			if((error = t_func_fordo(F, token, inst_list)) != ERR_CODE_OK) return error;
		}
		else if(token->type == TK_VAR)
		{
			if((error = t_id_assign(F, token, inst_list)) != ERR_CODE_OK) return error;
		}
		else if(token->type == TK_KEY_IF)
		{
			if((error = t_func_if(F, token, inst_list)) != ERR_CODE_OK) return error;
		}
		else if(token->type == TK_KEY_WRITE)
		{	
			if((error = t_func_write(F, token, inst_list)) != ERR_CODE_OK) return error;
		}
		else if(token->type == TK_KEY_RETURN)
		{
			//volani expression
			if((error = t_expression(F, token, inst_list, 0)) != ERR_CODE_OK) return error;
			
			if(token->last_tree == NULL) return ERR_CODE_SYN;
			
			inst_add_last(inst_list, INST_CALL_RETURN, create_operand(*token), create_operand(*token), token->last_tree);
			
			// ;
			if(token->type != TK_SEMICOLON) return ERR_CODE_SYN;
		}
		else if(token->type == TK_KEY_ELSE)
		{
			return ERR_CODE_OK;
		}
		else if(token->type == TK_KEY_ELSEIF)
		{
			return ERR_CODE_OK;
		}
		else if(token->type == TK_KEY_END)
		{
			return ERR_CODE_OK;
		}
		else
		{
			//dostane neco jineho, nez je napsano v gramatice
			return ERR_CODE_SYN;
		}
		
		//nacteni noveho tokenu
		if((error = get_token(F, token)) != E_SCAN_OK) return error;
		napis_token("t_func_body:", *token);
	}
	return ERR_CODE_SEM;
}

/**
 * Gramatika pre cyklus for..do - rozsirenie
 * @param F FILE* deskriptor na subor
 * @param token TToken* ukazatel na token
 * @param inst_list TInst_list* ukazatel na instrukcnu pasku
 * @return int chybovy stav
 */
int t_func_fordo(FILE *F, TToken *token, TInst_list *inst_list)
{
	//for id = <expr>, [<expr>, <expr>] do <body> end ;
	int error = ERR_CODE_OK;
	TInstruction *inst_for = NULL;
	TInstruction *inst_end_for = NULL;
	TExp_tree *strom_step = NULL;
	
	if((error = get_token(F, token)) != E_SCAN_OK) return error;
	napis_token("t_func_fordo:", *token);
	
	// existuje uz premenna?
	if(bvs_search(bin_tree_local, token->name.chars) == 0) return ERR_CODE_SEM;
	
	// ulozime premennu
	TSymbol variable = create_operand(*token);
	
	if((error = get_token(F, token)) != E_SCAN_OK) return error;
	napis_token("t_func_fordo:", *token);
	
	// = 
	if(token->type != TK_ASSIGN) return ERR_CODE_SYN;
	
	// volani expression
	if((error = t_expression(F, token, inst_list, 0)) != ERR_CODE_OK) return error;
	
	if(token->last_tree == NULL) return ERR_CODE_SYN;
	
	inst_add_last(inst_list, INST_ID_ASSIGN, variable, variable, token->last_tree);

	// ,
	if(token->type != TK_COMMA) return ERR_CODE_SYN;

	// Podmienka pre for
	if((error = t_expression(F, token, inst_list, 0)) != ERR_CODE_OK) return error;
	
	// vytvorim strom pre podmienku pre fake-while cyklus
	TExp_tree_item *left = exp_tree_make_item(variable.key, TK_VAR, NULL);
	TExp_tree_item *right = exp_tree_make_item(NULL, TK_EXP_BLOCK, token->last_tree);
	TExp_tree *strom = exp_tree_make(TK_LOWER_EQUAL, left, right);
	
	inst_for = inst_add_last(inst_list, INST_CALL_WHILE, create_operand(*token), create_operand(*token), strom);
	
	if(token->type != TK_COMMA && token->type != TK_KEY_DO) return ERR_CODE_SYN;
		
	// ak nam prisiel aj krok
	if(token->type != TK_KEY_DO)
	{
		// krok pre for cyklus
		if((error = t_expression(F, token, inst_list, 0)) != ERR_CODE_OK) return error;
		
		TExp_tree_item *left2 = exp_tree_make_item(variable.key, TK_VAR, NULL);
		TExp_tree_item *right2 = exp_tree_make_item(NULL, TK_EXP_BLOCK, token->last_tree);
		
		strom_step = exp_tree_make(TK_PLUS, left2, right2);
		
		// do
		if(token->type != TK_KEY_DO) return ERR_CODE_SYN;
	}
	
	//nacteni noveho tokenu pro telo
	if((error = get_token(F, token)) != E_SCAN_OK) return error;
	napis_token("t_func_fordo:", *token);
	
	// body
	if ((error=t_func_body(F, token, inst_list)) != ERR_CODE_OK) return error;
	
	// pridany krok iteracie, ak bol zadany
	if(strom_step != NULL)
	{
		inst_add_last(inst_list, INST_ID_ASSIGN, variable, variable, strom_step);
	}
	
	// end
	if(token->type != TK_KEY_END) return ERR_CODE_SYN;

	if((error = get_token(F, token)) != E_SCAN_OK) return error;
	napis_token("t_func_fordo:", *token);
	
	// ;
	if(token->type != TK_SEMICOLON) return ERR_CODE_SYN;
	
	inst_end_for = inst_add_last(inst_list, INST_CALL_END_WHILE, create_operand(*token), create_operand(*token), NULL);
	
	inst_for = inst_update_instruction_jump(inst_for, inst_end_for);
	
	return ERR_CODE_OK;
}

/**
 * Gramatika pre cyklus while..do
 * @param F FILE* deskriptor na subor
 * @param token TToken* ukazatel na token
 * @param inst_list TInst_list* ukazatel na instrukcnu pasku
 * @return int chybovy stav
 */
int t_func_while(FILE *F, TToken *token, TInst_list *inst_list)
{
	//while ( <expression> ) do <body> end ;
	int error = ERR_CODE_OK;
	TInstruction *inst_while = NULL;
	TInstruction *inst_end_while = NULL;
	
	// volani expression
	if((error = t_expression(F, token, inst_list, 0)) != ERR_CODE_OK) return error;
	
	// do
	if(token->type != TK_KEY_DO) return ERR_CODE_SYN;
	
	if(token->last_tree == NULL) return ERR_CODE_SYN;
	
	inst_while = inst_add_last(inst_list, INST_CALL_WHILE, create_operand(*token), create_operand(*token), token->last_tree);
	
	//nacteni noveho tokenu pro telo
	if((error = get_token(F, token)) != E_SCAN_OK) return error;
	napis_token("func_while:", *token);
	// body
	if ((error=t_func_body(F, token, inst_list)) != ERR_CODE_OK) return error;
	
	// end
	if(token->type != TK_KEY_END) return ERR_CODE_SYN;

	if((error = get_token(F, token)) != E_SCAN_OK) return error;
	napis_token("func_while:", *token);
	
	// ;
	if(token->type != TK_SEMICOLON) return ERR_CODE_SYN;
	
	inst_end_while = inst_add_last(inst_list, INST_CALL_END_WHILE, create_operand(*token), create_operand(*token), NULL);
	
	inst_while = inst_update_instruction_jump(inst_while, inst_end_while);
	
	return ERR_CODE_OK;
}

/**
 * Gramatika pre argumenty volanej funkcie (od 2. argumentu)
 * @param F FILE* deskriptor na subor
 * @param token TToken* ukazatel na token
 * @param inst_list TInst_list* ukazatel na instrukcnu pasku
 * @param num_of_arg int pocet argumentov funkcie
 * @return int chybovy stav
 */
int t_cfunc_param_n(FILE *F, TToken *token, TInst_list *inst_list, int num_of_arg)
{
	// <cf_param_n>         , id <cf_param_n>
	// <cf_param_n>         eps
	
	int error = ERR_CODE_OK;
	
	if((error = get_token(F, token)) != E_SCAN_OK) return error;
	napis_token("call_param_n:", *token);
	
	
	if(num_of_arg == 0)
	{
		// zahadzujeme zvysok
		if((error = trash_func_param(F, token, 0)) != ERR_CODE_OK) return error;
		return ERR_CODE_OK;
	}
	
	if(token->type == TK_COMMA)
	{
		if((error = get_token(F, token)) != E_SCAN_OK) return error;
		napis_token("call_param_n:", *token);
		
		// VAR
		if(token->type == TK_VAR || token->type == TK_NUMBER || token->type == TK_KEY_TRUE || token->type == TK_KEY_FALSE || token->type == TK_KEY_NIL || token->type == TK_STRING)
		{
			if(token->type == TK_VAR)
			{
				// existuje?
				if(bvs_search(bin_tree_local, token->name.chars) == 0) return ERR_CODE_SEM;
			}
			
			// vytvorim strom s 1 operandom
			TExp_tree_item *left = exp_tree_make_item(token->name.chars, token->type, NULL);
			TExp_tree *strom = exp_tree_make(TK_ONE_OPERAND, left, NULL);
			
			inst_add_last(inst_list, INST_CALL_ARG, create_operand(*token), create_operand(*token), strom);
			
			// <cf_param_n>
			if(num_of_arg != -1) num_of_arg--; // ak neviem kolko bude mat argumentov (rekurzia)
			if((error = t_cfunc_param_n(F, token, inst_list, num_of_arg)) != ERR_CODE_OK) return error;
			return ERR_CODE_OK;
		}
		else
		{
			// vsetko ostatne
			return ERR_CODE_SYN;
		}
		
		if((error = t_cfunc_param_n(F, token, inst_list, --num_of_arg)) != ERR_CODE_OK) return error;
		
		return ERR_CODE_OK;	
	} 
	else 
	{	
		if(token->type != TK_RBRACKET) return ERR_CODE_SYN;
		
		// eps
		return ERR_CODE_OK;
	}
}

/**
 * Gramatika pre argumenty volanej funkcie
 * @param F FILE* deskriptor na subor
 * @param token TToken* ukazatel na token
 * @param inst_list TInst_list* ukazatel na instrukcnu pasku
 * @param num_of_arg int pocet argumentov funkcie
 * @return int chybovy stav
 */
int t_cfunc_param(FILE *F, TToken *token, TInst_list *inst_list, int num_of_arg)
{
	//<cf_param>              id <cf_param_n>
	//<cf_param>			  eps
	
	int error = ERR_CODE_OK;
	
	if((error = get_token(F, token)) != E_SCAN_OK) return error;
	napis_token("cfun_param:", *token);
	
	if(num_of_arg == 0)
	{
		// zahadzujeme zvysok
		//printf("%d", token->type);
		if((error = trash_func_param(F, token, 1)) != ERR_CODE_OK) return error;
		return ERR_CODE_OK;
	}
	
	if(token->type == TK_VAR || token->type == TK_NUMBER || token->type == TK_KEY_TRUE || token->type == TK_KEY_FALSE || token->type == TK_KEY_NIL || token->type == TK_STRING)
	{
		if(token->type == TK_VAR)
		{
			// existuje?
			if(bvs_search(bin_tree_local, token->name.chars) == 0) return ERR_CODE_SEM;
		}
		
		// vytvorim strom s 1 operandom
		TExp_tree_item *left = exp_tree_make_item(token->name.chars, token->type, NULL);
		TExp_tree *strom = exp_tree_make(TK_ONE_OPERAND, left, NULL);
		
		inst_add_last(inst_list, INST_CALL_ARG, create_operand(*token), create_operand(*token), strom);
		
		// <cf_param_n>
		if(num_of_arg != -1) num_of_arg--; // ak neviem kolko bude mat argumentov (rekurzia)
		if((error = t_cfunc_param_n(F, token, inst_list, num_of_arg)) != ERR_CODE_OK) return error;
		return ERR_CODE_OK;
	}
	else 
	{
		// eps
		return ERR_CODE_OK;
	}
}

/**
 * Skontrolovanie semantiky vyrazu
 * @param operator int operator, ktory musi byt kompatibilny s operandom
 * @param operand int operator, ktory musi byt kompatibilny s operatorom
 * @return int chybovy stav
 */
int t_exp_valid(int operator, int operand)
{
	// kdyz je to promenna, hned je to spravne, protoze nevime, co s tim
	if(operand == TK_VAR)
	{
		return ERR_CODE_OK;
	}
	
	// porovnavani: <, >, >=, <=
	if(operator >= TK_LOWER && operator <= TK_GREATER_EQUAL)
	{
		if(operand != TK_NUMBER && operand != TK_STRING && operand != TK_JOIN && !(operand >= TK_PLUS && operand <= TK_EXP)) return ERR_CODE_SEM;
	}
	
	// porovnavani: =, ~=
	if(operator == TK_EQUAL || operator == TK_NON_EQUAL)
	{
		// vse se muze porovnavat
		return ERR_CODE_OK;
	}
	
	// matematicke operace: +, -, *, /, %, ^
	if(operator >= TK_PLUS && operator <= TK_EXP)
	{
		if(operand != TK_NUMBER && !(operand >= TK_PLUS && operand <= TK_EXP)) return ERR_CODE_SEM;
	}
	
	// konkatenace: ..
	if(operator == TK_JOIN)
	{
		if(operand != TK_STRING && operand != TK_JOIN) return ERR_CODE_SEM;
	}
	
	return ERR_CODE_OK;	
}

/**
 * Skontrolovanie semantiky vyrazu pre porovnavanie velkosti stringu a cisla
 * @param operator int operator, ktory musi byt kompatibilny s operandami
 * @param operand1 int operator, ktory musi byt kompatibilny s operatorom a operandom2
 * @param operand2 int operator, ktory musi byt kompatibilny s operatorom a operandom1
 * @return int chybovy stav
 */
int t_exp_valid_num_string(int operator, int operand1, int operand2)
{
	if(
		(
			((operand1 == TK_NUMBER || (operand1 >= TK_PLUS && operand1 <= TK_EXP)) && (operand2 == TK_STRING || operand2 == TK_JOIN))
			||
			((operand2 == TK_NUMBER || (operand2 >= TK_PLUS && operand2 <= TK_EXP)) && (operand1 == TK_STRING || operand1 == TK_JOIN))
		)
		&&
		(operator == TK_LOWER || operator == TK_GREATER || operator == TK_LOWER_EQUAL || operator == TK_GREATER_EQUAL)
	) return ERR_CODE_SEM;
	
	return ERR_CODE_OK;
}

/**
 * Vybera trojicu (operand, operator, operand) zo zasobniku pre vyrazy
 * @param F FILE* deskriptor na subor
 * @param token TToken* ukazatel na token
 * @param symbol_stack TSymStack* zasobnik pre vyrazy
 * @param id int* znaci, jestli posledna vlozena hodnota bola id
 * @param top_offset int* posunutie vrcholu zasobnika
 * @param rbracket int pocet pravych zatvoriek
 * @return int chybovy stav
 */
int t_exp_pop(FILE *F, TToken *token, TSymStack *symbol_stack, int *id, int *top_offset, int rbracket)
{
	
	TSymSymbol symbol;
	symbol.terminal_symbol = TK_UNINIT; // inicializace
	symbol.ltgt = ' ';
	symbol.exp_token = NULL;
	symbol.tree = NULL;
	TExp_tree_item *left = NULL; left = left;
	TExp_tree_item *right = NULL; right = right;
	int num_of_pop = 0;
	int operator_middle = 0;
	char a = 0;
	
	while(symbol_stack->top != 1)
	{
		a = precedent_table[symbol_stack->data[symbol_stack->top-*top_offset].terminal_symbol][token->type];
		// kdyz jsme mimo tabulku
		if(token->type >= MAX_PT) a = '>';
		//printf("priorita %c indexy %d %d\n", a, symbol_stack->data[symbol_stack->top-*top_offset].terminal_symbol, token->type);
		
		if(a == '<') break;
		
		if(a == '=')
		{
			symbol_stack->data[symbol_stack->top-1] = symbol_stack->data[symbol_stack->top];
			symbol_stack->top--;
			*top_offset = 1;
			*id = 1;

			break;
		}
		
		for(int i = 0; i < 3; i++)
		{
			//printf("pop %d %s\n", symbol_stack->data[symbol_stack->top].terminal_symbol, symbol_stack->data[symbol_stack->top].exp_token->name.chars);
			
			if((symbol_stack->data[symbol_stack->top].terminal_symbol == TK_RBRACKET) || ((symbol_stack->data[symbol_stack->top].terminal_symbol == TK_LBRACKET) && (rbracket != 0)))
			{
				// neukladat do stromu
				i--;
			}		
					
			sym_stack_pop(symbol_stack, &symbol);
						
			if(symbol_stack->data[symbol_stack->top+1].terminal_symbol == TK_RBRACKET) 
			{
				rbracket--;
			}
			
			if((symbol_stack->data[symbol_stack->top].terminal_symbol == TK_LBRACKET) && (rbracket == 0) && (symbol.terminal_symbol == TK_RBRACKET))
			{
				// uz inkrementovano, ale popneme jeste jednou, aby nam nezustala zbyvajici (
				i--;
			}
			
			// ukladanie do stromu
			if(symbol_stack->data[symbol_stack->top+1].terminal_symbol != TK_LBRACKET && symbol_stack->data[symbol_stack->top+1].terminal_symbol != TK_RBRACKET)
			{
				if(num_of_pop == 0)
				{
					// vpravo
					if(symbol_stack->data[symbol_stack->top+1].terminal_symbol == TK_EXP_BLOCK)
					{
						right = exp_tree_make_item(NULL, 0, symbol_stack->data[symbol_stack->top+1].tree);
						//printf("vytvarim pravy substrom\n");	
					}
					else
					{
						right = exp_tree_make_item(symbol_stack->data[symbol_stack->top+1].exp_token->name.chars, symbol_stack->data[symbol_stack->top+1].exp_token->type, NULL);
						//printf("strom_prava: %s\n", symbol_stack->data[symbol_stack->top+1].exp_token->name.chars);
					}
					num_of_pop = 1;
				}
				else if(num_of_pop == 1)
				{
					// stred - operator
					operator_middle = symbol_stack->data[symbol_stack->top+1].terminal_symbol;
					num_of_pop = 2;
					//printf("strom_stred: %s\n", symbol_stack->data[symbol_stack->top+1].exp_token->name.chars);
				}
				else if(num_of_pop == 2)
				{
					// vlavo
					if(symbol_stack->data[symbol_stack->top+1].terminal_symbol == TK_EXP_BLOCK)
					{
						left = exp_tree_make_item(NULL, 0, symbol_stack->data[symbol_stack->top+1].tree);
						//printf("vytvarim levy substrom\n");
					}
					else
					{
						left = exp_tree_make_item(symbol_stack->data[symbol_stack->top+1].exp_token->name.chars, symbol_stack->data[symbol_stack->top+1].exp_token->type, NULL);
						//printf("strom_leva: %s\n", symbol_stack->data[symbol_stack->top+1].exp_token->name.chars);
					}
					num_of_pop = 0;
					
				}
			}
			
			if((symbol_stack->top == 1) && (*id == 1)) break;
			
			if((symbol_stack->top == 1) && (i < 2)) return ERR_CODE_SYN;	
		}
		
		// pokud je na zasobniku uz jen mezivysledek, nebudeme ho znovu ukladat do stromu (uz tam je)
		if((num_of_pop == 1) && (symbol_stack->data[symbol_stack->top+1].terminal_symbol == TK_EXP_BLOCK)) break;
		
		// vytvorime stromcek
		if(num_of_pop == 1)
		{ 
			// kdyz je num_of_pop == 1 - mame pouze 1 prvok, zmenime trochu strom
			token->last_tree = exp_tree_make(TK_ONE_OPERAND, right, NULL);
		}
		else
		{
			// overime semantiku
			int sem_err = ERR_CODE_OK;
			int sem_operand1 = 0;
			int sem_operand2 = 0;
			
				if(left == NULL || right == NULL) return ERR_CODE_SYN;
				
				// leva vetev
				if(left->child == NULL)
				{
					// primy operand
					if(left->exp_token == NULL) return ERR_CODE_IN_INTER;
					if((sem_err = t_exp_valid(operator_middle, left->exp_token->type)) != ERR_CODE_OK) return sem_err;
					sem_operand1 = left->exp_token->type;
				}
				else
				{
					// substrom
					if((sem_err = t_exp_valid(operator_middle, left->child->operator)) != ERR_CODE_OK) return sem_err;
					sem_operand1 = left->child->operator;
				}
				
				// prava vetev
				if(right->child == NULL)
				{
					// primy operand
					if(right->exp_token == NULL) return ERR_CODE_IN_INTER;
					if((sem_err = t_exp_valid(operator_middle, right->exp_token->type)) != ERR_CODE_OK) return sem_err;
					sem_operand2 = right->exp_token->type;
				}
				else
				{
					// substrom
					if((sem_err = t_exp_valid(operator_middle, right->child->operator)) != ERR_CODE_OK) return sem_err;
					sem_operand2 = right->child->operator;
				}
				
				// overenie semantiky porovnavania velkosti stringu a cisla
				if((sem_err = t_exp_valid_num_string(operator_middle, sem_operand1, sem_operand2)) != ERR_CODE_OK) return sem_err;
			
			// tady je vsetko korektne
			token->last_tree = exp_tree_make(operator_middle, left, right);
		}
		
		if((symbol_stack->top == 1) && (symbol.terminal_symbol != TK_RBRACKET)) 
		{
			symbol.terminal_symbol = TK_EXP_BLOCK;
			*id = 1;
			symbol.exp_token = token;
			symbol.tree = token->last_tree;
			sym_stack_push(symbol_stack, symbol);
			*top_offset = 1;
			break;
		}
			symbol.terminal_symbol = TK_EXP_BLOCK;
			*id = 1;
			symbol.exp_token = token;
			symbol.tree = token->last_tree;
			sym_stack_push(symbol_stack, symbol);
			*top_offset = 1;
	}
	return ERR_CODE_OK;
}

/**
 * Precedencni analyza pre vyrazy
 * @param F FILE* deskriptor na subor
 * @param token TToken* ukazatel na token
 * @param inst_list TInst_list* ukazatel na instrukcnu pasku
 * @param first_token int znaci, jestli sa ma nacitavat prvy token alebo nie
 * @return int chybovy stav
 */
int t_expression(FILE *F, TToken *token, TInst_list *inst_list, int first_token)
{
	TSymStack *symbol_stack = NULL;
	symbol_stack = sym_stack_init(symbol_stack);	
	
	// implicitne nastavime vystup na prazdny strom
	token->last_tree = NULL;
	
	// dolar
	TSymSymbol symbol;
	symbol.terminal_symbol = TK_DOLLAR;
	symbol.ltgt = ' ';
	symbol.exp_token = NULL;
	symbol.tree = NULL;
	
	sym_stack_push(symbol_stack, symbol);
	
	int id = 0;		// je id?
	int lbracket = 0;
	int rbracket = 0;
	int error = ERR_CODE_OK;
	int end = 0;
	int last_rbracket = 0;
	int last_lbracket = 0;
	int top_offset = 0;
	
	int num_of_tokens = 0;
	
	do
	{
		// kdyz uz prvy token mame, musime osetrit
		if(first_token == 0)
		{
			if((error = get_token(F, token)) != E_SCAN_OK) 
			{
				sym_stack_free(symbol_stack);
				return error;
			}
		}
		first_token = 0;
		
		num_of_tokens++;
		napis_token("t_expression:", *token);
		
		// kdyz prijde token " , nic se nestane
		if(token->type == TK_QUOTE) continue;
		
		else if(token->type == TK_RBRACKET)
		{ 
			if(lbracket == 0)
			{ 
				if((error = t_exp_pop(F, token, symbol_stack, &id, &top_offset, rbracket)) != ERR_CODE_OK) 
				{
					sym_stack_free(symbol_stack);
					return error;
				}
				sym_stack_free(symbol_stack);
				return ERR_CODE_OK;
			}
			else if((id != 1) && (last_rbracket == 0))
			{
				sym_stack_free(symbol_stack);
				return ERR_CODE_SYN;
			}
			else
			{
				rbracket++;
				lbracket--;
				last_rbracket = 1;
				symbol.terminal_symbol = token->type;
				symbol.exp_token = token;

				id = 0;

				if((error = t_exp_pop(F, token, symbol_stack, &id, &top_offset, rbracket)) != ERR_CODE_OK) 
					{
						sym_stack_free(symbol_stack);
						return error;
					}
			}
		}
		
		else if(token->type == TK_LBRACKET)
		{
			if(id != 0)
			{ 
				sym_stack_free(symbol_stack);
				return ERR_CODE_SYN;
			}
			else
			{
				lbracket++;
				last_lbracket = 1;
				symbol.terminal_symbol = token->type;
				symbol.exp_token = token;
				sym_stack_push(symbol_stack, symbol);
				id = 0;
			}
		}
		
		else if(token->type == TK_VAR || token->type == TK_NUMBER || token->type == TK_STRING || token->type == TK_KEY_NIL || token->type == TK_KEY_TRUE || token->type == TK_KEY_FALSE)
		{
			if(token->type == TK_VAR)
			{
				if(bvs_search(bin_tree_local, token->name.chars) == 0) 
				{
					sym_stack_free(symbol_stack);
					return ERR_CODE_SEM;
				}
			}
			if((last_rbracket == 1) || (id == 1))
			{
				sym_stack_free(symbol_stack);
				return ERR_CODE_SYN;
			}
			else
			{	
				symbol.terminal_symbol = token->type;
				symbol.ltgt = ' ';
				symbol.exp_token = token;
				sym_stack_push(symbol_stack, symbol);
				
				id = 1;
				top_offset = 1;
				
				last_rbracket = 0;
				last_lbracket = 0;
			}
		}
		// prijde token, ktery neni ve vyrazech -> ukonci se t_expression
		else if(token->type >= MAX_PT)
		{ 
			if(lbracket != 0) 
			{
				sym_stack_free(symbol_stack);
				return ERR_CODE_SYN;
			}
			if((error = t_exp_pop(F, token, symbol_stack, &id, &top_offset, rbracket)) != ERR_CODE_OK) 
			{
				sym_stack_free(symbol_stack);
				return error;
			}
			sym_stack_free(symbol_stack);
			return ERR_CODE_OK;
		}
		
		else
		{
			if((last_lbracket == 1) || (last_rbracket == 0 && id == 0))
			{
				sym_stack_free(symbol_stack);
				return ERR_CODE_SYN;
			}
			else
			{
				
				if(last_lbracket == 1)
				{
					top_offset = 0;
				}
				else
				{
					top_offset = 1;
				}
				switch(precedent_table[symbol_stack->data[symbol_stack->top-top_offset].terminal_symbol][token->type])
				{			
					case '=':
						//printf("\ncase =\n");
						symbol.terminal_symbol = token->type;
						symbol.exp_token = token;
						sym_stack_push(symbol_stack, symbol);
						
						if((error = get_token(F, token)) != E_SCAN_OK) 
						{
							sym_stack_free(symbol_stack);
							return error;
						}
						napis_token("t_exp[=]:", *token);
						
					break;
					
					case '<':
						//printf("\ncase <\n");
						if(id == 1)
							symbol_stack->data[symbol_stack->top-1].ltgt = '<';
						else
							symbol_stack->data[symbol_stack->top].ltgt = '<';
							
						symbol.terminal_symbol = token->type;
						symbol.exp_token = token;
						sym_stack_push(symbol_stack, symbol);
						
					break;
					
					case '>':
						//printf("\ncase >\n");
						if((error = t_exp_pop(F, token, symbol_stack, &id, &top_offset, rbracket)) != ERR_CODE_OK) 
						{
							sym_stack_free(symbol_stack);
							return error;
						}
						
						// aktualni token
						symbol.terminal_symbol = token->type;
						symbol.exp_token = token;
						sym_stack_push(symbol_stack, symbol);
						
					break;
					
					default: sym_stack_free(symbol_stack); return ERR_CODE_SEM;
				}
				
				id = 0;	
				last_lbracket = 0;
				last_rbracket = 0;
			}
		}
		
	} while(!end);
	
	sym_stack_free(symbol_stack);
	return ERR_CODE_OK;
}

/**
 * Zahadzuje nadbytocne argumenty funkcie
 * @param F FILE* deskriptor na subor
 * @param token TToken* ukazatel na token
 * @param start_without_comma int znaci, jestli mame na zaciatku porovnavat aj ciarku
 * @return int chybovy stav
 */
int trash_func_param(FILE *F, TToken *token, int start_without_comma)
{
	int error = ERR_CODE_OK;
	
	while(token->type != TK_RBRACKET)
	{
		if(start_without_comma == 0)
		{
			// ciarka
			if(token->type != TK_COMMA) return ERR_CODE_SYN;
			
			// hocico ine ..
			if((error = get_token(F, token)) != E_SCAN_OK) return error;
			napis_token("trash_param:", *token);
		}
		start_without_comma = 0;
		
		// overime, ci existuje premenna
		if(token->type == TK_VAR)
		{
			if(bvs_search(bin_tree_local, token->name.chars) == 0) return ERR_CODE_SEM;
		}
		
		// .. teda konkretne toto
		if(!(token->type == TK_VAR || token->type == TK_NUMBER || token->type == TK_KEY_TRUE || token->type == TK_KEY_FALSE || token->type == TK_KEY_NIL || token->type == TK_STRING))
		{
			return ERR_CODE_SYN;
		}
		
		// mozno zatvorka, inak ciarka
		if((error = get_token(F, token)) != E_SCAN_OK) return error;
		napis_token("trash_param:", *token);
		
	}

	return ERR_CODE_OK;
}

/**
 * Gramatika pre argumenty funkcie pri deklaracii (2.argument a vyssie)
 * @param F FILE* deskriptor na subor
 * @param token TToken* ukazatel na token
 * @param inst_list TInst_list* ukazatel na instrukcnu pasku
 * @param function_name char* nazov funkcie
 * @return int chybovy stav
 */
int t_func_param_n(FILE *F, TToken *token, TInst_list *inst_list, char *function_name)
{
	// <f_param_n>         , id <f_param_n>
	// <f_param_n>         eps
	
	int error = ERR_CODE_OK;
	
	if((error = get_token(F, token)) != E_SCAN_OK) return error;
	napis_token("t_param_n:", *token);
	
	if(token->type == TK_COMMA)
	{
		if((error = get_token(F, token)) != E_SCAN_OK) return error;
		napis_token("t_param_n:", *token);
		
		// existuje?
		if(bvs_search(bin_tree_local, token->name.chars) == 1) return ERR_CODE_SEM;
		
		function_num_of_var++;
		bin_tree_local = bvs_make_tree(bin_tree_local, token->name.chars, 0);
		
		inst_add_last(inst_list, INST_ARG_VAR, create_operand(*token), create_operand(*token), NULL);
		
		// inkrementacia pocitadla argumentov
		bin_tree = bvs_increment_function_arg(bin_tree, function_name);
		
		if(token->type != TK_VAR) return ERR_CODE_SYN;
		
		if((error = t_func_param_n(F, token, inst_list, function_name)) != ERR_CODE_OK) return error;
		
		return ERR_CODE_OK;	
	}
	else 
	{	
		if(token->type != TK_RBRACKET) return ERR_CODE_SYN;
		
		// eps
		return ERR_CODE_OK;	
	}
}

/**
 * Gramatika pre argumenty funkcie pri deklaracii
 * @param F FILE* deskriptor na subor
 * @param token TToken* ukazatel na token
 * @param inst_list TInst_list* ukazatel na instrukcnu pasku
 * @param function_name char* nazov funkcie
 * @return int chybovy stav
 */
int t_func_param(FILE *F, TToken *token, TInst_list *inst_list, char *function_name)
{
	//<f_param>           id <f_param_n>
	//<f_param>           eps
	
	int error = ERR_CODE_OK;
	
	if((error = get_token(F, token)) != E_SCAN_OK) return error;
	napis_token("t_param:", *token);
	
	if(token->type == TK_VAR)
	{
		// existuje?
		if(bvs_search(bin_tree_local, token->name.chars) == 1) return ERR_CODE_SEM;
		
		function_num_of_var++;
		bin_tree_local = bvs_make_tree(bin_tree_local, token->name.chars, 0);
		
		inst_add_last(inst_list, INST_ARG_VAR, create_operand(*token), create_operand(*token), NULL);
		
		// inkrementacia pocitadla argumentov
		bin_tree = bvs_increment_function_arg(bin_tree, function_name);
		
		// <f_param_n>
		if((error = t_func_param_n(F, token, inst_list, function_name)) != ERR_CODE_OK) return error;
		return ERR_CODE_OK;
	} 
	else 
	{
		// eps
		return ERR_CODE_OK;
	}
}

/**
 * Gramatika pre deklarciu funkcie
 * @param F FILE* deskriptor na subor
 * @param token TToken* ukazatel na token
 * @param inst_list TInst_list* ukazatel na instrukcnu pasku
 * @return int chybovy stav
 */
int t_function(FILE *F, TToken *token, TInst_list *inst_list)
{
	// function id ( <f_param> ) local id; <body> end
	
	int error = ERR_CODE_OK;
	TInstruction *instruction_address = NULL;
	// reset poctu premennych
	function_num_of_var = 0;
	
	if((error = get_token(F, token)) != E_SCAN_OK) return error;
	napis_token("t_function:", *token);
	
	// id
	if(token->type != TK_VAR) return ERR_CODE_SYN;
	
	int bvs_search_result = bvs_search(bin_tree, token->name.chars);
	
	// existuje?
	if(bvs_search_result == 1) return ERR_CODE_SEM;
	
	// uz bolo volanie, ale nebola deklaracia (rekurzia)
	if(bvs_search_result == 2)
	{
		// musime mu zmenit znacku deklaracie na 0 = deklarovane
		bin_tree = bvs_set_declaration(bin_tree, token->name.chars, 0);
	}
	
	// nebol uz main?
	if(bvs_search(bin_tree, "main") == 1) return ERR_CODE_SEM;
	
	bin_tree = bvs_make_tree(bin_tree, token->name.chars, 0);
	
	//inicializace lokalneho stromu
	bin_tree_local = NULL;
	if ((bin_tree_local=bvs_init_tree(bin_tree_local))==NULL) return ERR_CODE_IN_INTER;
		
	// pridanie do instrukcii
	instruction_address = inst_add_last(inst_list, INST_INIT_FUNCTION, create_operand(*token), create_operand(*token), NULL);
	
	bin_tree = bvs_update_inst_addr(bin_tree, token->name.chars, instruction_address);
	
	// ulozenie nazvu funkcie
	TSymbol function_name = create_operand(*token);
	
	if((error = get_token(F, token)) != E_SCAN_OK)
	{ 
		bvs_destroy_tree(bin_tree_local); 
		return error; 
	}
	napis_token("t_function:", *token);
	
	// (
	if(token->type != TK_LBRACKET) 
	{ 
		bvs_destroy_tree(bin_tree_local); 
		return ERR_CODE_SYN; 
	}
	
	// ak mame main, tak nemozeme mat argumenty
	if(strcmp("main", function_name.key) != 0)
	{	
		// <f_param>
		if((error = t_func_param(F, token, inst_list, function_name.key)) != ERR_CODE_OK) 
		{ 
			bvs_destroy_tree(bin_tree_local); 
			return error; 
		}
	}
	else
	{
		if((error = get_token(F, token)) != E_SCAN_OK) return error;
		napis_token("t_function:", *token);
	}
	
	// )
	if(token->type != TK_RBRACKET) return ERR_CODE_SYN;
	
	if((error = get_token(F, token)) != E_SCAN_OK) return error;
	napis_token("t_function:", *token);
	
	while(token->type==TK_KEY_LOCAL)
	{
		if((error = t_local(F,token,inst_list)) != ERR_CODE_OK) return error;
		
		if((error = get_token(F, token)) != E_SCAN_OK) 
		{
			bvs_destroy_tree(bin_tree_local); 
			return error;
		}
		napis_token("t_function:", *token);
	}
	
	// body funkcie
	if((error = t_func_body(F, token, inst_list)) != ERR_CODE_OK) 
	{
		bvs_destroy_tree(bin_tree_local); 
		return error; 
	}
	
	// k povodnej INIT instrukcii pridame pocet premmenych
	instruction_address = inst_update_var_total(instruction_address, function_num_of_var);
	
	// end
	if(token->type != TK_KEY_END) return ERR_CODE_SYN;
	
	if( inst_list->actual_instruction != NULL)
	{
		if((error = get_token(F, token)) != E_SCAN_OK) 
		{ 
			bvs_destroy_tree(bin_tree_local); 
			return error;
		}
		napis_token("t_function:", *token);
		// ;
		if(token->type != TK_SEMICOLON) 
		{ 
			bvs_destroy_tree(bin_tree_local); 
			return ERR_CODE_SYN; 
		}
	}
	
	if(bvs_search(bin_tree, "main") == 1)
	{
		inst_add_last(inst_list, INST_END_OF_MAIN, create_operand(*token), create_operand(*token), NULL);
	}
	else
	{
		// pridanie do instrukcii
		inst_add_last(inst_list, INST_END_OF_FUNCTION, create_operand(*token), create_operand(*token), NULL);
	}
	
	// uvolnenie
	bvs_destroy_tree(bin_tree_local);
	
	return ERR_CODE_OK;
}

/**
 * Overenie semantiky read()
 * @param param char* retazec, ktory je volany s funkciou read()
 * @return int chybovy stav
 */
int t_read_sem(char *param)
{
	if(param[0] != '*') return 1;
	
	if(strlen(param)<2) return 1;
	
	switch(param[1])
	{
		case 'l':
		case 'n':
		case 'a':
		break;
		default:
			return 1;
			break;
	}
	
	param[2] = '\0';
	
	return 0;
}

/**
 * Gramatika pre var = read()
 * @param F FILE* deskriptor na subor
 * @param token TToken* ukazatel na token
 * @param inst_list TInst_list* ukazatel na instrukcnu pasku
 * @return int chybovy stav
 */
int t_read(FILE *F, TToken *token, TSymbol variable, TInst_list *inst_list)
{
	// id = read ( <read_param> ) ;
	
	int error = ERR_CODE_OK;
	
	if((error = get_token(F, token)) != E_SCAN_OK) return error;
	napis_token("t_read:", *token);
	
	// (
	if(token->type != TK_LBRACKET) return ERR_CODE_SYN;
	
	// <read_param>
	if((error = get_token(F, token)) != E_SCAN_OK) return error;
	napis_token("t_read:", *token);
	
	// <read_param>
	if(token->type == TK_STRING)
	{		
		
		// overenie semantiky stringu
		if(t_read_sem(token->name.chars)==1) return ERR_CODE_SEM;
		
		// pridanie do instrukcii
		inst_add_last(inst_list, INST_READ, variable, create_operand(*token), NULL);
		
	} else
	// cislo
	if(token->type == TK_NUMBER) 
	{
		// overenie semantiky cisla
		
		// pridanie do instrukcii
		inst_add_last(inst_list, INST_READ, variable, create_operand(*token), NULL);
	}
	else if(token->type == TK_KEY_TRUE || token->type == TK_KEY_FALSE)
	{
		return ERR_CODE_SEM;
	}
	else
	{
		return ERR_CODE_SYN;
	}
	
	if((error = get_token(F, token)) != E_SCAN_OK) return error;
	napis_token("t_read:", *token);
	
	// )
	if(token->type != TK_RBRACKET) return ERR_CODE_SYN;
	
	if((error = get_token(F, token)) != E_SCAN_OK) return error;
	napis_token("t_read:", *token);
	
	// ;
	if(token->type != TK_SEMICOLON) return ERR_CODE_SYN;
	
	return ERR_CODE_OK;
}

/**
 * Gramatika pre volanie funkcie
 * @param F FILE* deskriptor na subor
 * @param token TToken* ukazatel na token
 * @param inst_list TInst_list* ukazatel na instrukcnu pasku
 * @param num_of_arg int pocet argumentov volanej funkcie
 * @return int chybovy stav
 */
int t_function_call(FILE *F, TToken *token, TInst_list *inst_list, int num_of_arg)
{
	// id = id ( <f_param> ) ;
	int error = ERR_CODE_OK;
	
	// <f_param>
	if((error = t_cfunc_param(F, token, inst_list, num_of_arg)) != ERR_CODE_OK) return error;
	
	// )
	if(token->type != TK_RBRACKET) return ERR_CODE_SYN;
	
	if((error = get_token(F, token)) != E_SCAN_OK) return error;
	napis_token("func_call:", *token);
	
	// ;
	if(token->type != TK_SEMICOLON) return ERR_CODE_SYN;
	
	return ERR_CODE_OK;
}

/**
 * Gramatika pre priradenie do premennej
 * @param F FILE* deskriptor na subor
 * @param token TToken* ukazatel na token
 * @param inst_list TInst_list* ukazatel na instrukcnu pasku
 * @return int chybovy stav
 */
int t_id_assign(FILE *F, TToken *token, TInst_list *inst_list)
{
	// id = id ( <f_param> ) ;
	// id = <expression> ;
	// id = read ( <read_param> ) ;
	
	int error;
	TInstruction *helper_inst = NULL;
	TInstruction *helper_inst2 = NULL;

	// existuje uz premenna?
	if(bvs_search(bin_tree_local, token->name.chars) == 0) return ERR_CODE_SEM;
	
	// z akej premennej prisiel?
	TSymbol variable = create_operand(*token);
	
	if((error = get_token(F, token)) != E_SCAN_OK) return error;
	napis_token("t_id_assign:", *token);

	if(token->type != TK_ASSIGN) return ERR_CODE_SYN;
	
	// co je dalsie
	if((error = get_token(F, token)) != E_SCAN_OK) return error;
	napis_token("t_id_assign:", *token);
	
	if(token->type == TK_KEY_READ)
	{
		return t_read(F, token, variable, inst_list);
	}
	else if(token->type == TK_VAR || token->type == TK_KEY_SUBSTR || token->type == TK_KEY_SORT || token->type == TK_KEY_FIND || token->type == TK_KEY_TYPE)
	{
		TSymbol name_of_function = create_operand(*token);
		
		int inter_func = 0;
		if(token->type == TK_KEY_SUBSTR || token->type == TK_KEY_SORT || token->type == TK_KEY_FIND || token->type == TK_KEY_TYPE) inter_func = 1;
		
		// funkcia alebo premenna?
		if(bvs_search(bin_tree, token->name.chars) == 1)
		{
			// je to funkcia
			if((error = get_token(F, token)) != E_SCAN_OK) return error;
			napis_token("id_assign:", *token);
			
			// '(' => volanie funkcie
			if(token->type == TK_LBRACKET)
			{
				// pridanie do instrukcii
				helper_inst = inst_add_last(inst_list, INST_CALL_FUNCTION, variable, name_of_function, NULL);
				
				// ziskame, kam sa ma skakat a prilozime to k tomu
				helper_inst2 = bvs_get_instruction_address(bin_tree, name_of_function.key);
				inst_update_instruction_jump(helper_inst, helper_inst2);				
				
				// ziskanie poctu argumentov
				int num_of_arg = bvs_get_function_arg(bin_tree, name_of_function.key);
				
				return t_function_call(F, token, inst_list, num_of_arg);
			}
			else
			{
				return ERR_CODE_SYN;
			}
		}
		else if(bvs_search(bin_tree_local, token->name.chars) == 1)
		{
			// je to premenna, volani expression
			if((error = t_expression(F, token, inst_list, 1)) != ERR_CODE_OK) return error;
			
			if(token->last_tree == NULL) return ERR_CODE_SYN;
			
			inst_add_last(inst_list, INST_ID_ASSIGN, variable, variable, token->last_tree);
			
			// ;
			if(token->type != TK_SEMICOLON) return ERR_CODE_SYN;
		}
		else
		{
			// nie je v tabulke symbolov - ani v jednej
			// jedna sa ASI o vzajomne rekurzivne volanie
			int num_of_arg = 0;
			
			// pridanie do stromu ako nedeklarovane
			bin_tree = bvs_make_tree(bin_tree, token->name.chars, 1);
			
			if((error = get_token(F, token)) != E_SCAN_OK) return error;
			napis_token("id_assign:", *token);
			
			// '(' => volanie funkcie
			if(token->type == TK_LBRACKET)
			{
				// pridanie do instrukcii
				helper_inst = inst_add_last(inst_list, INST_CALL_FUNCTION, variable, name_of_function, NULL);
				
				// neviem kolko argumentov budem potrebovat
				num_of_arg = -1;
				return t_function_call(F, token, inst_list, num_of_arg);
			}
			else
			{
				return ERR_CODE_SEM;
			}
			
			return ERR_CODE_SEM;
		}
	}
	else 
	{
		// co ine moze prist? hodime do expression
		if((error = t_expression(F, token, inst_list, 1)) != ERR_CODE_OK) return error;
		
		// prislo ale nieco?
		if(token->last_tree == NULL) return ERR_CODE_SYN;
		
		inst_add_last(inst_list, INST_ID_ASSIGN, variable, variable, token->last_tree);
		
		// ;
		if(token->type != TK_SEMICOLON) return ERR_CODE_SYN;
	}
	
	return ERR_CODE_OK;
}

/**
 * Inicializacia zasobnika pre pouzitie v IFTHEN gramatike
 * @param s TStack_if* zasobnik
 * @return TStack_if zasobnik
 */
TStack_if *if_stack_init(TStack_if *s)
{
	s = my_malloc(sizeof(TStack_if));
	
	s->data = my_malloc(sizeof(TInstruction*)*IF_STACK_ALLOC);
	s->top = -1;
	s->size_of_alloc = IF_STACK_ALLOC;
	
	return s;
}

/**
 * Vlozenie na vrchol IFTHEN zasobnika
 * @param s TStack_if* zasobnik
 * @param inst TInstruction instrukcia, ktoru chceme vlozit
 * @return int chybovy stav
 */
int if_stack_push(TStack_if *s, TInstruction *inst)
{
	s->top = s->top+1;
	
	if(s->size_of_alloc <= s->top)
	{
		
		s->data = my_realloc(s->data, sizeof(TInstruction*)*(s->size_of_alloc + IF_STACK_ALLOC));
		s->size_of_alloc = s->size_of_alloc + IF_STACK_ALLOC;
	}
	
	if(s->data != NULL)
	{	
		s->data[s->top] = inst;
	}
	else
	{
		s->top = -1;
	}
	
	return ERR_CODE_OK;
}

/**
 * Prejde cely zasobnik a kazdej instrukcii nastavy adresu pre skoky
 * @param s TStack_if* zasobnik
 * @param where_to_jump TInstruction* instrukcia, kam chceme skakat
 * @return TStack_if* zasobnik
 */
TStack_if *if_stack_set_instruction(TStack_if *s, TInstruction *where_to_jump)
{
	if(s == NULL) return NULL;
	int i = 0;
	for(i = 0; i <= s->top; i++)
	{
		s->data[i] = inst_update_instruction_jump(s->data[i], where_to_jump);
	}
	
	return s;
}

/**
 * Gramatika pre konstrukciu if..then..else..end; (aj s rozsirenim)
 * @param F FILE* deskriptor na subor
 * @param token TToken* ukazatel na token
 * @param inst_list TInst_list* ukazatel na instrukcnu pasku
 * @return int chybovy stav
 */
int t_func_if(FILE *F, TToken *token, TInst_list *inst_list)
{
	// if <expression> then <body> else <body> end ;
	// <expression> nahrazeno prazdnym prikazem (if then <body> else <body> end;)
	int error = ERR_CODE_OK;
	TInstruction *inst_else = NULL;
	TInstruction *inst_if = NULL;
	TInstruction *inst_elseif = NULL;
	TInstruction *inst_elseif_tmp = NULL;
	TInstruction *inst_end = NULL;
	TInstruction *inst_null = NULL;
	TInstruction *inst_get_out = NULL;
	TStack_if *stack_get_out = NULL;
	
	if((stack_get_out = if_stack_init(stack_get_out)) == NULL) return ERR_CODE_IN_INTER;
	if(stack_get_out == NULL) assert(0);
	// <expression>
	if((error = t_expression(F, token, inst_list, 0)) != ERR_CODE_OK) return error;
	
	if(token->last_tree == NULL) return ERR_CODE_SYN;
	
	inst_if = inst_add_last(inst_list, INST_CALL_IF, create_operand(*token), create_operand(*token), token->last_tree);
	
	// prazdny vyraz - IF THEN nepodporujeme
	if(token->last_tree == NULL) return ERR_CODE_SYN;
	
	//then
	if(token->type != TK_KEY_THEN) return ERR_CODE_SYN;
	
	if((error = get_token(F, token)) != E_SCAN_OK) return error;
	napis_token("t_func_if:", *token);
	//body
	if((error = t_func_body(F, token, inst_list)) != ERR_CODE_OK) return error;
	
	inst_get_out = inst_add_last(inst_list, INST_CALL_ELSE, create_operand(*token), create_operand(*token), NULL);
	if((error = if_stack_push(stack_get_out, inst_get_out)) != ERR_CODE_OK) return error;
	
	while(token->type == TK_KEY_ELSEIF)
	{
		// <expression>
		if((error = t_expression(F, token, inst_list, 0)) != ERR_CODE_OK) return error;
		
		if(token->last_tree == NULL) return ERR_CODE_SYN;
		
		// pridame instrukciu
		inst_null = inst_add_last(inst_list, INST_NULL, create_operand(*token), create_operand(*token), NULL);
		inst_elseif_tmp = inst_add_last(inst_list, INST_CALL_IF, create_operand(*token), create_operand(*token), token->last_tree);
		// ak bol elseif, priradime mu skok sem, inak skok priradime IFu
		if(inst_elseif != NULL)
		{
			inst_elseif = inst_update_instruction_jump(inst_elseif, inst_null);
		}
		else
		{
			inst_if = inst_update_instruction_jump(inst_if, inst_null);
		}
		inst_elseif = inst_elseif_tmp;
		// prazdny vyraz - IF THEN nepodporujeme
		if(token->last_tree == NULL) return ERR_CODE_SYN;
		//then
		if(token->type != TK_KEY_THEN) return ERR_CODE_SYN;
		
		if((error = get_token(F, token)) != E_SCAN_OK) return error;
		napis_token("t_func_if:", *token);
		//body
		if((error = t_func_body(F, token, inst_list)) != ERR_CODE_OK) return error;
		
		inst_get_out = inst_add_last(inst_list, INST_CALL_ELSE, create_operand(*token), create_operand(*token), NULL);
		if((error = if_stack_push(stack_get_out, inst_get_out)) != ERR_CODE_OK) return error;
	}
	
	// tu moze prist ELSE alebo END:
	if(token->type != TK_KEY_ELSE && token->type != TK_KEY_END) return ERR_CODE_SYN;
	
	// ak prislo END
	if(token->type == TK_KEY_END)
	{
		if((error = get_token(F, token)) != E_SCAN_OK) return error;
		napis_token("t_function:", *token);
		
		// ;
		if(token->type != TK_SEMICOLON) return ERR_CODE_SYN;
		
		// pridame do instrukcii
		inst_end = inst_add_last(inst_list, INST_CALL_END_IF, create_operand(*token), create_operand(*token), NULL);
		// ak nebolo elseif, sem ma skocit IF, inak elseif
		if(inst_elseif != NULL)
		{
			inst_elseif = inst_update_instruction_jump(inst_elseif, inst_end);
		}
		else
		{
			inst_if = inst_update_instruction_jump(inst_if, inst_end);
		}
		// vypadok - z jednej splnenej podmienky
		stack_get_out = if_stack_set_instruction(stack_get_out, inst_end);
	
		return ERR_CODE_OK;
	}
	
	inst_else = inst_add_last(inst_list, INST_CALL_ELSE, create_operand(*token), create_operand(*token), NULL);
	
	// mame else, ak bolo elseif, priradime to poslednemu, ak nie priradime ho ifu
	if(inst_elseif != NULL)
	{
		inst_elseif = inst_update_instruction_jump(inst_elseif, inst_else);
	}
	else
	{
		inst_if = inst_update_instruction_jump(inst_if, inst_else);
	}
	
	if((error = get_token(F, token)) != E_SCAN_OK) return error;
	napis_token("t_func_if:", *token);
	//body
	if((error = t_func_body(F, token, inst_list)) != ERR_CODE_OK) return error;
	
	// end
	if(token->type != TK_KEY_END) return ERR_CODE_SYN;
	
	if((error = get_token(F, token)) != E_SCAN_OK) return error;
	napis_token("t_function:", *token);
	
	// ;
	if(token->type != TK_SEMICOLON) return ERR_CODE_SYN;
	
	inst_end = inst_add_last(inst_list, INST_CALL_END_IF, create_operand(*token), create_operand(*token), NULL);
	
	inst_else = inst_update_instruction_jump(inst_else, inst_end);
	// vypadok - z jednej splnenej podmienky
	stack_get_out = if_stack_set_instruction(stack_get_out, inst_end);
	
	return ERR_CODE_OK;
}

/**
 * Zaciatok syntaktickej analyzy
 * @param F FILE* deskriptor na subor
 * @param inst_list TInst_list* ukazatel na instrukcnu pasku
 * @return int chybovy stav
 */
int start_parse(FILE *F, TInst_list *inst_list)
{
	TToken token;
	
	//inicializace stromu
	bin_tree = NULL;
	if ((bin_tree=bvs_init_tree(bin_tree))==NULL) return ERR_CODE_IN_INTER;
	
	if(init_my_string(&token.name) != 0)
	{
		return ERR_CODE_IN_INTER;
	}
	
	token.next_state = ST_START;
	token.error = 0;
	token.line = 1;
	token.last_tree = NULL;
	
	int error = 0;
	int func_error;
	function_num_of_var = 0;
	
	// Vlozenie vstavanych funkcii do BVS
	TSymbol int_symbol;
	int_symbol.token_type = TK_VAR;
	
	// substr
		int_symbol.key = "substr";
		bin_tree = bvs_make_tree(bin_tree, int_symbol.key, 0);
		bin_tree = bvs_increment_function_arg(bin_tree, int_symbol.key);
		bin_tree = bvs_increment_function_arg(bin_tree, int_symbol.key);
		bin_tree = bvs_increment_function_arg(bin_tree, int_symbol.key);
	
	// sort
		int_symbol.key = "sort";
		bin_tree = bvs_make_tree(bin_tree, int_symbol.key, 0);
		bin_tree = bvs_increment_function_arg(bin_tree, int_symbol.key);

	// type
		int_symbol.key = "type";
		bin_tree = bvs_make_tree(bin_tree, int_symbol.key, 0);
		bin_tree = bvs_increment_function_arg(bin_tree, int_symbol.key);
		
	// find
		int_symbol.key = "find";
		bin_tree = bvs_make_tree(bin_tree, int_symbol.key, 0);
		bin_tree = bvs_increment_function_arg(bin_tree, int_symbol.key);
		bin_tree = bvs_increment_function_arg(bin_tree, int_symbol.key);			
	// end vlozenie vstavanych funkcii

	while(error != E_SCAN_EOF)
	{
		token.name.length = 0;
		error = get_token(F, &token);
		
		if(error == E_SCAN_OK)
		{
			//napis_token("cyklus:", token);
			
			// function
			if(token.type == TK_KEY_FUNCTION)
			{
				if((func_error = t_function(F, &token, inst_list)) != ERR_CODE_OK) break;
			}
			else
			{
				// vsetko ostatne je v hlavnom programe nepripustne
				func_error = ERR_CODE_SYN;
				break;
			}
		}
		else if(error != E_SCAN_EOF)
		{
			// chyba z lexikalneho analyzatora
			return error;
		}
	}
	
	// prazdny program
	if(inst_list->first == NULL) return ERR_CODE_SYN;
	
	// zistime, ci tam niekde nahodou neostala nedeklarovana
	// ak ale uz bola chyba, je to jedno
	if(func_error == ERR_CODE_OK)
	{
		func_error  = bvs_is_not_declared(bin_tree);
	}
	
	// konec souboru prisel driv, nez mel
	if(func_error == E_SCAN_EOF && error != ERR_CODE_OK) func_error = ERR_CODE_SYN;
	
	// ak vsetko prebehlo zatial ok, tak zavolame interpret
	if(func_error == ERR_CODE_OK && error == E_SCAN_EOF)
	{
		error = start_interpret(inst_list, bin_tree);
		func_error = error;
	}
	/*
	else
	{
		printf("Interpret se nespustil, protoze uz nastala chyba '%d' na radku '%d'.\n", func_error, token.line);
	}
	*/
	
	inst_free(inst_list);
	bvs_destroy_tree(bin_tree);
	
	return func_error;
}
