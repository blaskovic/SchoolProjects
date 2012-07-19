/**
 * ------------------------------------------
 * Project : Interpreter jazyka IFJ11 
 * Name    : Hlavni program
 * File    : main.c
 * Authors : Karel Hala (xhalak00)
 *         : Pavlina Bartikova (xbarti00)
 *         : Tomas Goldmann (xgoldm03)
 *         : Branislav Blaskovic (xblask00)
 * ------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "my_memory.h"
#include "my_string.h"
#include "scaner.h"
#include "interpreter.h"
#include "parser.h"

/**
 * Hlavna funkcia programu
 * @param argc int pocet argumentov pri volani programu
 * @param argv[] char* pole s argumentami programu
 * @return int kod chyby
 * */
int main(int argc, char* argv[])
{
	FILE *source_file;
	
	// kontrola vstupnych argumentov
	if(argc!=2)
	{
		fprintf(stderr, "Neni korektne zadany vstupni soubor jako jedinny argument programu.");
		return EXIT_FAILURE;
	}
	
	// otevreni zdrojoveho souboru
	if((source_file = fopen(argv[1], "r")) == NULL)
	{
		fprintf(stderr, "Subor se nepodarilo otevrit");
		return EXIT_FAILURE;
	}
	
	int error = ERR_CODE_OK;
	
	// inicializacia spravy pamate
	memory_init();
	
	// zoznam instrukcii
	TInst_list inst_list;
	inst_init_list(&inst_list);
	
	error = start_parse(source_file, &inst_list);
	
	// zavreni souboru, uvolneni pameti
	fclose(source_file);
	memory_free_all();
	
	return error;
	
}
