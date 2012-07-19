/**
 * ------------------------------------------
 * Project : Interpreter jazyka IFJ11 
 * Name    : Praca so stringom
 * File    : my_string.c
 * Authors : Karel Hala (xhalak00)
 *         : Pavlina Bartikova (xbarti00)
 *         : Tomas Goldmann (xgoldm03)
 *         : Branislav Blaskovic (xblask00)
 * ------------------------------------------
 */
 
 #include <stdlib.h>
 #include <stdio.h>
 #include "types.h"
 #include "my_memory.h"
 #include "scaner.h"
 #include "my_string.h"

/**
 * Prida 1 znak do retazca
 * @param my_word my_string* ukazatel na retazec, do ktoreho znak pridavame
 * @param character char znak, ktory pridavame
 * @return int chybovy stav
 */
int add_char_to_my_string(my_string *my_word, char character)
{
	// inkrementace delky 
	my_word->length += 1;
	
	// kdyz neni misto -> realloc
	if((my_word->length) >= my_word->size_of_alloc) 
	{
		my_word->chars = (char *)my_realloc(my_word->chars, (my_word->length + CHARS_PLUS_ALLOC) * sizeof(char));
		my_word->size_of_alloc = (my_word->length + CHARS_PLUS_ALLOC);
	}
	
	// ulozi se znak a nulty bajt
	my_word->chars[my_word->length-1] = character;
	my_word->chars[my_word->length] = '\0';
	
	return ERR_CODE_OK;
}

/**
 * Inicializacia nasho retazca
 * @param my_word my_string* ukazatel na retazec, ktory chceme inicializovat
 * @return int chybovy stav
 */
int init_my_string(my_string *my_word)
{
	my_word->chars = (char *)my_malloc(CHARS_PLUS_ALLOC);
	
	// chyba
	if(my_word->chars == NULL)
	{
		return ERR_CODE_IN_INTER;
	}
		
	my_word->chars[0] = '\0';
	my_word->length = 0;
	my_word->size_of_alloc = CHARS_PLUS_ALLOC;
	
	return ERR_CODE_OK;
}

/**
 * Uvolni retazec
 * @param my_word my_string* ukazatel na retazec, ktory chceme uvolnit
 * @return void
 */
void free_my_string(my_string *my_word)
{
	if(my_word->size_of_alloc > 0) 
	{
		my_free(my_word->chars);
		my_word->chars = NULL;
	}
	
	my_word->length = 0;
	my_word->size_of_alloc = 0;
}
