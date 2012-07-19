/**
 * ------------------------------------------
 * Project : Interpreter jazyka IFJ11 
 * Name    : Lexikalni analyzator
 * File    : scaner.c
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
#include <stdbool.h>
#include "types.h"
#include "my_string.h"
#include "scaner.h"

/**
 * Zistuje, ci je retazec klucove slovo
 * @param word char* retazec, ktory chceme overit
 * @return int typ klucoveho slova alebo -1 v pripade, ze to nie je klucove slovo
 */
int key_or_var(char *word)
{
	int i = 0;
	for(i = 0; i < NUM_KEYWORDS; i++)
	{
		if(strcmp(word, KEYWORD_TABLE[i].name)==0) 
		{
			return KEYWORD_TABLE[i].type;
		}
	}
	return -1;
}

/**
 * Funkce pro nacteni jednotlivych tokenu.
 * Funkce postupne nacita jednotlive znaky, pokud je nacten operator, ulozi ho,
 * pokud je to ridici operator (",-,=,~,.) prejde do jemu prirazenemu stavu.
 * Pokud funkce nacte klicove slovo/identifikator, prejde do jemu danemu stavu.
 * Pokud funkce nacte cokoliv jineho nez operator, zjisti, zda neni nactene
 * cislo, komentar, string, klicove slovo/identifikator nebo promenna.
 * Pokud se posloupnost znaku neshoduje ani s jednim stavem, vypise lexikalni chybu.
 * @param F File* ukazatel na soubor.
 * @param token TToken* ukazatel na token.
 * @return cislo chyby. Pokud je cokoliv jineho nez 0 funkce main vypise chybu.
 */
int get_token(FILE *F, TToken *token)
{
	char actual_char;
	int state = token->next_state;
	int back_slash = 0;
	int ascii_value = 0;
	int ascii_num_toggle = 0;
	token->next_state = ST_START;
	token->name.length = 0;
	token->name.chars[0] = '\0';
	
	bool byla_tecka = false;
	bool bylo_e = false;
				
	
	while((actual_char = fgetc(F)) != EOF)
	{
		switch(state)
		{
			// pocatecni stav
			case ST_START:
				
				// kdyz je konec radku, inkrementuje pocitadlo
				if(actual_char == EOL)
				{
					token->line++;
				}
				
				if(isspace(actual_char))
				{
					// nestane se nic, bile znaky jsou ignorovany
				}
				
				else if(actual_char == ';')
				{
					add_char_to_my_string(&token->name, actual_char);
					token->type = TK_SEMICOLON;
					return E_SCAN_OK;
				}
				
				else if(actual_char == ',')
				{
					add_char_to_my_string(&token->name, actual_char);
					token->type = TK_COMMA;
					return E_SCAN_OK;
				}
				
				else if(actual_char == '(')
				{
					add_char_to_my_string(&token->name, actual_char);
					token->type = TK_LBRACKET;
					return E_SCAN_OK;
				}
				
				else if(actual_char == ')')
				{
					add_char_to_my_string(&token->name, actual_char);
					token->type = TK_RBRACKET;
					return E_SCAN_OK;
				}
				
				else if(actual_char == '^')
				{
					add_char_to_my_string(&token->name, actual_char);
					token->type = TK_EXP;
					return E_SCAN_OK;
				}
				
				else if(actual_char == '*')
				{
					add_char_to_my_string(&token->name, actual_char);
					token->type = TK_MULTIPLY;
					return E_SCAN_OK;
				}
				
				else if(actual_char == '/')
				{
					add_char_to_my_string(&token->name, actual_char);
					token->type = TK_DIVIDE;
					return E_SCAN_OK;
				}

				else if(actual_char == '%')
				{
					add_char_to_my_string(&token->name, actual_char);
					token->type = TK_MODULO;
					return E_SCAN_OK;
				}
				
				else if(actual_char == '+')
				{
					add_char_to_my_string(&token->name, actual_char);
					token->type = TK_PLUS;
					return E_SCAN_OK;
					
				}
				
				else if(actual_char == '"')
				{
					/*add_char_to_my_string(&token->name, actual_char);
					token->type = TK_QUOTE;
					token->next_state = ST_STRING;
					return E_SCAN_OK;*/
					token->name.length = 0;
					token->name.chars[0] = '\0';
					state = ST_STRING;
				}
				
				else if(actual_char == '-')
				{
					add_char_to_my_string(&token->name, actual_char);
					state = ST_MINUS;
				}
				
				else if(actual_char == '=')
				{
					add_char_to_my_string(&token->name, actual_char);
					state = ST_ASSIGN;
				}
				
				else if(actual_char == '~')
				{
					add_char_to_my_string(&token->name, actual_char);
					state = ST_WAVE;
				}
				
				else if(actual_char == '.')
				{
					add_char_to_my_string(&token->name, actual_char);
					state = ST_DOT;
				}
				
				else if(actual_char == '<')
				{
					add_char_to_my_string(&token->name, actual_char);
					state = ST_LOWER;
				}
				
				else if(actual_char == '>')
				{
					add_char_to_my_string(&token->name, actual_char);
					state = ST_GREATER;
				}
				
				else if(actual_char == '_')
				{
					add_char_to_my_string(&token->name, actual_char);
					state = ST_VAR;
				}
				
				// pismena
				else if(isalpha(actual_char))
				{
					add_char_to_my_string(&token->name, actual_char);
					state = ST_KEY_OR_VAR;
				}
				
				// cisla
				else if(isdigit(actual_char))
				{
					add_char_to_my_string(&token->name, actual_char);
					state = ST_NUMBER;
				}
				
				// vse ostatni je chyba
				else
				{
					return ERR_CODE_LEX;
				}
				
			break;
			
			// znak -
			case ST_MINUS:
				
				// kdyz je komentar
				if(actual_char == '-')
				{
					add_char_to_my_string(&token->name, actual_char);
					state = ST_COMMENT;
				}
				
				else
				{
					token->type = TK_MINUS;
					ungetc(actual_char, F);
					return E_SCAN_OK;
				}
				
			break;
			
			// retezec
			case ST_STRING:
				if(actual_char == '"')
				{
					// su escapovane
					if((token->name.length-1) >= 0 && (back_slash%2) == 1 && token->name.chars[token->name.length-1] == '\\')
					{
						add_char_to_my_string(&token->name, actual_char);
					}
					// konec stringu
					else
					{
						// nebolo ukoncene trojciferne ascii cislo
						if(ascii_num_toggle > 0)
						{
							return ERR_CODE_LEX;
						}
						
						token->type = TK_STRING;
						token->next_state = ST_START;
						return E_SCAN_OK;
					}
				}
				else
				{
					add_char_to_my_string(&token->name, actual_char);
				}
				
				// nenacitalo sa kompletne ascii cislo
				if(ascii_num_toggle > 0 && !(actual_char >= '0' && actual_char <= '9'))
				{
					return ERR_CODE_LEX;
				}
				
				// ASCII cislo znak
				if(
				actual_char >= '0' && actual_char <= '9' && 
				( ((back_slash%2) == 1 && back_slash > 0) || (ascii_num_toggle > 0 && ascii_num_toggle < 4) )
				)
				{
					// inkrementacia pocitadla znaku
					ascii_num_toggle++;
					// priradenie hodnoty
					ascii_value = ascii_value * 10;
					ascii_value += actual_char - '0';
				}
				else
				{					
					ascii_num_toggle = 0;
					ascii_value = 0;
				}
				
				// escapeovane znaky
				if(back_slash%2 == 1 && !(actual_char >= '0' && actual_char <= '9'))
				{
					switch(actual_char)
					{
						case 'n':
							token->name.length--;
							token->name.chars[token->name.length-1] = '\n';
							token->name.chars[token->name.length] = '\0';
						break;
						case 't':
							token->name.length--;
							token->name.chars[token->name.length-1] = '\t';
							token->name.chars[token->name.length] = '\0';
						break;
						case '\\':
							token->name.length--;
							token->name.chars[token->name.length-1] = '\\';
							token->name.chars[token->name.length] = '\0';
						break;
						case '"':
							token->name.length--;
							token->name.chars[token->name.length-1] = '"';
							token->name.chars[token->name.length] = '\0';
						break;
						default:
							return ERR_CODE_LEX;
						break;
					}
				}
				
				// mame 3 cifry, ak je to cislo od 1 do 255,
				// tak tam priradime znak z ASCII tabulky
				if(ascii_num_toggle == 3 && ascii_value >= 1 && ascii_value <= 255)
				{
					token->name.length -= 3;
					token->name.chars[token->name.length-1] = ascii_value;
					token->name.chars[token->name.length] = '\0';
					ascii_num_toggle = 0;
				}
				
				// postupnost back slashov
				if(actual_char == '\\')
				{
					back_slash++;
				}
				else
				{
					back_slash = 0;
				}
				
			break;
			
			// znak =
			case ST_ASSIGN:
			
				// ==
				if(actual_char == '=')
				{
					add_char_to_my_string(&token->name, actual_char);
					token->type = TK_EQUAL;
					return E_SCAN_OK;
				}
				
				// =
				else
				{
					token->type = TK_ASSIGN;
					ungetc(actual_char, F);
					return E_SCAN_OK;
				}
			
			break;
			
			// znak ~
			case ST_WAVE:
			
				// ~=
				if(actual_char == '=')
				{
					add_char_to_my_string(&token->name, actual_char);
					token->type = TK_NON_EQUAL;
					return E_SCAN_OK;
				}		
				
				// nikde jinde ~ byt nemuze
				else
				{
					return ERR_CODE_LEX;
				}		
			
			break;
			
			// znak <
			case ST_LOWER:
			
				// <=
				if(actual_char == '=')
				{
					add_char_to_my_string(&token->name, actual_char);
					token->type = TK_LOWER_EQUAL;
					return E_SCAN_OK;
				}
				
				// <
				else
				{
					ungetc(actual_char, F);
					token->type = TK_LOWER;
					return E_SCAN_OK;
				}
				
			break;
			
			// znak >
			case ST_GREATER:
			
				// >=
				if(actual_char == '=')
				{
					add_char_to_my_string(&token->name, actual_char);
					token->type = TK_GREATER_EQUAL;
					return E_SCAN_OK;
				}
				
				// >
				else
				{
					ungetc(actual_char, F);
					token->type = TK_GREATER;
					return E_SCAN_OK;
				}
			
			break;
			
			// znak tecka
			case ST_DOT:
			
				// ..
				if(actual_char == '.')
				{
					add_char_to_my_string(&token->name, actual_char);
					token->type = TK_JOIN;
					return E_SCAN_OK;
				}		
				
				// nikde jinde . byt nemuze
				else
				{
					return ERR_CODE_LEX;
				}		
				
			break;
			
			// komentare
			case ST_COMMENT:
			
				// kdyz je blokovy komentar
				if(strcmp(token->name.chars, "--[[") == 0)
				{
					// kdyz je konec radku, inkrementuje pocitadlo
					if(actual_char == EOL)
					{
						token->line++;
					}
					token->name.length = 0;
					token->name.chars[0] = '\0';
					ungetc(actual_char, F);
					state = ST_BLOCK_COMMENT;
					continue;
				}
				
				// kdyz uz neni sance, ze bude blokovy
				if(token->name.length >= 4 && state == ST_COMMENT)
				{
					if(actual_char != EOL)
					{
						while((actual_char = fgetc(F)) != EOL && actual_char != EOF);
					}
					token->line++;
					token->name.length = 0;
					state = ST_START;
				}
				// kdyz jsou 3 znaky (napr: --[)
				else 
				{
					// ak nastal koniec riadka
					if(actual_char == EOL)
					{
						token->line++;
						token->name.length = 0;
						state = ST_START;
					}
					else
					{
						add_char_to_my_string(&token->name, actual_char);
					}
				}
				
			break;
			
			// blokove komentare
			case ST_BLOCK_COMMENT:
				
				// kdyz je konec radku, inkrementuje pocitadlo
				if(actual_char == EOL)
				{
					token->line++;
				}
				if(actual_char == ']')
				{
					add_char_to_my_string(&token->name, actual_char);
					// kdyz je ukoncovaci (]])
					if(strcmp(token->name.chars, "]]") == 0)
					{
						token->name.length = 0;
						token->name.chars[0] = '\0';
						state = ST_START;
						break;
					}
				}
				else
				{
					token->name.length = 0;
					token->name.chars[0] = '\0';
				}
				
			break;
			
			// cisla
			case ST_NUMBER:
				
				if(isdigit(actual_char))
				{
					add_char_to_my_string(&token->name, actual_char);
				}
				
				else if(actual_char == 'e' || actual_char == 'E')
				{
					if(bylo_e == true)
					{
						return ERR_CODE_LEX;
					}
					bylo_e = true;
					add_char_to_my_string(&token->name, actual_char);
					actual_char = fgetc(F);
					
					if(isdigit(actual_char))
					{
						add_char_to_my_string(&token->name, actual_char);
					}
					
					else if(actual_char == '+' || actual_char == '-')
					{
						add_char_to_my_string(&token->name, actual_char);
						actual_char = fgetc(F);
										
						if(isdigit(actual_char))
						{
							add_char_to_my_string(&token->name, actual_char);
						}
						
						else
						{
							return ERR_CODE_LEX;
						}
					}
					
					else
					{
						return ERR_CODE_LEX;
					}
				}
				
				else if(actual_char == '.')
				{
					if(byla_tecka == true || bylo_e == true)
					{
						return ERR_CODE_LEX;
					}
					byla_tecka = true;
					add_char_to_my_string(&token->name, actual_char);
					actual_char = fgetc(F);
					
					if(isdigit(actual_char))
					{
						add_char_to_my_string(&token->name, actual_char);
					}
					
					else
					{
						return ERR_CODE_LEX;
					}
				}
				
				else
				{
					// ulozi nactene cislo do seznamu
					ungetc(actual_char, F);
					token->type = TK_NUMBER;
					return E_SCAN_OK;
				}
				
			break;
			
			// promenne
			case ST_VAR:
				
				if(isalpha(actual_char) || isdigit(actual_char) || actual_char == '_')
				{
					add_char_to_my_string(&token->name, actual_char);
				}
				
				else
				{
					// ulozi nactenou posloupnost znaku do seznamu
					ungetc(actual_char, F);
					token->type = TK_VAR;
					return E_SCAN_OK;
				}
			
			break;
			
			// promenne nebo klicova/rezervovana slova
			case ST_KEY_OR_VAR:
				
				if(isalpha(actual_char) || actual_char == '_' || isdigit(actual_char))
				{
					add_char_to_my_string(&token->name, actual_char);
				}
				
				else
				{
					int key_type = key_or_var(token->name.chars);
					if(key_type == -1)
					{
						state = ST_VAR;
						ungetc(actual_char, F);
					}
					
					else
					{
						token->type = key_type;
						ungetc(actual_char, F);
						return E_SCAN_OK;
					}
				}
			
			break;
		}
	}
	
	//printf("%d (%s)\n", state, token->name.chars);
	
	// prisli sme z riadkoveho komentara
	if(state == ST_COMMENT && strcmp("--[[", token->name.chars) == 0)
		// zacali sme nacitavat blokovy, ale neocakavane prisiel EOF
		return ERR_CODE_LEX;
	else if(state == ST_COMMENT)
		// bol to obycajny riadkovy
		return E_SCAN_EOF;
	
	// pokud nastane EOF driv, nez ma, vratime chybu
	if(strlen(token->name.chars) != 0)
	{
		// ak sme tam mali cislo a bolo zrazu ukoncene
		if(state == ST_NUMBER)
		{
			if(isdigit(token->name.chars[token->name.length-1])) return ERR_CODE_SYN;
		}
		
		if(state == ST_VAR || state == ST_KEY_OR_VAR) return ERR_CODE_SYN;
		
		if(state == ST_START && token->name.length == 0) return E_SCAN_EOF;
		
		return ERR_CODE_LEX;
	}
	
	if(state == ST_BLOCK_COMMENT) {
		
		return ERR_CODE_LEX;
	}
	
	
	return E_SCAN_EOF;
}
