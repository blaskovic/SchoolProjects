/**
 * ------------------------------------------
 * Project : Interpreter jazyka IFJ11 
 * Name    : Sprava pamate
 * File    : my_memory.h
 * Authors : Karel Hala (xhalak00)
 *         : Pavlina Bartikova (xbarti00)
 *         : Tomas Goldmann (xgoldm03)
 *         : Branislav Blaskovic (xblask00)
 * ------------------------------------------
 */

// velkost tabulky nastavime na prvocislo, aby nebol problem, ked alokujeme na blokoch
#define HASH_TABLE_NUM 503

TMemory_item *memory[HASH_TABLE_NUM];

int memory_hash(void *ptr);
void memory_remove(void *ptr, int free_ptr);
void memory_insert(void *ptr);
void memory_free_all();
void memory_init();
void *my_realloc(void *old_ptr, size_t size);
void my_free(void *ptr);
void *my_malloc(size_t size);
