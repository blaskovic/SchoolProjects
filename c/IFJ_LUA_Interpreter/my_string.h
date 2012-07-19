/**
 * ------------------------------------------
 * Project : Interpreter jazyka IFJ11 
 * Name    : Prace se stringem
 * File    : my_string.h
 * Authors : Karel Hala (xhalak00)
 *         : Pavlina Bartikova (xbarti00)
 *         : Tomas Goldmann (xgoldm03)
 *         : Branislav Blaskovic (xblask00)
 * ------------------------------------------
 */

static const int CHARS_PLUS_ALLOC = 10;

int add_char_to_my_string(my_string *my_word, char character);
int init_my_string( my_string *my_word );
void free_my_string( my_string *my_word );
