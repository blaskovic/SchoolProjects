/*
 * Subor:
 * Datum:	december 2010
 * Autor:	Branislav Blaskovic xblask00@stud.fit.vutbr.cz
 * Projekt:	Ceske radenie, projekt 4 pre predmet IZP
 * Popis:
 */

/**
 * Kniznice
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
* compare tables
**/
const unsigned char cmpTable1[256]={
	// odsadenie pre znaky, ktore som zabudol
	[(unsigned char) 'A']=3,[(unsigned char) 'a']=3, [(unsigned char)'Á']=3,[( unsigned char)'á']=3,
	[(unsigned char) 'B']=4,[(unsigned char) 'b']=4,
	[(unsigned char) 'C']=5,[(unsigned char) 'c']=5, 
	[(unsigned char) 'Æ']=5,[(unsigned char) 'è']=5,
	[(unsigned char) 'D']=7,[(unsigned char) 'd']=7, [(unsigned char) 'Ï']=7,[(unsigned char) 'ï']=7,
	[(unsigned char) 'E']=8,[(unsigned char) 'e']=8, [(unsigned char) 'Ì']=8,[(unsigned char) 'ì']=8, [(unsigned char) 'É']=8,[(unsigned char) 'é']=8,
	[(unsigned char) 'F']=9,[(unsigned char) 'f']=9,
	[(unsigned char) 'G']=10,[(unsigned char) 'g']=10,
	[(unsigned char) 'H']=11,[(unsigned char) 'h']=11,
	// ch = 12/13
	[(unsigned char) '%']=12,
	
	[(unsigned char) 'I']=14,[(unsigned char) 'i']=14,[(unsigned char) 'Í']=14,[(unsigned char) 'í']=14,
	[(unsigned char) 'J']=15,[(unsigned char) 'j']=15, 
	[(unsigned char) 'K']=16,[(unsigned char) 'k']=16,
	[(unsigned char) 'L']=17,[(unsigned char) 'l']=17, 
	[(unsigned char) 'M']=18,[(unsigned char) 'm']=18,
	[(unsigned char) 'N']=19,[(unsigned char) 'n']=19,[(unsigned char) 'Ò']=19,[(unsigned char) 'ò']=19,
	[(unsigned char) 'O']=20,[(unsigned char) 'o']=20,[(unsigned char) 'Ó']=20,[(unsigned char) 'ó']=20,
	[(unsigned char) 'P']=21,[(unsigned char) 'p']=21,
	[(unsigned char) 'Q']=22,[(unsigned char) 'q']=22,
	[(unsigned char) 'R']=23,[(unsigned char) 'r']=23,
	[(unsigned char) 'Ø']=23,[(unsigned char) 'ø']=23,
	[(unsigned char) 'S']=25,[(unsigned char) 's']=25,
	[(unsigned char) '©']=25,[(unsigned char) '¹']=25,
	[(unsigned char) 'T']=27,[(unsigned char) 't']=27,
	[(unsigned char) '«']=27,[(unsigned char) '»']=27,
	[(unsigned char) 'U']=29,[(unsigned char) 'u']=29, [(unsigned char) 'Ú']=29,[(unsigned char) 'ú']=29, [(unsigned char) 'Ù']=29,[(unsigned char) 'ù']=29,
	[(unsigned char) 'V']=30,[(unsigned char) 'v']=30,
	[(unsigned char) 'W']=31,[(unsigned char) 'w']=31, 
	[(unsigned char) 'X']=32,[(unsigned char) 'x']=32,
	[(unsigned char) 'Y']=32,[(unsigned char) 'y']=33,[(unsigned char) 'Ý']=33,[(unsigned char) 'ý']=33,
	[(unsigned char) 'Z']=33,[(unsigned char) 'z']=34,
	[(unsigned char) '®']=33,[(unsigned char) '¾']=33,
	[(unsigned char) '0']=35,
	[(unsigned char) '1']=36,
	[(unsigned char) '2']=37,
	[(unsigned char) '3']=38,
	[(unsigned char) '4']=39,
	[(unsigned char) '5']=40,
	[(unsigned char) '6']=41,
	[(unsigned char) '7']=42,
	[(unsigned char) '8']=43,
	[(unsigned char) '9']=44,
	[(unsigned char) '.']=45,
	[(unsigned char) ',']=46,
	[(unsigned char) ';']=47,
	[(unsigned char) '?']=48,
	[(unsigned char) '!']=49,
	[(unsigned char) ':']=50,
	[(unsigned char) '"']=51,
	[(unsigned char) '-']=52,
	[(unsigned char) '|']=53,
	[(unsigned char) '/']=54,
	[(unsigned char) '\\']=55,
	[(unsigned char) '(']=56,
	[(unsigned char) ')']=57,
	// //= 58,

	[(unsigned char) '[']=59,
	[(unsigned char) ']']=60,
	[(unsigned char) '<']=61,
	[(unsigned char) '>']=62,
	[(unsigned char) '{']=63,
	[(unsigned char) '}']=64,
	[(unsigned char) '&']=65,
	[(unsigned char) '§']=68,
	[(unsigned char) '$']=71,
	[(unsigned char) '=']=72,
	[(unsigned char) '+']=73,
	[(unsigned char) '×']=73,
	[(unsigned char) '*']=73,
	[(unsigned char) '#']=73,
	[(unsigned char) '~']=74,
};

const unsigned char cmpTable2[256]={
	// odsadenie pre znaky, ktore som zabudol
	[(unsigned char) 'A']=2,[(unsigned char) 'a']=2,
	[(unsigned char) 'Á']=3,[( unsigned char)'á']=3,
	[(unsigned char) 'B']=4,[(unsigned char) 'b']=4,
	[(unsigned char) 'C']=5,[(unsigned char) 'c']=5, 
	[(unsigned char) 'Æ']=6,[(unsigned char) 'è']=6,
	[(unsigned char) 'D']=7,[(unsigned char) 'd']=7, 
	[(unsigned char) 'Ï']=8,[(unsigned char) 'ï']=8,
	[(unsigned char) 'E']=9,[(unsigned char) 'e']=9,
	[(unsigned char) 'Ì']=10,[(unsigned char) 'ì']=10, 
	[(unsigned char) 'É']=11,[(unsigned char) 'é']=11,
	[(unsigned char) 'F']=12,[(unsigned char) 'f']=12,
	[(unsigned char) 'G']=13,[(unsigned char) 'g']=13,
	[(unsigned char) 'H']=14,[(unsigned char) 'h']=14,
	// ch = 15/16
	[(unsigned char) '%']=15,
	[(unsigned char) 'I']=17,[(unsigned char) 'i']=17,
	[(unsigned char) 'Í']=18,[(unsigned char) 'í']=18,
	[(unsigned char) 'J']=19,[(unsigned char) 'j']=19, 
	[(unsigned char) 'K']=20,[(unsigned char) 'k']=20,
	[(unsigned char) 'L']=21,[(unsigned char) 'l']=21, 
	[(unsigned char) 'M']=22,[(unsigned char) 'm']=22,
	[(unsigned char) 'N']=23,[(unsigned char) 'n']=23,
	[(unsigned char) 'Ò']=24,[(unsigned char) 'ò']=24,
	[(unsigned char) 'O']=25,[(unsigned char) 'o']=25,
	[(unsigned char) 'Ó']=26,[(unsigned char) 'ó']=26,
	[(unsigned char) 'P']=27,[(unsigned char) 'p']=27,
	[(unsigned char) 'Q']=28,[(unsigned char) 'q']=28,
	[(unsigned char) 'R']=29,[(unsigned char) 'r']=29,
	[(unsigned char) 'Ø']=30,[(unsigned char) 'ø']=30,
	[(unsigned char) 'S']=31,[(unsigned char) 's']=31,
	[(unsigned char) '©']=32,[(unsigned char) '¹']=32,
	[(unsigned char) 'T']=33,[(unsigned char) 't']=33,
	[(unsigned char) '«']=34,[(unsigned char) '»']=34,
	[(unsigned char) 'U']=35,[(unsigned char) 'u']=35,
	[(unsigned char) 'Ú']=36,[(unsigned char) 'ú']=36,
	[(unsigned char) 'Ù']=37,[(unsigned char) 'ù']=37,
	[(unsigned char) 'V']=38,[(unsigned char) 'v']=38,
	[(unsigned char) 'W']=39,[(unsigned char) 'w']=39, 
	[(unsigned char) 'X']=40,[(unsigned char) 'x']=40,
	[(unsigned char) 'Y']=41,[(unsigned char) 'y']=41,
	[(unsigned char) 'Ý']=42,[(unsigned char) 'ý']=42,
	[(unsigned char) 'Z']=43,[(unsigned char) 'z']=43,
	[(unsigned char) '®']=44,[(unsigned char) '¾']=44,
	[(unsigned char) '0']=45,
	[(unsigned char) '1']=46,
	[(unsigned char) '2']=47,
	[(unsigned char) '3']=48,
	[(unsigned char) '4']=49,
	[(unsigned char) '5']=50,
	[(unsigned char) '6']=51,
	[(unsigned char) '7']=52,
	[(unsigned char) '8']=53,
	[(unsigned char) '9']=54,
	[(unsigned char) '.']=55,
	[(unsigned char) ',']=56,
	[(unsigned char) ';']=57,
	[(unsigned char) '?']=58,
	[(unsigned char) '!']=59,
	[(unsigned char) ':']=60,
	[(unsigned char) '"']=61,
	[(unsigned char) '-']=62,
	[(unsigned char) '|']=63,
	[(unsigned char) '/']=64,
	[(unsigned char) '\\']=65,
	[(unsigned char) '(']=66,
	[(unsigned char) ')']=67,
	// //= 68,
	[(unsigned char) '[']=69,
	[(unsigned char) ']']=70,
	[(unsigned char) '<']=71,
	[(unsigned char) '>']=72,
	[(unsigned char) '{']=73,
	[(unsigned char) '}']=74,
	[(unsigned char) '&']=75,
	[(unsigned char) '§']=78,
	[(unsigned char) '$']=81,
	[(unsigned char) '=']=82,
	[(unsigned char) '+']=83,
	[(unsigned char) '×']=84,
	[(unsigned char) '*']=85,
	[(unsigned char) '#']=86,
	[(unsigned char) '~']=87,

};

typedef struct params
{
	int help;
	int error;
	int direction;
	char *directionColumn;
	char *directionText;
	int sort;
	char *printColumn;
	char *file1;
	char *file2;
} TParams;

typedef struct list
{
	int error;
	int numRows;
	char **words;
	int *charCounts;
} TList;

/**
 * errors
 * */
enum ERRORS
{
	E_OK = 0,		// Bez chyby
	E_BADPARAM,		// zle perametre
	E_MALLOC,		// malloc chyba
	E_BADFILE,		// chybny format vstupneho suboru
	E_FCLOSE,		// chyba pri zatvarani suboru
	E_EOF,			// koniec subora
	E_LINEBREAK, 	// line break
	E_UNKNOWNCOLUMN,// neznamy stlpec
};
/**
 * error messages
 * */
const char *ERRORSTEXT[] =
{
	// E_OK
	"Vsetko v poriadku.",
	// E_ZLEPARAM
	"Chybne parametre",
	// E_MALLOC
	"Chyba pri mallocovani pamate",
	// E_ZLYSUBOR
	"Chybny vstupny subor",
	// E_FCLOSE
	"Chyba pri zatvarani suboru",
	// E_EOF
	"Neocakavany koniec subora",
	// E_LINEBREAK
	"Neocakavany koniec riadka - zly format subora",
	// E_UNKNOWNCOLUMN
	"Stlpec sa nenasiel",
};

/**
 * direction 
 * */
enum DIRECTION
{
	NODIRECTION,
	AFTER,		
	BEFORE,	
};

/**
 * help message
 * */
const char *NAPOVEDATEXT =
	"Ceske radenie 4. projekt do IZP\nAutor: Branislav Blaskovic xblask00@stud.fit.vutbr.cz\n"
	"Pouzitie:\n"
	"\t--before stlpec retazec - vybere z tabulky polozky abecedne pred zadanym retazcom v stlpci (nepovinne)\n"
	"\t--after stlpec retazec - vybere z tabulky polozky abecedne za zadanym retazcom v stlpci (nepovinne)\n"
	"\t--print stlpec - urcuje, ktory stlpec sa ma ulozit do suboru (povinne)\n"
	"\t--sort - zotriedi vysledok podla abecedy (nepovinne)\n";

/**
 * prints error
 * */
void printError(int errorCode)
{
	if( errorCode != E_OK )
		fprintf(stderr,"Chyba: %s\n", ERRORSTEXT[errorCode]);
}
/**
 * prepare array of chars to my special array, in which array[0] is num of chars
 * */
char *prepareMyString( char *string )
{
	char *myString = NULL;
	int num = 0;
	
	if( string != NULL ) {
		while(1) {
			if( string[num] == '\0' ) break;
			num++;
		}
		num++;
	}
	
	myString = malloc( (num+1) * sizeof( int ) );
	if( myString != NULL ) {
		myString[0] = num;
		for( int i = 1; i < num; i++ ) {
			myString[i] = string[i-1];
		}
	}
	
	return myString;
}

/**
 * parameters
 * */
TParams getParams(int argc, char *argv[])
{
	
	TParams parameters =
	{
		.help = 0,
		.error = E_OK,
		.direction = NODIRECTION,
		.directionColumn = NULL,
		.directionText = NULL,
		.sort = 0,
		.printColumn = NULL,
		.file1 = "",
		.file2 = "",
	};
	
	// 1 = column, 2 = value
	int select = 0;
	// 1 = value
	int print = 0;
	
	if( argc == 9 || argc == 8 || argc == 6 || argc == 5 ) {
		
		for( int index = 1; index < argc; index++ ) {
			
			if( index == (argc-2) ) {
				parameters.file1 = argv[index];
			} else
			if( index == (argc-1) ) {
				parameters.file2 = argv[index];
			} else
			if( select == 1 ) {		// select column
				parameters.directionColumn = prepareMyString( argv[index] );
				select = 2;
			} else
			if( select == 2 ) {		// select value
				parameters.directionText = prepareMyString(argv[index])	;
				select = 0;
			} else
			if( print == 1 ) {
				parameters.printColumn = prepareMyString(argv[index]);
				print = 0;
			} else
			if( strcmp(argv[index], "--before") == 0 ) {
				parameters.direction = BEFORE;
				select = 1;
			} else
			if( strcmp(argv[index], "--after") == 0 ) {
				parameters.direction = AFTER;
				select = 1;
			} else
			if( strcmp(argv[index], "--sort") == 0 ) {
				parameters.sort = 1;
			} else
			if( strcmp(argv[index], "--print") == 0 ) {
				print = 1;
			}

		}
		
		if( strcmp( parameters.printColumn, "" ) == 0 )
			parameters.error = E_BADPARAM;
		
	} else
	if( argc == 2 && strcmp(argv[1],"-h")==0 ) {
		
		parameters.help = 1;
		
	} else {
		
		parameters.error = E_BADPARAM;
		
	}
	
	return parameters;

}
/**
 * read 1 word from file
 * */
char *readString( FILE *fileHandler, int *error )
{
	int returnE = E_OK;
	int lchar = ' ';
	int charCount = 1;
	int mallocSize = 0;
	char *arrayString = malloc( sizeof( int ) );
	char *newArray = NULL;
	
	if( arrayString == NULL ) returnE = E_MALLOC;
	
	// whitespaces
	while( isspace( lchar ) && lchar != EOF && returnE == E_OK ) {
		if( lchar == '\n' ) returnE = E_LINEBREAK;
		lchar = fgetc( fileHandler );
		if( lchar == EOF ) returnE = E_EOF;
	}
	
	// last char
	if( lchar != EOF && returnE == E_OK ) {
		arrayString[charCount] = lchar;
		charCount++;
	}
	
	// cycle for chars
	while( !isspace( lchar ) && lchar != EOF && returnE == E_OK ) {
		
		lchar = fgetc( fileHandler );
		if( lchar == '\n' ) returnE = E_LINEBREAK;
		if( lchar == EOF ) returnE = E_EOF;
		
		if( !isspace( lchar ) ) {
			
			if( (charCount+1) >= mallocSize ) {
				
				mallocSize = mallocSize + 10;

				newArray = realloc(arrayString, (mallocSize) * sizeof(int)); 
				if( newArray == NULL ) {
					free( arrayString );
					arrayString = NULL;
					returnE = E_MALLOC;
					break;
				} else {
					arrayString = newArray;
				}
			}
			
			
			
			if( returnE != E_MALLOC ) {
				arrayString[charCount] = lchar;
				charCount++;
			}
			
		}
		
	}
	
	// return variables
	if( arrayString != NULL ) {
		arrayString[0] = charCount;
	}
	
	*error = returnE;
	
	return arrayString;
}
/**
 * my custom function for comparing 2 strings
 * return 0 = different strings
 * return 1 = same strings
 * */
int myStrCmp( char *string1, char *string2 )
{
	
	if( string1[0] != string2[0] ) return 0;
	
	for( int i = 1; i < string1[0]; i++ ) {

		if( string2[i] == '\0' ) {
			return 0;
		}
		
		if( string1[i] != string2[i] ) {
			return 0;
		}
		
		
	}
	
	return 1;
}

/**
 * read head of table and find column indexes
 * */
int readColumn( FILE *fileHandler, int direction, char *directionColumn, char *printColumn, int *directionColumnIndex, int *printColumnIndex )
{
	int returnE = E_OK;
	int error = E_OK;
	int i = 0;
	char *stringC = NULL;
	
	while( error == E_OK && returnE == E_OK ) {
		stringC = readString( fileHandler, &error );

		if( stringC != NULL ) {
			
			// set column indexes
			if( direction != NODIRECTION ) {
				if( myStrCmp(stringC, directionColumn) == 1 ) *directionColumnIndex = i;
			}

			if( myStrCmp(stringC, printColumn) == 1 ) *printColumnIndex = i;
			
			free( stringC );
		}
		
		
		i++;
	}
	
	return returnE;
}
/**
 * my custom function for counting chars in word
 * */
int czStrLen( char *string )
{
	int num = 0;
	
	for( int i = 1; i < string[0]; i++ ) {
		
		// searching CH
		if( string[i] == 'c' || string[i] == 'C' ) {
			if( (i+1) != (string[0]) ) {
				if( string[i+1] == 'h' || string[i+1] == 'H' ) num--;
			}
		}
		
		num++;
		
	}
	
	return num;
}
/**
 * my custom function for copying string to new string
 * */
void myStrCopy( char *to, char *from, int count ) {
	for( int i = 1; i <= count; i++) {
		to[i] = from[i];
	}
	to[0] = count;
}
/**
 * replace 'ch' with '%'
 * */
char *repairCH( char *string )
{
	char *tempString1 = malloc( (string[0]+1) * sizeof( int ) );
	int charCount = 1;
	
	if( tempString1 != NULL ) {
		
		for( int i = 1; i < string[0]-1; i++) {
			tempString1[charCount] = string[i];
			if( string[i] == 'c' || string[i] == 'C' ) {
				if( string[i+1] == 'h' || string[i+1] == 'h' ) {
					i++;
					tempString1[charCount] = '%';
					
				}
			}
			charCount++;
		}
		tempString1[0] = charCount;
		
	}
	
	return tempString1;
}
/**
 * compares 2 strings
 * */
int czCompare(char *string1, char *string2)
{
	int weight1 = 0, weight2 = 0;
	
	int diff = 0;	
	int forCycles = 0;
	
	if( string1 == NULL ) return 1;
	if( string2 == NULL ) return -1;
	
	char *tempString1 = NULL, *tempString2 = NULL;
	
	tempString1 = repairCH( string1 );
	if( tempString1 == NULL ) return 1;
	
	tempString2 = repairCH( string2 );
	if( tempString2 == NULL ) {
		free( tempString1 );
		return -1;
	}
	
	if( tempString1[0] > tempString2[0] )
		forCycles = tempString2[0];
	else
		forCycles = tempString1[0];
	
	// if strings are the same
	if( myStrCmp( string1, string2 ) == 1 ) {
		free( tempString1 );
		free( tempString2 );
		return 0;
	}
	for( int i = 1; i < forCycles; i++ ) {
		
		weight1 = cmpTable1[(unsigned char)tempString1[i]];
		weight2 = cmpTable1[(unsigned char)tempString2[i]];
		
		if( weight1 > weight2 ) {
			diff = -1;
			break;
		} else
		if( weight1 < weight2 ) {
			diff = 1;
			break;
		}
	
	}
	
	if( diff == 0 && (tempString1[0] < tempString2[0]) ) diff = 1;
	if( diff == 0 && (tempString1[0] > tempString2[0]) ) diff = -1;
	
	if( diff == 0 ) {

		for( int i = 1; i < forCycles; i++ ) {
			
			weight1 = cmpTable2[(unsigned char)tempString1[i]];
			weight2 = cmpTable2[(unsigned char)tempString2[i]];
			
			if( weight1 > weight2 ) {
				diff = -1;
				break;
			} else
			if( weight1 < weight2 ) {
				diff = 1;
				break;
			}
			
			
		}
	}
	
	if( diff == 0 && (tempString1[0] < tempString2[0]) ) diff = 1;
	if( diff == 0 && (tempString1[0] > tempString2[0]) ) diff = -1;
	
	free( tempString1 );
	free( tempString2 );
	
	return diff;
}
/**
 * frees array
 * */
void freeArray( char **array, int numRows )
{
	for( int i = 0; i < numRows; i++ ) {
		free( array[i] );
	}
	free( array );
}
/**
 * fills array with words, which user want to print
 * */
char **readLines( FILE *fileHandler, int *chyba, int *numRows, int direction, char *directionText, int directionColumnIndex, int printColumnIndex )
{
	int returnE = E_OK, error = E_OK, i = 0;
	char *stringC = NULL;
	int tempMallocError = 0;
	char *tempDirection;
	TList lineList;
	
	int reallocCount = 0;
	
	// data
	char **lines = NULL;
	char **reLines = NULL;
	char *printWord = NULL;
	
	lineList.error = E_OK;
	lineList.numRows = 0;
	lineList.words = NULL;
	lineList.charCounts = NULL;
	
	tempDirection = (char *) malloc(sizeof(int));
	
	if( tempDirection == NULL ) {
		returnE = E_MALLOC;
		tempMallocError = 1;
	}
	
	if( printColumnIndex == -1 || ( direction != NODIRECTION && directionColumnIndex == -1 ) ) {
		returnE = E_UNKNOWNCOLUMN;
	}	
	
	while( error != E_EOF && returnE == E_OK ) {
		
		
		if( error == E_LINEBREAK ) {
			i = 0;

			if( (czCompare( tempDirection, directionText ) == -1 && direction == AFTER) || (czCompare( tempDirection, directionText ) == 1 && direction == BEFORE ) ||	direction == NODIRECTION ) {
				
				// realloc lines array for new line
				if( lineList.numRows >= reallocCount ) {
					reallocCount += 20;
					reLines = realloc(lines, reallocCount * sizeof(char*)); 
				}
				if( reLines == NULL ) {
					
					freeArray( lines, lineList.numRows );
					lines = NULL;
					returnE = E_MALLOC;
					free( printWord );
					printWord = NULL;
					break;
					
				} else {
					
					lines = reLines;
					
					lines[lineList.numRows] = malloc( (printWord[0]+1) * sizeof( int ));
					
					myStrCopy(lines[lineList.numRows], printWord, printWord[0]);
					
					free( printWord );
					printWord = NULL;
					
					lineList.numRows++;
				}	
				

			} else {
				if( printWord != NULL ) {
					free( printWord );
					printWord = NULL;
				}
			}
			
		}
		
		stringC = readString( fileHandler, &error );

		if( directionColumnIndex == i ) {
			free( tempDirection );
			tempDirection = (char *) malloc( (stringC[0]+1) * sizeof( int ));
			if( tempDirection == NULL ) {
				if( printWord != NULL ) {
					free( printWord );
					printWord = NULL;
				}

				returnE = E_MALLOC;
				tempMallocError = 1;
				break;
			} else
				myStrCopy(tempDirection, stringC, stringC[0] );
		}
		
		if( printColumnIndex == i ) {
			
			printWord = (char *) malloc( (stringC[0]+1) * sizeof( int ));
			
			if( printWord == NULL ) {
				
				free( tempDirection );
				tempDirection = NULL;
				returnE = E_MALLOC;
				tempMallocError = 1;
				break;
				
			} else {

				myStrCopy(printWord, stringC, stringC[0] );

			}
			
		}
		
		if( stringC != NULL ) free( stringC );
		
		
		i++;

	}
	
	
	// free zone
	if( tempDirection != NULL ) free( tempDirection );
	if( printWord != NULL ) free( printWord );
	
	
	// return
	*numRows = lineList.numRows;

	*chyba = returnE;
	return lines;
}
/**
 * sorts array
 * */
char **sortArray( char **array, int numRows )
{
	char *temp = NULL;
	
	for (int i = 0; i < numRows; i++) {

		for (int j = numRows-1; j > i; j--) {
			
			if ( czCompare(array[j-1], array[j]) == -1 ) {
				
				temp = array[j-1];
				array[j-1] = array[j];
				array[j] = temp;
				
			}
			
		}
	}	
	
	return array;
}
/**
 * print array values to file
 * */
void printArray( FILE *fileHandler, char **array, int numRows ) {
	for( int i = 0; i < numRows; i++ ) {
		for( int a = 1; a <	 array[i][0]; a++ ) {
			fputc( array[i][a], fileHandler );
		}
		fprintf(fileHandler, "\n");
	}
}

/**
 * main function
 * */
int main(int argc, char *argv[])
{
	TParams parameters = getParams(argc, argv);
	FILE *file1;
	FILE *file2;
	int chyba = E_OK;
	int directionColumnIndex = -1, printColumnIndex = -1;

	// help
	if( parameters.help == 1) {
		printf("%s", NAPOVEDATEXT);
		return EXIT_SUCCESS;
	}
	
	// was error
	if( parameters.error != E_OK ) {
		printError(parameters.error);
		return EXIT_FAILURE;
	}
	
	// open file to read
	file1 = fopen( parameters.file1, "r" );
	if( file1 == NULL ) parameters.error = E_BADFILE;
	
	// open file to write
	if( parameters.error == E_OK ) {
		file2 = fopen( parameters.file2, "w" );
		if( file2 == NULL ) parameters.error = E_BADFILE;
	}	
	
	// MAIN PART
	if( parameters.error == E_OK ) {
		
		// read header
		chyba = readColumn(file1, parameters.direction, parameters.directionColumn, parameters.printColumn, &directionColumnIndex, &printColumnIndex );

		// read lines
		if( chyba == E_OK ) {
			
			int numRows;
			char **lineArray = NULL;
			
			lineArray = readLines( file1, &chyba, &numRows, parameters.direction, parameters.directionText, directionColumnIndex, printColumnIndex);
			
			if( chyba == E_OK ) {
				
				if( parameters.sort == 1 ) {
					lineArray = sortArray( lineArray, numRows );
				}
				
				printArray( file2, lineArray, numRows );
				freeArray( lineArray, numRows );
			}
			
			printError( chyba );

		}
		
		if( fclose(file1)==EOF ) {
			parameters.error = E_FCLOSE;
		}
		if( fclose(file2)==EOF ) {
			parameters.error = E_FCLOSE;
		}
		
		printError( parameters.error );
		
	} else {
		printError( parameters.error );
	}
	
	// free zone
	if( parameters.directionColumn != NULL ) free( parameters.directionColumn );
	if( parameters.directionText != NULL ) free( parameters.directionText );
	if( parameters.printColumn != NULL ) free( parameters.printColumn );

	
	return parameters.error == E_OK ? EXIT_SUCCESS : EXIT_FAILURE;

}
