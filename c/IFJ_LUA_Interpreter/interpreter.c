/**
 * ------------------------------------------
 * Project : Interpreter jazyka IFJ11 
 * Name    : Interpreter
 * File    : interpreter.c
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
#include <math.h>
#include <stdbool.h>
#include "types.h"
#include "my_string.h"
#include "scaner.h"
#include "interpreter.h"
#include "parser.h"
#include "ial.h"
#include "my_memory.h"


//Optimalizace volani funkce, prepis do makra
//popnuti zasobniku promennych a ulozeni promenne do symbol
#define var_stack_pop(stack, datax)  datax = stack->data[stack->top]; stack->top--;


/**
 * Ziskani datove struktury z pameti hodnot
 * @param val_mem TData* pole s daty
 * @param index int index
 * @return TData* ukazatel do pole hodnot
 */
TData* get_from_memory(TData *val_mem,int index)
{
	return &val_mem[index];
}


/**
 * Ziskani datove struktury z pameti hodnot
 * @param val_mem TData* pole s daty
 * @param index int index
 * @param data TData vkladana data
 * @return int chzbova hodnota
 */
int update_memory(TData *val_mem,int index,TData data)
{
	val_mem[index]=data;
	return ERR_CODE_OK;
}


/**
 * Inicializace pameti hodnot
 * @param val_mem TData* pole s daty
 * @param max int velikost pole
 * @return void
 */
void init_memory(TData *val_mem, int max)
{
	for(int i=0;i<max;i++)
	{
		init_exp_data(&val_mem[i]);
	}

}

/**
 * Funkce pro uvolneni pameti hodnot
 * @param val_mem TData* pole s daty
 * @param max int velikost pole
 * @return void
 */
void static destroy_memory(TData *val_mem,int max)
{
	for(int i=0;i<max;i++)
	{
		if(val_mem[i].type==STRING)
		{
			my_free(val_mem[i].val_str);
		}
	}
	my_free(val_mem);
}

/**
 * Funkce pro inicializaci datove struktury
 * @param data TData* ukazatel na datovou strukturu
 * @return void
 */
void init_exp_data(TData *data)
{
	data->val_str=NULL;
	data->id=NULL;
	data->val_num=0;
	data->id_n=0;
	data->type=NIL;
}

/**
 * Substr funkce
 * @param param1 TData* parametr 1 - string
 * @param param2 TData* parametr 2 - index 1
 * @param param3 TData* parametr 3 - index 2
 * @return TData
 */
TData substr(TData *param1,TData *param2,TData *param3)
{
	TData result;
	init_exp_data(&result);
	
	//typova kontrola parametru
	if(param1->type != STRING  || param2->type != NUMBER || param3->type != NUMBER )
	{
		result.type=NIL;
		return result;
	}
	
	char *text=param1->val_str;
	int start=param2->val_num;
	int end=param3->val_num;
	
	//jestlize je vstupni text null
	if(text == NULL)
	{
		return result;
	}
	
	// sem to ulozime
	my_string out;
	
	init_my_string(&out);
	
	int num_of_chars = strlen(text);
	
	// kombinace ktere vraceji prazdne retezce
	if(end == 0 || (start == 0 && end == 0) || num_of_chars == 0)
	{
		result.type=STRING;
		result.val_str=out.chars;
		return result;
	}
	
	// vypocet odkud pokud
	int from = 0;
	int to = 0;
	
	// odkud
	if(start < 0)
	{
		// zaporny start
		from = num_of_chars + start;
	}
	else
	{
		// kladny start
		// 1 - je zacatek, 0 je tez zacatek
		if(start == 0)
		{
			from = 0;
		}
		else
		{
			from = start - 1;
		}
		// prechod na C index od 0
		
	}
	
	// pokud
	if(end < 0)
	{
		// pokud je zaporny konec
		to = num_of_chars + end;
		
		if(to < 0)
		{
			result.type=STRING;
			result.val_str=out.chars;
			return result;
		}
	}
	else
	{
		// kladny konec
		to = end-1;
	}

	// osetreni hranic
	if(from > num_of_chars)
	{
		result.type=STRING;
		result.val_str=out.chars;
		return result;
	}
	if(to >= num_of_chars) to = num_of_chars-1;
	if(from < 0) from = 0;
	if(to < 0) to = 0;
	
	// pokud je mezi nimi nulovy a mensi rozdil vracime prazdy retezec
	if((to-from) < 0)
	{
		result.type=STRING;
		result.val_str=out.chars;
		return result;
	}
	
	// pruchid cyklem
	int i = 0;
	
	for(i = from;i<=to;i++)
	{
		add_char_to_my_string(&out, (char)text[i]);
	}

	result.type=STRING;
	result.val_str=out.chars;
	return result;
}


/**
 * Funkce pro inicializaci instrukcni pasky
 * @param inst_list TInst_list * instrukcni paska
 * @return void
 */
void inst_init_list(TInst_list *inst_list)
{
	inst_list->first = NULL;
	inst_list->last = NULL;
	inst_list->num_of_instructions = 1;
	inst_list->last_jump_instruction = NULL;
	inst_list->actual_instruction = NULL;
}


/**
 * Funkce pro inicializaci instrukcni pasky
 * @param inst_list TInst_list * instrukcni paska
 * @return void
 */
TInstruction * inst_update_var_total(TInstruction *instruction, int var_total)
{
	if(instruction == NULL) return NULL;
	
	instruction->var_total = var_total;
	
	return instruction;
}

/**
 * Uprava instrukce pro skok
 * @param instruction TInstruction* ukazatel na instrukci
 * @param instruction TInstruction* ukazatel na instrukci skoku
 * @return TInstruction* ukazatel na upravenou instrukci
 */
TInstruction *inst_update_instruction_jump(TInstruction *instruction, TInstruction *jump_instruction)
{
	if(instruction != NULL)
	{
		instruction->instruction_jump = jump_instruction;
	}
	else
	{
		instruction = NULL;
	}
	
	return instruction;
}


/**
 * Uvoleni instrukcni pasky
 * @param inst_list TInst_list* ukazatel na list instrukcni pasky
 * @return void
 */
void inst_free(TInst_list *inst_list)
{
	TInstruction *pom;
	TInstruction *pom_free;
	
	pom = inst_list->first;
	
	// cyklus bezi, pokial nie je zoznam prazdny a maze prvy prvok..
	while( pom != NULL )
	{
		if(pom->tree != NULL)
		{
			// uvolnim stromcek
		}
		
		pom_free = pom;
		pom = pom->next;
		my_free(pom_free);
	}
}

/**
 * Uprava indexu promenne 
 * @param instruction TInstruction* ukazatel na instrukci
 * @param num1 int pro 1. operand
 * @param num1 int pro 2. operand
 * @return TInstruction*
 */
 
TInstruction * inst_update_var_order(TInstruction *instruction, int num1, int num2)
{
	if(instruction == NULL) return NULL;
	
	instruction->var1_order = num1;
	instruction->var2_order = num2;
	
	return instruction;
}


/**
 * Pridani instrukce
 * @param inst_list TInst_list* ukazatel na list instrukcni pasky
 * @param operator int operator 
 * @param operand1 TSymbol operand
 * @param operand2 TSymbol operand
 * @param tree TExp_tree* vyrazovy strom
 * @return TInstruction*
 */
TInstruction * inst_add_last(TInst_list *inst_list, int operator, TSymbol operand1, TSymbol operand2, TExp_tree *tree)
{

	TInstruction *new = my_malloc(sizeof(TInstruction));
	    
	new->operator = operator;
	new->operand1 = operand1;
	new->operand2 = operand2;
	new->instruction_jump = NULL;
	new->inst_num = inst_list->num_of_instructions;
	new->next = NULL;
	
	// cislo premennej
	if(bin_tree_local != NULL)
	{
		Tbin_tree *variable1 = bvs_get_key(bin_tree_local, operand1.key);
		Tbin_tree *variable2 = bvs_get_key(bin_tree_local, operand2.key);
		if(variable1 != NULL) new->var1_order = variable1->order_of_var;
		if(variable2 != NULL) new->var2_order = variable2->order_of_var;	
	}
	
	// kopirovanie stromu
	new->tree = tree;

	// ak je prazdny zoznam
	if( inst_list->first == NULL )
	{
		inst_list->first = new;
		inst_list->last = new;
	}
	else
	{
		inst_list->last->next = new;
		inst_list->last = new;
	}
	
	// ak je to main
	if(operator == INST_INIT_FUNCTION && strcmp(operand1.key, "main") == 0)
	{
		inst_list->actual_instruction = new;
	}
	
	inst_list->num_of_instructions++;
	
	return new;
}

/**
 * Vytvoreni operandu
 * @param token TToken vytvoreni operandu
 * @return TSymbol
 */
TSymbol create_operand(TToken token)
{
	TSymbol *new_symbol = my_malloc(sizeof(TSymbol));
	
	new_symbol->key = my_malloc( sizeof(char) * strlen(token.name.chars) +1);
	strcpy( new_symbol->key, token.name.chars );
	new_symbol->token_type = token.type;
	return *new_symbol;
}

/**
 * Inicializace zasobniku s promennyma
 * @param stack TVarStack * zasobnik
 * @return int chybovy stav
 */
int var_stack_init(TVarStack *stack)
{
	stack->top = -1;
	stack->size = VAR_STACK_INIT_SIZE;
	stack->data = my_malloc(sizeof(TVarStackData)*VAR_STACK_INIT_SIZE);

	if(stack->data == NULL)
	{
		return ERR_CODE_IN_INTER;
	}
	return ERR_CODE_OK;
}

/**
 * Uvolneni zasobniku
 * @param stack TVarStack * zasobnik
 * @return void
 */
void var_stack_free(TVarStack *stack)
{
	my_free(stack->data);
	stack->top = -1;
	stack->size = 0;
}

/**
 * Pridani hodnoty na zasobnik
 * @param stack TVarStack * zasobnik
 * @param data TData * datova struktura
 * @param val_mem TVal_mem pole s datovymi polozkami
 * @param inst TInstruction * ukazetel na instrukci
 * @return int chybovy stav
 */
int var_stack_push(TVarStack *stack,TData *data,TVal_mem val_mem,TInstruction *inst)
{ 
	stack->top++;
	if(stack->size <= stack->top) 
	{  
		stack->data = my_realloc(stack->data, sizeof(TVarStackData)*(stack->size + VAR_STACK_PLUS_SIZE));
		if(stack->data == NULL )
		{
			return ERR_CODE_IN_INTER;
		}
		stack->size += VAR_STACK_PLUS_SIZE;
	}
	stack->data[stack->top].data=*data;
	stack->data[stack->top].val_mem = val_mem;
	stack->data[stack->top].instruction = inst;	
	
	return ERR_CODE_OK;
}

/**
 * Adresovani zasobniku stack.top->sub
 * @param stack TVarStack * zasobnik
 * @param data TVarStackData * polozka v zasobniku
 * @param sub int index pro odecteni od vrcholu
 * @return void
 */
void var_stack_address(TVarStack *stack, TVarStackData *data,int sub)
{
	if((stack->top-sub)>=0)
	{
		*data = stack->data[stack->top-sub];
	}	
}

/**
 * Funkce pro ziskani dalsi instrukce od aktualni
 * @param inst_list TInst_list* instrukcni paska
 * @return TInstruction* vrai ukazatel na instrukci
 */
TInstruction * get_next_instruction(TInst_list *inst_list)
{
	if(inst_list->actual_instruction != NULL)
	{
		inst_list->actual_instruction=inst_list->actual_instruction->next;
		return inst_list->actual_instruction;
	}
	return NULL; 
}

/**
 * Funkce pro vyhodnoceni ciselnzch operandu
 * @param operator int operator, dle definice v scaner.h
 * @param a int operand 1
 * @param b int operand 2
 * @param error int* pripadna chyba
 * @return vraci vysledek bool operace
 */
int evaluate_bool_operands(int operator,int a,int b,int *error)
{
	switch(operator)
	{
		case TK_EQUAL:
			if(a == b)
			{
				return 1;
			}
			else
				return 0;
		case TK_NON_EQUAL:
			if(a != b)
				return 1;
			else
				return 0;

		default:
			*error=ERR_CODE_INTER;
			return 0;
	}
}

/**
 * Funkce pro vyhodnoceni ciselnzch operandu
 * @param operator int operator, dle definice v scaner.h
 * @param a char* string 1
 * @param b char* string 2
 * @param error int* pripadna chyba
 * @return TData vraceni datove struktury
 */
TData evaluate_string_operands(int operator,char *a,char *b,int *error)
{
	TData out;
	init_exp_data(&out);
	
	if(operator == TK_JOIN)
	{
		//vytvoreni noveho retezce pro konkatenaci
		out.val_str=my_malloc(sizeof(char)*(strlen(a)+strlen(b)+1));

		if(out.val_str==NULL)
		{
			*error=ERR_CODE_IN_INTER;
			out.type=NIL;
			return out;
		}
			
		strcpy(out.val_str,"");strcat(out.val_str,a);strcat(out.val_str,b);
		
		
		out.type=STRING;
	}
	else if(operator == TK_EQUAL || operator == TK_NON_EQUAL)
	{
		if(strcmp(a,b)==0)
		{
			if(operator == TK_EQUAL)
				out.val_num=1;
			else
				out.val_num=0;
		}
		else
		{
			if(operator == TK_EQUAL)
				out.val_num=0;
			else
				out.val_num=1;
		}
		out.type=BOOL;
	}
	else if(operator == TK_LOWER || operator == TK_LOWER_EQUAL || operator == TK_GREATER || operator == TK_GREATER_EQUAL)
	{
		int diff = strcmp(a,b);
		
		switch(operator)
		{
			case TK_LOWER:
				if(diff == 0) out.val_num = 0;
				if(diff < 0) out.val_num = 1;
				if(diff > 0) out.val_num = 0;
			break;
			case TK_LOWER_EQUAL:
				if(diff == 0) out.val_num = 1;
				if(diff < 0) out.val_num = 1;
				if(diff > 0) out.val_num = 0;
			break;
			case TK_GREATER:
				if(diff == 0) out.val_num = 0;
				if(diff < 0) out.val_num = 0;
				if(diff > 0) out.val_num = 1;
			break;
			case TK_GREATER_EQUAL:
				if(diff == 0) out.val_num = 1;
				if(diff < 0) out.val_num = 0;
				if(diff > 0) out.val_num = 1;
			break;
		}
		
		out.type=BOOL;
	}
	else
	{
		*error=ERR_CODE_INTER;
		out.val_str=NULL;
	}
	//my_free(a);
		//my_free(b);
	return out;
}


/**
 * Funkce pro vyhodnoceni ciselnzch operandu
 * @param operator int operator, dle definice v scaner.h
 * @param a double operand 1
 * @param a double operand 2
 * @param error int pokud doslo pri provadeni vypoctu k chybe
 * @return double vraci vysledek operace
 */
double evaluate_operands(int operator,double a,double b,int *error)
{
	double loc_value;
	switch(operator)
	{
		case TK_PLUS:
			loc_value=a+b;
			return loc_value;
		
		case TK_MINUS:
			loc_value=a-b;
			return loc_value;
		
		case TK_MULTIPLY:
			loc_value=a*b;
			return loc_value;

		case TK_DIVIDE:
			if(b==0)
			{
				*error=ERR_CODE_INTER;
				return 0;
			}			
			loc_value=a/b;
			return loc_value;
		case TK_EXP:
			loc_value=pow (a,b);
			return loc_value;
		case TK_LOWER:
			if(a < b)
				return 1;
			else
				return 0;

		case TK_GREATER:
			if(a > b)
				return 1;
			else
				return 0;
				
		case TK_LOWER_EQUAL:
			if(a <= b)
				return 1;
			else
				return 0;
				
		case TK_MODULO:
			if(b==0)
			{
				*error=ERR_CODE_IN_INTER;
				return 0;
			}
			double res = a / b;
			loc_value = a - ((int)res*b);
			return loc_value;
		case TK_GREATER_EQUAL:
			if(a >= b)
				return 1;
			else
				return 0;
		case TK_EQUAL:
			if(a == b)
				return 1;
			else
				return 0;
		case TK_NON_EQUAL:
			if(a != b)
				return 1;
			else
				return 0;
	}

	return 0;
}

/**
 * Funkce pro cteni cisel.
 * Dev note: osetrene vracejici navratove kody, dle zakladni definice
 * @param result double* vysledek
 * @return int chybovy stav
 */
int read_number(double *result,int *EOL_flag)
{
	int index=0;
	int c=0;
	char *str;
	char *ralloc;
	bool byla_tecka=false;
	bool bylo_e=false;
	bool bylo_minus=false;
	str=my_malloc(sizeof(char)*COUNT_CHAR_READ_LINE);
	
	if(str==NULL)
		return ERR_CODE_IN_INTER;
		
	while ((c = getchar()) != '\n' && c != EOF) {
	
		//maximalne dojde ke 3 ukladani do stringu
		if(index%(COUNT_CHAR_READ_NUMBER-3)==0)
		{
			ralloc=my_realloc(str,(index+1+COUNT_CHAR_READ_NUMBER)*sizeof(char));
			if(ralloc==NULL)
			{
				my_free(str);
				return ERR_CODE_IN_INTER;
			}
			str=ralloc;
		}
		//konecny automat
		if(isdigit(c))
		{
			str[index]=(char)c;
			index++;
		}
		else if(c == 'e' || c == 'E')
		{
			if(bylo_e == true)
			{
				my_free(str);
				return -1;
			}
			bylo_e = true;
			str[index]=(char)c;
			index++;
			c = getchar();
			
			if(isdigit(c))
			{
				str[index]=(char)c;
				index++;
			}
			else if(c == '+' || c == '-')
			{
				str[index]=(char)c;
				index++;
				c = getchar();
								
				if(isdigit(c))
				{
					str[index]=(char)c;
					index++;
				}
				
				else
				{
					my_free(str);
					return -1;
				}
			}		
			else
			{
				my_free(str);
				return -1;
			}
		}
		else if(c == '-')
		{
			if(bylo_minus == true)
			{	
				my_free(str);
				return -1;
			}
			bylo_minus = true;
			str[index]=(char)c;
			index++;
		}
		else if(c == '.')
		{
			if(byla_tecka == true || bylo_e == true)
			{
				my_free(str);
				return -1;
			}
			byla_tecka = true;
			str[index]=(char)c;
			index++;
			c = getchar();
			
			if(isdigit(c))
			{
				str[index]=(char)c;
				index++;
			}
			else
			{
				my_free(str);
				return -1;
			
			}
		}
		else
		{
			ungetc(c, stdin);
			return -1;
		}
	}
	str[index]='\0';
	if((c == '\n' ||  c == EOF) && index == 0 )
		*EOL_flag=1;

	*result=atof(str);
	my_free(str);
	return 1; 
}

/**
 * Funkce pro nacitani dle volani z read.
 * Dev note: osetrene vracejici navratove kody, dle zakladni definice
 * @param string char** ukazatel na adresu, kam se bude ukladat
 * @return int chybovy stav
 */
int read_line(char **string,int *EOL_flag)
{
	int index=0;
	int c=0;
	char *str;
	char *ralloc;

	str=my_malloc(sizeof(char)*COUNT_CHAR_READ_LINE);
	
	if(str==NULL)
		return ERR_CODE_IN_INTER;
		
	while ((c = getchar()) != '\n' && c != EOF) {
		if(index%COUNT_CHAR_READ_LINE==0)
		{
			ralloc=my_realloc(str,(index+1+COUNT_CHAR_READ_LINE)*sizeof(char));
			if(ralloc==NULL)
			{
				my_free(str);
				return ERR_CODE_IN_INTER;
			}
			str=ralloc;
		}
			
		str[index]=(char)c;
		index++;
	}
	//pokud neni EOL nastavim flag
	if(c != '\n' && index == 0 )
		*EOL_flag=1;
	
	str[index]='\0';
	*string=str;
	
	return ERR_CODE_OK; 
}

/**
 * Nacita retazec zo standardneho vstupu po EOF
 * @param string char** ukazatel na adresu, kam se bude ukladat
 * @return int chybovy stav
 */
int read_to_EOF(char **string)
{
	int index=0;
	int c=0;
	char *ralloc;
	char *str=my_malloc(sizeof(char)*COUNT_CHAR_READ_EOF);

	if(str==NULL)
		return ERR_CODE_IN_INTER;
		
	while ((c = getchar()) != EOF) {
		if(index%COUNT_CHAR_READ_EOF==0)
		{
			ralloc=my_realloc(str,(index+1+COUNT_CHAR_READ_EOF)*sizeof(char));
			if(ralloc==NULL)
			{
				my_free(str);
				return ERR_CODE_IN_INTER;
			}
			str=ralloc;
		}
		str[index]=(char)c;
		index++;
	}
	str[index]='\0';
	*string=str;
	
	return ERR_CODE_OK; 
}

/**
 * Nacita presny pocet znakov retazca zo standardneho vstupu
 * @param string char** ukazatel na adresu, kam se bude ukladat
 * @param count int pocet znakov
 * @return int chybovy stav
 */
int read_count_char(char **string,int count)
{
	int index=0;
	int c=0;
	char *ralloc;
	char *str=my_malloc(sizeof(char)*COUNT_CHAR_READ_LINE);
	if(str==NULL)
		return ERR_CODE_IN_INTER;
		
	while ((c = getchar()) != EOF) {
		if(index%COUNT_CHAR_READ_LINE==0)
		{
			ralloc=my_realloc(str,(index+1+COUNT_CHAR_READ_LINE)*sizeof(char));
			if(ralloc==NULL)
			{
				my_free(str);
				return ERR_CODE_IN_INTER;
			}
			str=ralloc;
		}
			
		str[index]=(char)c;
		index++;
		
		if(index>=count)
			break;
	}
	str[index]='\0';
	*string=str;
	
	return ERR_CODE_OK; 
}

/**
 * Funkce pro uvolnovani retezce z datove struktury.
 * @param data TData* datova slozka
 * @return void
 */
void free_old_data_str(TData *data)
{
	if(data->type==STRING)
	{
		my_free(data->val_str);
		data->val_str=NULL;
	}
}

/**
 * Funkce zjistujici zda retezec cisel obsahuje desetinou cast.
 * @param sub_symbol_table Tbin_tree* tabulka symbolu, ze ktere se ziskavaji hodnoty promennych pri vyhodnocovani
 * @param load_var char* nazev promenne do ktere se bude nacitat, pokud neni je to sem. chyba
 * @param type char* typy pro nacitani dle zadani(*n,*l,*a)
 * @return int chybovy stav
 */
int read(TVal_mem val_mem,int index,char *type)
{
	TData *id=get_from_memory(val_mem,index);
	
	free_old_data_str(id);
	
	char *chars=NULL;
	double number=0;
	int scanf_ret=0;
	int EOL_flag=0;
	
	//pokud se povedlo najit promennou, do ktere se bude prirazovat
	if(id != NULL)
	{		
		if(strcmp(type,"*n")==0)
		{
			scanf_ret=read_number(&number,&EOL_flag); 
			
			if(scanf_ret != 1 || EOL_flag == 1)
			{
				id->type=NIL;
				return ERR_CODE_OK;
			}
			
			id->type=NUMBER;
			id->val_num=number;
			
			return ERR_CODE_OK;
		}
		else if(strcmp(type,"*l")==0)
		{
			read_line(&chars,&EOL_flag);
			id->val_str=chars;
			
			//Pokud neni EOL a prvni znak je ukoncovaci, tak vysledek bude NIL
			if(id->val_str[0]=='\0' && EOL_flag == 1)
			{
				my_free(id->val_str);
				id->type=NIL;
			}
			else
			{
				id->type=STRING;
			}
			
			
			return ERR_CODE_OK;
		}
		else if(strcmp(type,"*a")==0)
		{		
			read_to_EOF(&chars);	
			id->val_str=chars;
			
	
				id->type=STRING;
		
			
			return ERR_CODE_OK;
		}
		else
		{
			//Bude se jednat o par, poctu nactenych znaku
			number=atoi(type);	
			read_count_char(&chars, number);
			id->val_str=chars;
			if(id->val_str[0]=='\0')
			{
				my_free(id->val_str);
				id->type=NIL;
			}
			else
				id->type=STRING;
		}
	}
	//pokud byl v teto instrukci retezec uvolni se a vlozi se nove mallocovany
	return ERR_CODE_OK;

}

/**
 * Funkce vyhodnocujici stranu stromu.
 * @param tree TExp_tree* vyrazovy strom
 * @param leaf TExp_tree_item* aktualni uzel
 * @param sub_symbol_table Tbin_tree* tabulka symbolu, ze ktere se ziskavaji hodnoty promennych pri vyhodnocovani
 * @return TExpResult vraci vyhodnoceny vysledek strany stromu.
 */
TExpResult evaluate_exp_leaf(TExp_tree* tree,TExp_tree_item* leaf,TVal_mem val_mem)
{
	TExpResult result;
	init_exp_data(&result.data);
	result.error=ERR_CODE_OK;
	
	TData* mem_tmp=NULL;
	
	if(leaf->child != NULL)
	{
		result=evaluate_exp_tree(leaf->child, val_mem);
	}
	else if(leaf->data.type != NIL)
	{	

		if(leaf->data.type==VAR)
		{		
			mem_tmp=get_from_memory(val_mem,leaf->data.id_n);	
			result.data=*mem_tmp;
		}
		else
			result.data=leaf->data;
	}
	return result;
}

/**
 * Funkce vyhodnocujici vyrazovy strom.
 * @param tree TExp_tree* vyrazovy strom
 * @param sub_symbol_table Tbin_tree* tabulka symbolu, ze ktere se ziskavaji hodnoty promennych pri vyhodnocovani
 * @return TExpResult vysledek vyhodnoceni
 */
TExpResult evaluate_exp_tree(TExp_tree* tree,TVal_mem val_mem)
{	
	//pro binarni operatory je zapotrebi dvou strukru pro vysledky
	TExpResult operand1;
	TExpResult operand2;
	
	//vysledek 
	TExpResult result;
	init_exp_data(&result.data);
	result.error=ERR_CODE_OK;
	
	int error=0;
	
	//kdyz bude jeden operand
	if(tree->operator == TK_ONE_OPERAND)
	{	
		return evaluate_exp_leaf(tree,tree->left,val_mem);
	}
	
	
	//ziskani opernadu
	operand1=evaluate_exp_leaf(tree,tree->left,val_mem);
	operand2=evaluate_exp_leaf(tree,tree->right,val_mem);
	
	//typove kontroly pro vystup ze stromecku
	if(operand1.data.type == NUMBER && operand2.data.type == NUMBER)
	{
		result.data.val_num=evaluate_operands(tree->operator,operand1.data.val_num,operand2.data.val_num,&error);
		if(isinf(result.data.val_num) || isnan(result.data.val_num))
			error=ERR_CODE_INTER;
			
			result.data.type=NUMBER;
	}
	else if(operand1.data.type == STRING && operand2.data.type == STRING)
	{
		result.data=evaluate_string_operands(tree->operator,operand1.data.val_str,operand2.data.val_str,&error);
	}
	else if(operand1.data.type == BOOL && operand2.data.type == BOOL)
	{
		result.data.type=BOOL;
		result.data.val_num=evaluate_bool_operands(tree->operator,operand1.data.val_num,operand2.data.val_num,&error);
	}
	else if(operand1.data.type == NIL && operand2.data.type == NIL)
	{
		if(tree->operator==TK_EQUAL)
		{
			result.data.type=BOOL;
			result.data.val_num=1;
		}
		else
		{
			result.data.type=NIL;
			result.error=ERR_CODE_INTER;
			error=ERR_CODE_INTER;
		}
	}
	else
	{
		result.data.type=NIL;
		result.error=ERR_CODE_INTER;
		error=ERR_CODE_INTER;
	}
	
	
	// Pokial je tam == a nesedia datove typy, vysledok je BOOL a false
	// Pokial je tam ~= a nesedia datove typy, je to true
	if(tree->operator == TK_EQUAL || tree->operator == TK_NON_EQUAL)
	{
		result.data.type=BOOL;
		error = ERR_CODE_OK;
	}
	if(tree->operator == TK_EQUAL && (operand1.data.type != operand2.data.type))
	{
			result.data.type=BOOL;
			result.data.val_num = 0;
	}
	if(tree->operator == TK_NON_EQUAL && (operand1.data.type != operand2.data.type))
	{
			result.data.type=BOOL;
			result.data.val_num = 1;
	}
	
	// Pokial je tam operator mensi, vacsi atd. vzdy je vysledok BOOL!
	if(
		tree->operator == TK_EQUAL ||
		tree->operator == TK_NON_EQUAL ||
		tree->operator == TK_LOWER ||
		tree->operator == TK_LOWER_EQUAL ||
		tree->operator == TK_GREATER ||
		tree->operator == TK_GREATER_EQUAL
	)
	{
		result.data.type = BOOL;
	}
	
	if(error !=ERR_CODE_OK)
	{
		result.error=error;
	}
	else
		result.error=ERR_CODE_OK;
	
	return result;
}

/**
 * Funkce pro vypis budto prommene nebo primo vyrazu.
 * @param tree TExp_tree* vyrazovy strom
 * @param sub_symbol_table Tbin_tree* tabulka symbolu, ze ktere se ziskavaji hodnoty promennych pri vyhodnocovani
 * @return TExpResult vysledek vyhodnoceni
 */
int write(TExp_tree* tree,TVal_mem val_mem)
{
	TExpResult result=evaluate_exp_tree(tree,val_mem);
	
	if(result.error != ERR_CODE_OK)
		return result.error;
		
	if(result.data.type == STRING)
	{
		printf("%s",result.data.val_str);	
		return ERR_CODE_OK;
	}
	if(result.data.type == NUMBER)
	{
		printf("%g",result.data.val_num);
		return ERR_CODE_OK;
	}
	if(result.data.type == BOOL)
		return ERR_CODE_INTER;

	if(result.data.type == NIL)
		return ERR_CODE_INTER;

	return ERR_CODE_OK;
}

/**
 * Uvolnovani dat ze zasobniku.
 * @param data TVarStackData* ukazatel na data zasobniku
 * @return int vraceni chybove hodnoty
 */
int	stack_data_free(TVarStackData *sdata)
{
	if(sdata->data.type==STRING)
		my_free(sdata->data.val_str);
		
	return ERR_CODE_OK;
}

/**
 * Prirazeni hodnoty do promenne.
 * @param instruction TInstruction* ukazatel na isntrukci
 * @param sub_symbol_table Tbin_tree* tabulka symbolu, pro nalezeni promenn a ulozeni do ni hodnoty
 * @return int vraceni chybove hodnoty
 */
int f_inst_id_assign(TInstruction *instruction,TVal_mem val_mem)
{
	TData *id=get_from_memory(val_mem,instruction->var1_order);
	//vyhodnoceny  vyraz
	
	TExpResult result=evaluate_exp_tree(instruction->tree,val_mem);
	
	//Pokud doslo k nejake chyb pri vyhodnocovani
	if(result.error != ERR_CODE_OK)
		return result.error;
		
	free_old_data_str(id);
	
	if(result.data.val_str != NULL && result.data.type==STRING)
	{
		id->val_str=my_malloc(sizeof(char)*(strlen(result.data.val_str)+1));
		if(id->val_str == NULL )
		{
			return ERR_CODE_IN_INTER;
		}
		strcpy(id->val_str,result.data.val_str);
	}
	
	id->type=result.data.type;
	id->val_num=result.data.val_num;
	
	return ERR_CODE_OK;
}

/**
 * Funkce pro cyklus while.
 * @param inst_list TInst_list* ukazatel na seznam instrukci
 * @param instruction TInstruction* ukazatel na isntrukci
 * @param val_mem TVal_mem pole promennych s hodnotami (pouziti pri vyhodnocovani podminky)
 * @return int vraceni chybove hodnoty
 */
int f_inst_call_while(TInst_list *inst_list,TInstruction *instruction,TVal_mem val_mem)
{
	TInstruction *instruction_tmp=instruction->instruction_jump;
	TExpResult result=evaluate_exp_tree(instruction->tree,val_mem);
	
	//pokud doslo k nejake chybe pri vyhodnoceni vyrazu dojde k vraceni chyby
	if(result.error != ERR_CODE_OK)
		return result.error;

	//ulozeni do instrukce WHILE_END zacatek cyklu
	instruction_tmp->instruction_jump=instruction;
	if((result.data.val_num>0 && (result.data.type == NUMBER || result.data.type == BOOL)))
		;
	else
	{
		instruction=instruction->instruction_jump;
		inst_list->actual_instruction=instruction;
	}
	
	return ERR_CODE_OK;
}

/**
 * Funkce pro cyklus while.
 * @param inst_list TInst_list* ukazatel na seznam instrukci
 * @param instruction TInstruction* ukazatel na isntrukci
 * @param val_mem TVal_mem pole promennych s hodnotami (pouziti pri vyhodnocovani podminky)
 * @return int vraceni chybove hodnoty
 */
int f_inst_call_if(TInst_list *inst_list,TInstruction *instruction,TVal_mem val_mem)
{
	TExpResult result=evaluate_exp_tree(instruction->tree,val_mem);
	
	if(result.error != ERR_CODE_OK)
		return result.error;

	//if sa neprevedie, iba ak je tam nil alebo false
	if((result.data.val_num == 0 && result.data.type == BOOL) || result.data.type == NIL)
	{
		instruction=instruction->instruction_jump;
		inst_list->actual_instruction=instruction;
	}
	
	return ERR_CODE_OK;
}

/**
 * Inicializace funkce
 * @param val_mem TVal_mem* pro mallocovani pole s hodnotami
 * @param act_arg int* pocet argumentu pro vynulovani (aby se mohla volat dalsi funkce)
 * @param var_count int pocet promennych co budou v tabulce hodnot
 * @return int vraceni chybove hodnoty
 */
int f_inst_init_function(TVal_mem *val_mem,int *act_arg,int var_count)
{
	TVal_mem val_mem_= my_malloc(sizeof(TData)*var_count);
	
	if(val_mem_==NULL)
		return ERR_CODE_IN_INTER;
		
	init_memory(val_mem_, var_count);
	*act_arg=0;
	
	*val_mem=val_mem_;
	
	return ERR_CODE_OK;
}				

/**
 * Pridani argumentu na zasobnik
 * @param instruction TInstruction* ukazatel na isntrukci
 * @param val_mem TVal_mem pole hodnot
 * @param var_stack TVarStack * zasobnik
 * @param arg_count int * pocet volanych promennych
 * @return int vraceni chybove hodnoty
 */
int f_inst_call_arg(TInstruction *instruction,TVal_mem val_mem,TVarStack *var_stack,int *arg_count)
{
	TExpResult result=evaluate_exp_tree(instruction->tree,val_mem);
	var_stack_push(var_stack,&result.data,NULL,NULL);
	*arg_count=*arg_count+1;
	
	return ERR_CODE_OK;
}

/**
 * Vyzvednuti argumentu ze zasobniku
 * @param index int index k pameti hodnot, aby se mohl argument ulozit na spravne misto
 * @param val_mem TVal_mem pole hodnot
 * @param val_mem TVal_mem pole hodnot
 * @param var_stack TVarStack * zasobnik
 * @param act_arg int * index ktery rika kolik argumentu uz bylo ulozeno do pole hodnot
 * @return int vraceni chybove hodnoty
 */
int f_inst_arg_var(int index,TVal_mem val_mem,TVarStack *var_stack,int *act_arg)
{
	TVarStackData data_stack;
	init_exp_data(&data_stack.data);
	TData data;
	init_exp_data(&data);
	
	var_stack_address(var_stack,&data_stack,1);
	
	int arg_count_for_tree=data_stack.data.val_num;
	//nullovani prebyvajicich parametru
	if(*act_arg>=arg_count_for_tree)
		data.type=NIL;
	else
	{
		var_stack_address(var_stack,&data_stack,arg_count_for_tree+1-(*act_arg));
		data=data_stack.data;
		if(data.type==STRING)
		{
			data.val_str=my_malloc(sizeof(char)*(strlen(data_stack.data.val_str)+1));
			if(data.val_str==NULL)
				return ERR_CODE_IN_INTER;
			strcpy(data.val_str,data_stack.data.val_str);
			//my_free(data_stack.data.val_str);
		}
	}
	update_memory(val_mem,index,data);
	*act_arg=*act_arg+1;
	
	return ERR_CODE_OK;
}

/**
 * Vyzvednuti argumentu ze zasobniku
 * @param instruction TInstruction * instrukce
 * @param call_instruction TInstruction * ukazatel na volanou funkci
 * @param bin_tree Tbin_tree * hlavni tabulka symbolu, ve ktere jsou zaznaceny vsechny funkce
 * @param val_mem TVal_mem pole hodnot
 * @param var_stack TVarStack * zasobnik
 * @param act_arg int * index ktery rika kolik argumentu uz bylo ulozeno do pole hodnot
 * @return int vraceni chybove hodnoty
 */
int f_inst_call_function(TInstruction *instruction,TInstruction **call_instruction,Tbin_tree *bin_tree,TVal_mem val_mem,TVarStack *var_stack)
{
	Tbin_tree *bin_tree_tmp=bvs_get_key(bin_tree,instruction->operand2.key);
	//dojde k volani funkce, jen je treba jeste pushovat argumenty
	
	TData empty;
	init_exp_data(&empty);
	
	//push tabulky symbolu pro main		
	var_stack_push(var_stack,&empty,val_mem,NULL);					

	*call_instruction=bin_tree_tmp->instruction;
	
	return ERR_CODE_OK;
}

/**
 * Zisti datovy typ premennej alebo literalu
 * @param param1 TData* premenna alebo literal, ktoreho datovy typ chcem zistit
 * @return TData datovy typ ulozeny ako string
 */
TData type(TData *param1)
{
	TData result;
	init_exp_data(&result);
	
	result.val_str=my_malloc(sizeof(char)*MAX_LENGHT_DATA_TYPE);
	if(result.val_str==NULL)
	{
		
	}
		
	if(param1->type == STRING)
	{
		strcpy(result.val_str,"string");
	}
	
	if(param1->type == NUMBER)
	{
		strcpy(result.val_str,"number");
	}
	if(param1->type == NIL)
	{
		strcpy(result.val_str,"nil");
	}
	if(param1->type == BOOL)
	{
		strcpy(result.val_str,"boolean");
	}
	result.type=STRING;
	return result;
}

/**
 * Konec funkce (bez return)
 * @param inst_list TInst_list * instrukcni paska
 * @param var_stack TVarStack * zasobnik
 * @param inst TInstruction * instrukce
 * @param val_mem TVal_mem* pole hodnot
 * @param inter int flag zda jde o interni funkci
 * @return int vraceni chybove hodnoty
 */
int f_inst_end_of_function(TInst_list *inst_list,TVarStack *var_stack,TInstruction *inst,TVal_mem *val_mem,int inter)
{
	TVarStackData var_stack_ret_adr;
	TVarStackData var_stack_fnc_address;
	TVarStackData values_pop;
	TVarStackData var_stack_tmp;
	
	//ziskam adresu posledni arg instrukce, dalsi instrukce bude nasledovat za funkci
	var_stack_pop(var_stack,var_stack_ret_adr);
	
	//aby nedoslo k zniceni pameti pro interni funkce, ta se uvolnuje az na konci programu
	if(inter==1)
		destroy_memory(*val_mem,var_stack_ret_adr.data.val_num);
	
	//adresa instrukce z mista kde byla volana funkce
	var_stack_pop(var_stack,var_stack_fnc_address);	

	//vypopovani vsech veci z funkce
	if(var_stack_fnc_address.data.val_num>0)
		while(var_stack_fnc_address.data.val_num--)
		{
			var_stack_pop(var_stack,values_pop);
		}
	
	//a tady si tabulku symbolu, ktera z predesle funkce
	var_stack_pop(var_stack,var_stack_tmp);
	*val_mem=var_stack_tmp.val_mem;
	stack_data_free(&var_stack_tmp);
	
	inst=(TInstruction*)var_stack_ret_adr.instruction;
	inst_list->actual_instruction=inst;
	
	stack_data_free(&var_stack_ret_adr);
	stack_data_free(&var_stack_fnc_address);
	return ERR_CODE_OK;
}

/**
 * Konec funkce (s return)
 * @param inst_list TInst_list * instrukcni paska
 * @param var_stack TVarStack * zasobnik
 * @param inst TInstruction * instrukce
 * @param call_instruction TInstruction * instrukce odkud byla funkce volana, ve volani funkce je totiz hodnota pro prirazeni s return
 * @param val_mem TVal_mem* pole hodnot
 * @param inter int flag zda jde o interni funkci
 * @return int vraceni chybove hodnoty
 */
int f_inst_call_return(TInst_list *inst_list,TVarStack *var_stack,TInstruction *inst,TInstruction **call_instruction,TVal_mem *val_mem)
{
	if(var_stack->top==-1)
		return ERR_CODE_OK;
			
	TExpResult result;
	TVarStackData vs_ret_adr;
	TVarStackData vs_fnc_address_and_param;
	TVarStackData values_pop;
	TVarStackData vs_local_sym_tab;
	
	TData *id=NULL;
	TVal_mem local_val_mem=NULL;
	
	//ziskam adresu posledni arg instrukce, dalsi instrukce bude nasledovat za funkci
	var_stack_pop(var_stack,vs_ret_adr);
	//adresa instrukce z mista kde byla volana funkce a navic se vezme polozka poctu argumentu
	var_stack_pop(var_stack,vs_fnc_address_and_param);
	
	//vzhodnoceni navratoveho vyrazu
	result=evaluate_exp_tree(inst->tree,*val_mem);
	
	// ak je tam chyba
	if(result.error != ERR_CODE_OK) return result.error;
	
	//vypopovani vsech veci z funkce
	if(vs_fnc_address_and_param.data.val_num>0)
		while(vs_fnc_address_and_param.data.val_num--)
		{
			var_stack_pop(var_stack,values_pop);
		}
	
	var_stack_pop(var_stack,vs_local_sym_tab);
	local_val_mem=*val_mem;
	
	*val_mem=vs_local_sym_tab.val_mem;

	*call_instruction=(TInstruction*)vs_fnc_address_and_param.instruction;
	id=get_from_memory(*val_mem,vs_fnc_address_and_param.instruction->var1_order);
	
	free_old_data_str(id);
	
	if(result.data.type == STRING)
	{
		id->val_str=my_malloc(sizeof(char)*(strlen(result.data.val_str)+1));
		if(id->val_str==NULL)
			return ERR_CODE_IN_INTER;
		//Nakopirovani noveho retezce
		strcpy(id->val_str,result.data.val_str);
		//my_free(result.data.val_str);
		id->type=STRING;
	}
	else
		*id=result.data;	
	
	//nastaveni instrukce kde se bude vracet
	inst=vs_ret_adr.instruction;
	inst_list->actual_instruction=inst;
	
	//cisteni pameti

	destroy_memory(local_val_mem,(int)vs_ret_adr.data.val_num);
	
	return ERR_CODE_OK;
}

/**
 * Volani vnitrnich funkci
 * @param inst_list TInst_list * instrukcni paska
 * @param inst TInstruction * instrukce
 * @param inst_func TInstruction * instrukce odkud byla volana funkce, pro prirazeni hodnoty z return
 * @param inter_fce int flag_zda se jedna o interni funkci
 * @param val_mem TVal_mem pole hodnot
 * @param var_stack TVarStack * zasobnik
 * @param act_arg int* aktulani argument
 * @return int vraceni chybove hodnoty
 */
int f_inter(TInst_list *inst_list,TInstruction *inst,TInstruction *inst_func,int inter_fce,TVal_mem val_mem,TVarStack *var_stack, int *act_arg){

	TData result;
	init_exp_data(&result);
	TData *id=NULL;
	TData *param1=NULL;
	TData *param2=NULL;
	TData *param3=NULL;
	*act_arg=0;

	switch(inter_fce)
	{
		case TK_KEY_SUBSTR:
			f_inst_arg_var(1,val_mem,var_stack,act_arg);
			f_inst_arg_var(2,val_mem,var_stack,act_arg);
			f_inst_arg_var(3,val_mem,var_stack,act_arg);
			param1=get_from_memory(val_mem,1);
			param2=get_from_memory(val_mem,2);
			param3=get_from_memory(val_mem,3);
			result=substr(param1,param2,param3);
		break;
	
		case TK_KEY_FIND:
			f_inst_arg_var(1,val_mem,var_stack,act_arg);
			f_inst_arg_var(2,val_mem,var_stack,act_arg);
			param1=get_from_memory(val_mem,1);
			param2=get_from_memory(val_mem,2);
			result=t_boyer_moor(param1,param2);	
		break;
		
		case TK_KEY_TYPE:
			f_inst_arg_var(1,val_mem,var_stack,act_arg);
			param1=get_from_memory(val_mem,1);

			result=type(param1);
		break;
		
		case TK_KEY_SORT:
			f_inst_arg_var(1,val_mem,var_stack,act_arg);
			param1=get_from_memory(val_mem,1);
			result=quick_sort(param1);
		break;
		
		default: 
		break;
	}
	
	f_inst_end_of_function(inst_list,var_stack,inst,&val_mem,0);
	id=get_from_memory(val_mem,inst_func->var1_order);
	free_old_data_str(id);
	if(result.type == STRING)
	{
		id->val_str=my_malloc(sizeof(char)*(strlen(result.val_str)+1));
		if(id->val_str==NULL)
			return ERR_CODE_IN_INTER;
		strcpy(id->val_str,result.val_str);
		my_free(result.val_str);
		id->type=STRING;
	}
	else
		*id=result;	
	return ERR_CODE_OK;
}

/**
 * Start intepretu
 * @param inst_list TInst_list * instrukcni paska
 * @param bin_tree Tbin_tree * hlavni tabulka symbolu
 * @return int chybovy stav
 */
int start_interpret(TInst_list *inst_list,Tbin_tree *bin_tree)
{
	//inst_print(inst_list);
	//veskere pomocne ukazatele a promenne
	TInstruction *instruction;
	TInstruction *call_instruction=NULL; 
	TInstruction *func_instruction=NULL;
	//zasobnik
	TVarStack var_stack;
	var_stack_init(&var_stack);
	TData init_var;
	int err_code=0;

	//pocitadlo argumentu funkce, nuluje se pokud je zavolana nova funkce	
	int arg_count=0;
	
	//priznaku skoku zpet, u while
	int jmp_flag=0;
	
	//indexo pro prejmenovani argumentu 
	int act_arg=0;

	//volani funkce pro debug zasobniku
	int var_total=0;
	
	//vysledek pro vraceni z mainu
	TExpResult result;
	
	if(inst_list->actual_instruction == NULL)
	{
		fprintf(stderr,"Chybi main v programu");
		return ERR_CODE_INTER;
	}
	else
	{

		TData* val_mem=my_malloc(sizeof(TData)*(inst_list->actual_instruction->var_total+1));
		TVal_mem val_inter_f=my_malloc(sizeof(TData)*5);
		 init_memory(val_inter_f, 5);
		 init_memory(val_mem, inst_list->actual_instruction->var_total+1);
		if(val_inter_f==NULL)
			return ERR_CODE_IN_INTER;
			
		if(val_mem==NULL)
			return ERR_CODE_IN_INTER;
			
		//preskoci pripadne argumenty v mainu
		instruction=get_next_instruction(inst_list);		
		while(instruction->operator==INST_ARG_VAR)
		{
			instruction=get_next_instruction(inst_list);
		}

		while(1)
		{
			//printf("%.3d. addr: %.10d jmp: %.10d Operator: %s \t'%s'(%d) \t'%s'(%d) \n", instruction->inst_num, (int)instruction, (int)instruction->instruction_jump, INST_TEXTY[instruction->operator], instruction->operand1.key, instruction->operand1.token_type, instruction->operand2.key, instruction->operand2.token_type);

			switch(instruction->operator)
			{	
				case INST_ID_ASSIGN:
					//ziskani hodnoty bud integer vraceny funkci nebo strin ve val_string, prirazuje se ukaze z expr_tree
					if((err_code=f_inst_id_assign(instruction,val_mem)) != ERR_CODE_OK)
						return err_code;
				break;
				
				case INST_READ:
					if((err_code=read(val_mem,instruction->var1_order,instruction->operand2.key)) != ERR_CODE_OK)
						return err_code;
				break;
				
				case INST_CALL_WRITE:
					if((err_code=write(instruction->tree,val_mem)) != ERR_CODE_OK)
						return err_code;
				break;
				
				case INST_CALL_WHILE:
					if((err_code=f_inst_call_while(inst_list,instruction,val_mem)) != ERR_CODE_OK)
						return err_code;
				break;
				
				case INST_CALL_END_WHILE:
					instruction=instruction->instruction_jump;
					inst_list->actual_instruction=instruction;
					jmp_flag=1;
				break;
				
				case INST_CALL_IF:
					//if se provede pokud je hodnota vyrazu vetsi nez 1
					if((err_code=f_inst_call_if(inst_list,instruction,val_mem)) != ERR_CODE_OK)
						return err_code;
						
				break;
				
				//tato instrukce se preskakuje
				case INST_CALL_ELSE:
					jmp_flag=1;
					instruction=instruction->instruction_jump;
					inst_list->actual_instruction=instruction;
				break;
				
				//uzavreni ifu, zatim na nic nevyuzivam
				case INST_CALL_END_IF:
				
				break;
				
				//priradi parametru nove nazvy a ulozeni je do lokalni tabulky symbolu
				case INST_ARG_VAR:
					if((err_code=f_inst_arg_var(instruction->var1_order,val_mem,&var_stack,&act_arg)) != ERR_CODE_OK)
						return err_code;
				break;
			
				case INST_END_OF_MAIN:
					//destroy_memory(val_mem);
				return 0;
      
				case INST_LOCAL_VAR:
					//pridani localni hodnoty do lokalni tabulky symbolu
					
					init_exp_data(&init_var);
					update_memory(val_mem,instruction->var1_order,init_var);
				break;
					
				case INST_INIT_FUNCTION:
					var_total=instruction->var_total+1;
					if((err_code=f_inst_init_function(&val_mem,&act_arg,var_total)) != ERR_CODE_OK)
						return err_code;
				break;
					
				case INST_CALL_ARG:
					if((err_code=f_inst_call_arg(instruction,val_mem,&var_stack,&arg_count)) != ERR_CODE_OK)
						return err_code;
					if((instruction->next != NULL) &&  (instruction->next->operator != INST_CALL_ARG))
					{
						TData arg_c;
						init_exp_data(&arg_c);
						//pushnuti posledniho argumentu na zasobnik
						arg_c.val_num=arg_count;
						arg_c.type=NUMBER;
						arg_c.val_str=NULL;
						
						if(func_instruction->operand2.token_type != TK_VAR )
						{
							//ulozeni adresy odkud je insturkce volana a poctu parametru (je skoda nevyuzit mista)
							var_stack_push(&var_stack,&arg_c,NULL,instruction);
							//Push adresy navratove adresy na zasobnik
							arg_c.val_num=(double) 5;
							var_stack_push(&var_stack,&arg_c,NULL,instruction);
							f_inter(inst_list,instruction,func_instruction,func_instruction->operand2.token_type,val_inter_f,&var_stack,&arg_count);
						}
						else
						{
							//ulozeni adresy odkud je insturkce volana a poctu parametru (je skoda nevyuzit mista)
							var_stack_push(&var_stack,&arg_c,NULL,func_instruction);
							arg_c.val_num=(double) call_instruction->var_total+1;
							//Push adresy navratove adresy na zasobnik

							var_stack_push(&var_stack,&arg_c,NULL,instruction);
						
							//nastaveni instrukce a aktualni instrukce v def. pasky
							instruction=call_instruction;
							inst_list->actual_instruction=instruction;
							jmp_flag=1;		
						}	
					}
					
				break;
				
				//tady se budu divat do tabulky symbolu kde zacne funkce a nastavim call_instruction viz volani funkce na zacatku
				//push tabulky symbolu pro main			
				case INST_CALL_FUNCTION:				
					arg_count=0;
					func_instruction=instruction;

					if((err_code=f_inst_call_function(instruction,&call_instruction,bin_tree,val_mem,&var_stack)) != ERR_CODE_OK)
						return err_code;
					if((instruction->next != NULL) &&  (instruction->next->operator != INST_CALL_ARG))
					{
						TData arg_c;
						//pushnuti posledniho argumentu na zasobnik
						init_exp_data(&arg_c);
						arg_c.val_num=0;
						arg_c.type=NUMBER;
						arg_c.val_str=NULL;
						
						if(func_instruction->operand2.token_type != TK_VAR )
						{
							//ulozeni adresy odkud je insturkce volana a poctu parametru (je skoda nevyuzit mista)
							var_stack_push(&var_stack,&arg_c,NULL,instruction);
							//Push adresy navratove adresy na zasobnik
							arg_c.val_num=(double) 5;
							var_stack_push(&var_stack,&arg_c,NULL,instruction);
							f_inter(inst_list,instruction,func_instruction,func_instruction->operand2.token_type,val_inter_f,&var_stack,&arg_count);
						}
						else
						{
							//ulozeni adresy odkud je insturkce volana a poctu parametru (je skoda nevyuzit mista)
							var_stack_push(&var_stack,&arg_c,NULL,func_instruction);
							arg_c.val_num=(double) call_instruction->var_total+1;
							//Push adresy navratove adresy na zasobnik
							var_stack_push(&var_stack,&arg_c,NULL,instruction);
						
							//nastaveni instrukce a aktualni instrukce v def. pasky
							instruction=call_instruction;
							inst_list->actual_instruction=instruction;
							jmp_flag=1;		
						}
					}
				break;
						
				case INST_CALL_RETURN:
					if(var_stack.top==-1)
					{
						//vyhodnoceni navratoveho vyrazu
						result=evaluate_exp_tree(instruction->tree,val_mem);
						if(result.error != ERR_CODE_OK) return result.error;
						
						return ERR_CODE_OK;
					}
					if((err_code=f_inst_call_return(inst_list,&var_stack,instruction,&call_instruction,&val_mem)) != ERR_CODE_OK)
						return err_code;
				break;
						
						
				case INST_END_OF_FUNCTION:
					if((err_code=f_inst_end_of_function(inst_list,&var_stack,instruction,&val_mem,1)) != ERR_CODE_OK)
						return err_code;
				break;
						
			}
			if(!jmp_flag)
			{
				instruction=get_next_instruction(inst_list);
			}
			else
				jmp_flag=0;
		}
	}
	return ERR_CODE_OK;
}
