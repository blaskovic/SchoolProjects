/**
 * ------------------------------------------
 * Project : Interpreter jazyka IFJ11 
 * Name    : Sprava pamate
 * File    : my_memory.c
 * Authors : Karel Hala (xhalak00)
 *         : Pavlina Bartikova (xbarti00)
 *         : Tomas Goldmann (xgoldm03)
 *         : Branislav Blaskovic (xblask00)
 * ------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "scaner.h"
#include "my_memory.h"

/**
 * Vytvori hash pre hashovaciu tabulku
 * @param ptr void* pointer, pre ktory vytvarame hash
 * @return int vytvoreny hash
 */
int memory_hash(void *ptr)
{
	return ((int)ptr)%HASH_TABLE_NUM;
}

/**
 * Odstrani z hashovacej tabulky pointer
 * @param ptr void* pointer, ktory chceme odobrat
 * @param free_ptr int znacka, ci chceme pointer aj uvolnit
 * @return void
 */
void memory_remove(void *ptr, int free_ptr)
{
	TMemory_item *tmp = NULL;
	TMemory_item *previous = NULL;
	
	int hash = memory_hash(ptr);
	
	if(memory[hash] == NULL) return;
	
	tmp = memory[hash];
	
	while( tmp != NULL )
	{
		
		if( tmp->ptr == ptr )
		{
			// previazanie
			if( previous != NULL )
			{
				previous->next = tmp->next;
			}
			// prvy prvok
			if( tmp == memory[hash] )
			{
				memory[hash] = tmp->next;
			}
			
			if(free_ptr == 1) free(tmp->ptr);
			
			free(tmp);
			tmp = NULL;
			return;
		}
		else
		{
			// nebol to aktualny, tak ideme na dalsi
			previous = tmp;
			tmp = tmp->next;
		}
		
	}
	
}

/**
 * Vlozi pointer do hashovacej tabulky
 * @param ptr void* pointer, ktory vkladame
 * @return void
 */
void memory_insert(void *ptr)
{
	TMemory_item *new = (TMemory_item *)malloc(sizeof(struct struct_memory_item));
	if(new == NULL) return;
	
	int hash = memory_hash(ptr);
	
	new->ptr = ptr;
	new->next = memory[hash];
	
	memory[hash] = new;
}

/**
 * Uvolni vsetku pamat
 * @return void
 */
void memory_free_all()
{
	if( memory == NULL ) return;
	
	int i = 0;
	TMemory_item *tmp = NULL;
	TMemory_item *tmp2 = NULL;
	
	// pre kazdy prvok si to uvolnim
	for(i = 0; i < HASH_TABLE_NUM; i++)
	{
		if( memory[i] != NULL )
		{
			tmp = memory[i]->next;
			if(memory[i]->ptr != NULL) free( memory[i]->ptr );
			free( memory[i] );
			memory[i] = NULL;
			
			while( tmp != NULL )
			{
				// prejdem cely linearny zoznam
				tmp2 = tmp->next;
				free(tmp->ptr);
				free(tmp);
				tmp = tmp2;
			}
		}
	}
}

/**
 * Inicializacia hashovacej tabulky pre pouzitie na spravu pamate
 * @return int vytvoreny hash
 */
void memory_init()
{
	int i = 0;
	
	for(i = 0; i < HASH_TABLE_NUM; i++)
	{
		memory[i] = NULL;
	}
}

/**
 * Nas specialny realloc()
 * @param old_ptr void* stary pointer
 * @param size size_t velkost, na ktoru chceme zmenit velkost
 * @return void* novy pointer na pamatove miesto o novej velkosti
 */
void *my_realloc(void *old_ptr, size_t size)
{
	void *tmp = realloc(old_ptr, size);
	
	if(tmp != NULL)
	{
		memory_remove(old_ptr, 0);
		memory_insert(tmp);
	}
	
	return tmp;
}

/**
 * Nas specialny free()
 * @param ptr void* pointer na miesto, ktore chceme uvolnit
 * @return void
 */
void my_free(void *ptr)
{
	memory_remove(ptr, 1);
	ptr = NULL;
}

/**
 * Nas specialny malloc()
 * @param size size_t velkost, ktoru chceme vyhradit
 * @return int vytvoreny hash
 */
void *my_malloc(size_t size)
{
	void *mal = malloc(size);
	memory_insert(mal);
	return mal;
}
