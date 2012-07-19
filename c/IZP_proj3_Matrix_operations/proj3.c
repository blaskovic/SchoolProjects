/*
 * Subor:
 * Datum:	december 2010
 * Autor:	Branislav Blaskovic xblask00@stud.fit.vutbr.cz
 * Projekt:	Maticove operacie, projekt 3 pre predmet IZP
 * Popis:
 */

/**
 * Kniznice
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define TRUE 1
#define FALSE 0

typedef struct params
{
	int error;
	int status;
	char *text1;
	char *text2;
} TParams;

typedef struct subor
{
	FILE *pointer;
	char *nazovSuboru;
	int chyba;
} TSubor;

typedef struct matrix
{
	int typ;
	int riadkov;
	int stlpcov;
	int **matica;
	int chyba;
	int pocetMatic;
} TMatrix;

/**
 * chyby
 * */
enum ERRORS
{
	EOK = 0,		// Bez chyby
	EZLEPARAM,		// zle perametre
	EMALLOC,		// malloc chyba
	EZLYSUBOR,		// chybny format vstupneho suboru
	EFCLOSE,		// chyba pri zatvarani suboru
	EROZMERY,		// chybne rozmery
	EHODNOTY,		// chybne hodnoty
};
/**
 * texty k chybam
 * */
const char *ERRORSTEXT[] =
{
	// EOK
	"Vsetko v poriadku.",
	// EZLEPARAM
	"Chybne parametre",
	// EMALLOC
	"Chyba pri mallocovani pamate",
	// EZLYSUBOR
	"Chybny vstupny subor",
	// EFCLOSE
	"Chyba pri zatvarani suboru",
	// EROZMERY
	"Chybne rozmery",
	// EHODNOTY
	"Chybne hodnoty"
};

/**
 * status
 * */
enum STATUS
{
	STATUSNAPOVEDA,			// help
	STATUSTEST,				// test
	STATUSVADD,				// vadd
	STATUSVSCAL,			// vscal
	STATUSMMULT,			// vscal
	STATUSMEXPR,			// mexpr
	STATUSEIGHT,			// eight
	STATUSBUBBLES,			// bubbles
	STATUSEXTBUBBLES,		// 3d bubbles
};

/**
 * text k napovede
 * */
const char *NAPOVEDATEXT =
	"Program:\n\tMaticove operacie\n"
	"Autor:\n\tBranislav Blaskovic xblask00\n"
	"\txblask@stud.fit.vutbr.cz\n"
	"Pouzitie:\n"
	"\t -h - vypise napovedu\n"
	"\t --test data.txt - otestuje vstupny subor a spravne ho vypise\n"
	"\t --vadd a.txt b.txt - scita dva vektory\n"
	"\t --vscal a.txt b.txt - spravi skalarny sucin dvoch vektorov\n"
	"\t --mmult A.txt B.txt - vynasobi dve matice\n"
	"\t --mexpr A.txt B.txt - vypocita vyraz A*B*A\n"
	"\t --eight v.txt M.txt - vyhlada v matici M vektor v\n"
	"\t --bubbles M.txt - spocita pocet bublin v matici M\n"
	"\t --extbubbles 3DM.txt - spocita pocet bublin v 3D matici\n";

/**
 * Vypise chybu podla kodu
 * */
void zobrazChybu(int kodChyby)
{
	fprintf(stderr,"Chyba: %s\n", ERRORSTEXT[kodChyby]);
}

/**
 * ziska parametre z Main() a vyhodnocuje ich
 * */
TParams ziskajParametre(int argc, char *argv[])
{
	
	TParams vysledok =
	{
		.error = EOK,
		.status = STATUSNAPOVEDA,
	};
	
	if( argc == 2 && strcmp(argv[1],"-h") == 0 ) {
		
		vysledok.status = STATUSNAPOVEDA;
		
	} else if( argc == 3 && strcmp(argv[1],"--test") == 0 ) {
		
		vysledok.status = STATUSTEST;
		vysledok.text1 = argv[2];
	
	} else if( argc == 4 && strcmp(argv[1],"--vadd") == 0 ) {
		
		vysledok.status = STATUSVADD;
		vysledok.text1 = argv[2];
		vysledok.text2 = argv[3];
		
		
	} else if( argc == 4 && strcmp(argv[1],"--mmult") == 0 ) {
		
		vysledok.status = STATUSMMULT;
		vysledok.text1 = argv[2];
		vysledok.text2 = argv[3];
		
		
	} else if( argc == 4 && strcmp(argv[1],"--vscal") == 0 ) {
		
		vysledok.status = STATUSVSCAL;
		vysledok.text1 = argv[2];
		vysledok.text2 = argv[3];
		
	} else if( argc == 4 && strcmp(argv[1],"--mexpr") == 0 ) {
		
		vysledok.status = STATUSMEXPR;
		vysledok.text1 = argv[2];
		vysledok.text2 = argv[3];
		
	} else if( argc == 3 && strcmp(argv[1],"--bubbles") == 0 ) {
		
		vysledok.status = STATUSBUBBLES;
		vysledok.text1 = argv[2]; // matica
		
	} else if( argc == 3 && strcmp(argv[1],"--extbubbles") == 0 ) {
		
		vysledok.status = STATUSEXTBUBBLES;
		vysledok.text1 = argv[2]; // matica
		
	}
	 else if( argc == 4 && strcmp(argv[1],"--eight") == 0 ) {
		
		vysledok.status = STATUSEIGHT;
		vysledok.text1 = argv[2]; // vektor
		vysledok.text2 = argv[3]; // matica
		
	} else {
		vysledok.error = EZLEPARAM;
	}

	return vysledok;

}
/**
 * uvolni maticu, ktora sa nepodarila allokovat v celku
 * */
void freeCustomMatrix( TMatrix *matica, int riadok )
{
	riadok--;
	for( int r = riadok; r >= 0; r-- ) {
		free( matica->matica[r] );
	}
	free( matica->matica );
}
/**
 * allokuje maticu podla rozmerov
 * */
void allocMatrix( TMatrix *matica )
{
	int r,s;
	matica->matica = malloc( (matica->riadkov+1)*matica->pocetMatic * sizeof( int* ) ); // riadky
	if( matica->matica != NULL ) {
	
		for( r = 0; r < matica->riadkov*matica->pocetMatic; r++ ) {
		
			matica->matica[r] = malloc( (matica->stlpcov+1) * sizeof( int ) );
			if( matica->matica[r] != NULL) {
				for( s = 0; s < matica->stlpcov; s++) matica->matica[r][s] = 0;
			} else {
				freeCustomMatrix( matica, r );
				matica->chyba = EMALLOC;
			}
			
		}
		
	} else {
		matica->chyba = EMALLOC;
	}

}
/**
 * uvolni celu maticu
 * */
void freeMatrix( TMatrix *m )
{
	for(int i = 0; i < m->riadkov; i++) {
		free(m->matica[i]);
	}
	free(m->matica);
}
/**
 * nacita zdrojovy subor do struktury a osetri chyby
 * */
TMatrix nacitajMaticu( FILE *handler )
{
	int hodnota;
	TMatrix matica;
	matica.pocetMatic = 1;
	matica.chyba = EOK;
	matica.matica = NULL;
	int pocet;
	
	if( handler ) {
		
		// typ
		pocet=fscanf(handler, "%d", &hodnota);
		
		if( pocet == 0 ) {
			matica.chyba = EROZMERY;
		} else {
			matica.typ = hodnota;
		}
		
		if( matica.chyba == EOK ) {
			
			if( matica.typ == 1 ) {
				
				pocet=fscanf(handler, "%d", &hodnota);
				if( pocet == 0 ) {
					matica.chyba = EROZMERY;
				} else {
					matica.riadkov = hodnota;
					matica.stlpcov = 0;
				}
				
				
				
				
			} else
			if( matica.typ == 2 ) {
				pocet=fscanf(handler, "%d", &hodnota);
				if( pocet == 0 ) {
					matica.chyba = EROZMERY;
				} else {
					matica.riadkov = hodnota;
				}
				pocet=fscanf(handler, "%d", &hodnota);
				if( pocet == 0 ) {
					matica.chyba = EROZMERY;
				} else {
					matica.stlpcov = hodnota;
				}
			} else
			if( matica.typ == 3 ) {
				pocet=fscanf(handler, "%d", &hodnota);
				if( pocet == 0 ) {
					matica.chyba = EROZMERY;
				} else {
					matica.pocetMatic = hodnota;
				}
				pocet=fscanf(handler, "%d", &hodnota);
				if( pocet == 0 ) {
					matica.chyba = EROZMERY;
				} else {
					matica.riadkov = hodnota;
				}
				pocet=fscanf(handler, "%d", &hodnota);
				if( pocet == 0 ) {
					matica.chyba = EROZMERY;
				} else {
					matica.stlpcov = hodnota;
				}
				
				
			} else {
				matica.chyba = EROZMERY;
			}
			
			if( matica.chyba == EOK ) {
				allocMatrix( &matica );
			}
			
			// nacitavanie
			if( matica.chyba == EOK && matica.matica != NULL ) {
							
				for( int r = 0; r < (matica.riadkov * matica.pocetMatic); r++) {

					for( int s = 0; s < matica.stlpcov || (matica.typ==1 && s==0); s++ ) {
						
						pocet = fscanf(handler, "%d", &hodnota);
						
						if( pocet == 1 ) {
							matica.matica[r][s] = hodnota;
						} else {
							matica.chyba = EHODNOTY;
						}
						
					}	
				}
				
			}
			
			
		}
	}
		
	
	return matica;
	
}
/**
 * otvara subor a overuje jeho otvorenie
 * */
TSubor otvorSubor(char *nazovSuboru)
{
	TSubor textak;
	FILE *handler;

	handler = fopen (nazovSuboru,"r");
	
	if( handler != NULL ) {
		
		textak.chyba = EOK;
		textak.pointer = handler;
		textak.nazovSuboru = nazovSuboru;
	
	} else {
	
		textak.chyba = EZLYSUBOR;
		
	}
	return textak;
}
/**
 * vypisuje maticu/vektor
 * */
void vypisMaticu( TMatrix *matica )
{
	int nasobokRiadkov = 0; // na oddelovanie matic
	
	if( matica->typ == 1 ) {
		
		printf("%3d\n%3d\n", matica->typ, matica->riadkov);

		for( int r = 0; r < matica->riadkov; r++ ) {
			printf("%3d", matica->matica[r][0]);
		}
		
	} else
	if( matica->typ == 2 ) {
		printf("%3d\n%3d %3d\n", matica->typ, matica->riadkov, matica->stlpcov);
		
		for( int r = 0; r < matica->riadkov; r++ ) {
			
			for( int s = 0; s < matica->stlpcov; s++ ) {
				
				printf("%3d ", matica->matica[r][s]);
				
			}
			printf("\n");
			
		}
	} else
	if( matica->typ == 3 ) {
		
		printf("%3d\n%3d %3d %3d\n", matica->typ, matica->pocetMatic, matica->riadkov, matica->stlpcov);
		
		for( int r = 0; r < matica->riadkov*matica->pocetMatic; r++ ) {
		
			for( int s = 0; s < matica->stlpcov; s++ ) {
				
				printf("%3d ", matica->matica[r][s]);
				
			}
			printf("\n");
			
			// oddelenie matic
			nasobokRiadkov++;
			
			if( nasobokRiadkov == matica->riadkov ) {
				printf("\n");
				nasobokRiadkov = 0;
			}
			
		}

		
	}
}
/**
 * nasobi dve matice a uklada do tretej
 * */
void sucinMatic( TMatrix *matica1, TMatrix *matica2, TMatrix *matica3 )
{
	for( int r3 = 0; r3 < matica3->riadkov; r3++ ) {
				
		for( int s3 = 0; s3 < matica3->stlpcov; s3++ ) {
		
			for( int s = 0; s < matica1->stlpcov; s++ ) {
				
				matica3->matica[r3][s3] += matica1->matica[r3][s] * matica2->matica[s][s3];

			}
		
		}
			
		
	}
}

/**
 * overuje a pripravuje smery pre osemsmerovku
 * */
int overSmery( TMatrix *matica, int *indexR, int *indexS, int smer, int posun )
{
	/**
	 *
	 * sever 			0
	 * severo-vychod 	1
	 * vychod 			2
	 * juho-vychod		3
	 * juh 				4
	 * juho-zapad 		5
	 * zapad 			6
	 * sever-zapad 		7	 	 	 	 
	 *
	 * */	 	 	
	switch( smer ) {
		case 0:
			*indexR = *indexR - posun;
			break;
		case 1:
			*indexR = *indexR - posun;
			*indexS = *indexS + posun;
			break;
		case 2:
			*indexS = *indexS + posun;
			break;
		case 3:
			*indexR = *indexR + posun;
			*indexS = *indexS + posun;
			break;
		case 4:
			*indexR = *indexR + posun;
			break;
		case 5:
			*indexR = *indexR + posun;
			*indexS = *indexS - posun;
			break;
		case 6:
			*indexS = *indexS - posun;
			break;
		case 7:
			*indexR = *indexR - posun;
			*indexS = *indexS - posun;
			break;
	}
	if( ( *indexR >= 0 ) && ( *indexR < matica->riadkov ) && ( *indexS >= 0 ) && ( *indexS < matica->stlpcov ) ) {
		return TRUE;
	} else {
		return FALSE;
	}
	
}
/**
 * prehladava okolie cisla v osemsmerovke
 * */
int prehladajOkolie( TMatrix *vektor, TMatrix *matica, int indexR, int indexS )
{
	int indexVektor = 1;
	int pomR, pomS;
	
	for( int smer = 0; smer < 8; smer++ ) {
		for( int i = 1; i <= vektor->riadkov; i++ ) {
			
			// najdeny cely vektor
			if( indexVektor == vektor->riadkov ) return TRUE;
			
			// hladaj
			pomR = indexR;
			pomS = indexS;
			if( overSmery( matica, &pomR, &pomS, smer, i ) == 1 ) {
				if( matica->matica[pomR][pomS] == vektor->matica[indexVektor][0] ) {
					indexVektor++;
				} else {
					indexVektor = 1;
				}
				
			} else {
				indexVektor = 1;
			}
			
		}
	}
	return FALSE;

}
/**
 * hlada v okoli nuly dalsie nuly (bubliny)
 * */
void hladajBublinu( TMatrix *matica, int r, int s, int poschodie )
{
	// sever
	if( r-1 >= 0 ) {
		if( matica->matica[r-1 + poschodie * matica->riadkov][s] == 0) {
			matica->matica[r-1 + poschodie * matica->riadkov][s] = -1;
			hladajBublinu( matica, r-1, s, poschodie );
		}
	}
	// vychod
	if( s+1 < matica->stlpcov ) {
		if( matica->matica[r + poschodie * matica->riadkov][s+1] == 0) {
			matica->matica[r + poschodie * matica->riadkov][s+1] = -1;
			hladajBublinu( matica, r, s+1, poschodie );
		}
	}
	// juh
	if( r+1 < matica->riadkov ) {
		if( matica->matica[r+1 + poschodie * matica->riadkov][s] == 0) {
			matica->matica[r+1 + poschodie * matica->riadkov][s] = -1;
			hladajBublinu( matica, r+1, s, poschodie );
		}
	}
	// zapad
	if( s-1 >= 0 ) {
		if( matica->matica[r + poschodie * matica->riadkov][s-1] == 0) {
			matica->matica[r + poschodie * matica->riadkov][s-1] = -1;
			hladajBublinu( matica, r, s-1, poschodie );
		}
	}
	
	// minus
	if( poschodie-1 >= 0 ) {
		if( matica->matica[r + (poschodie-1) * matica->riadkov][s] == 0) {
			matica->matica[r + (poschodie-1) * matica->riadkov][s] = -1;
			hladajBublinu( matica, r, s, poschodie-1 );
		}
	}
	// plus
	if( poschodie+1 < matica->pocetMatic ) {
		if( matica->matica[r + (poschodie+1) * matica->riadkov][s] == 0) {
			matica->matica[r + (poschodie+1) * matica->riadkov][s] = -1;
			hladajBublinu( matica, r, s, poschodie+1 );
		}
	}
}
/**
 * pripravi pomocnu maticu na pouzitie
 * */
int pripravitMaticu( TMatrix *matica )
{
	allocMatrix(matica);
	
	if( matica->matica == NULL )
		return EMALLOC;
	else
		return EOK;
}
/**
 * hlavny program pre hladanie bublin
 * */
int programBubbles( TMatrix *matica, int status )
{

	int bublin = 0;
	int poschodie;
	
	// overenia
	if( matica->typ == 1 ) return EZLYSUBOR;
	if( status == STATUSBUBBLES && matica->typ != 2 ) return EZLYSUBOR;
	
	if( matica->riadkov > 0 && matica->stlpcov > 0 ) {
		
		// prechod po matici
		for( poschodie = 0; poschodie < matica->pocetMatic; poschodie++ ) {
			for( int r = 0; r < matica->riadkov; r++ ) {
				
				for( int s = 0; s < matica->stlpcov; s++ ) {
					
					if(matica->matica[r + poschodie * matica->riadkov][s] < 0) {
						printf("false");
						return EOK;
					}
					
					if( matica->matica[r + poschodie * matica->riadkov][s] == 0 ) {
						matica->matica[r + poschodie * matica->riadkov][s] = -1;
						bublin++;
						hladajBublinu( matica, r, s, poschodie );
					}
					
				}
				
			}
		}
		printf("%d\n", bublin);
		
	}
	
	return EOK;
}
/**
 * hlavny program pre osemsmerovku
 * */
int programEight( TMatrix *vektor, TMatrix *matica )
{
	
	int zhoda = 0;
	
	if( vektor->typ == 1 && matica->typ == 2 ) {
		
		// zacina osemsmerovka
		for( int r = 0; r < matica->riadkov; r++ ) {
		
			if( zhoda == TRUE ) break;
			
			for( int s = 0; s < matica->stlpcov; s++ ) {
			
				if( zhoda == TRUE ) break;
			
				if( matica->matica[r][s] == vektor->matica[0][0] ) {
					
					// zhoda
					zhoda = prehladajOkolie( vektor, matica, r, s );
					if( zhoda == TRUE ) {
						printf("true\n");
						break;
					}
				
				}
				
			}
		}
		if( zhoda != TRUE ) printf("false\n");

	} else {
		return EZLYSUBOR;
	}
	
	
	return EOK;
}
/**
 * hlavny program pre nasobenie matic
 * */
int programMmult( TMatrix *matica1, TMatrix *matica2, TMatrix *matica3 )
{
	int navrat = EOK;

	matica3->typ = 2;
	matica3->riadkov = matica1->riadkov;
	matica3->stlpcov = matica2->stlpcov;
	matica3->pocetMatic = 1;
	
	navrat = pripravitMaticu( matica3 );
	
	if( matica1->stlpcov == matica2->riadkov && matica1->typ == 2 && matica2->typ == 2 && navrat == EOK ) {

		sucinMatic( matica1, matica2, matica3 );
		
	} else {
		printf("false");
	}
	
	return navrat;
}
/**
 * nasobi matice podla predpisu A*B*A
 * */
int programMexpr( TMatrix *matica1, TMatrix *matica2, TMatrix *matica3 )
{
	int navrat = EOK;
	
	if( matica1->stlpcov == matica2->riadkov && matica1->riadkov == matica2->stlpcov && matica1->typ == 2 && matica2->typ == 2 ) {
		
		navrat = programMmult( matica1, matica2, matica3 );
		
		if( navrat == EOK ) 
			navrat = programMmult( matica3, matica1, matica2 );
		if( navrat == EOK ) {
			matica3->riadkov = matica2->riadkov;
			matica3->stlpcov = matica2->stlpcov;
			matica3->matica = matica2->matica;
		}
		
	} else {
		printf("false");
	}
		
	return navrat;
}
/**
 * skalarne nasobi vektory
 * */
int programVscal( TMatrix *matica1, TMatrix *matica2 )
{
	int navrat = EOK;
	int sucin = 0;

	
	if( matica1->riadkov == matica2->riadkov && matica1->typ == 1 && matica2->typ == 1 ) {
		
		for( int r = 0; r < matica1->riadkov; r++ )
			sucin += matica1->matica[r][0] * matica2->matica[r][0];

		printf("%d", sucin);
		
	} else {
		printf("false");
	}
	
	return navrat;
}
/**
 * scita vektory
 * */
int programVadd( TMatrix *matica1, TMatrix *matica2, TMatrix *matica3 )
{
	int navrat = EOK;
	matica1 = matica1;
	matica2 = matica2;
	
	matica3->typ = 1;
	matica3->pocetMatic = 1;
	matica3->riadkov = matica2->riadkov;
	matica3->stlpcov = 0;
	
	navrat = pripravitMaticu( matica3 );
	
	if( matica1->riadkov == matica2->riadkov && matica1->typ == 1 && matica2->typ == 1 && navrat == EOK ) {

		for( int r = 0; r < matica1->riadkov; r++ ) {
			matica3->matica[r][0] = matica1->matica[r][0] + matica2->matica[r][0];	
		}
		
		vypisMaticu( matica3 );

	} else {
		printf("false");
	}
	
	return navrat;
}
/**
 * otestuje zadany subor a vypise ho v korektnem formatovani
 * */
int programTest( TMatrix *matica )
{
	int navrat = EOK;
	
	// vypis suboru
	if( matica->typ >= 1 && matica->typ <= 3 && matica->chyba == EOK ) {
		vypisMaticu( matica );
	} else { 
		matica->chyba = EZLYSUBOR;
	}
	
	return navrat;
}
/**
 * hlavny program
 * */
int main(int argc, char *argv[])
{
		
	TParams hodnoty = ziskajParametre(argc, argv);
	int chyba = EOK;
	
	TSubor subor1, subor2;
	TMatrix matica1, matica2, matica3;
	matica1.chyba = EOK;
	matica2.chyba = EOK;
	matica3.chyba = EOK;
	
	// ak mame vstupny subor
	if( argc >= 3 && hodnoty.error == EOK ) {
		
		subor1 = otvorSubor(hodnoty.text1);

		if( subor1.chyba == EOK ) {
		
			matica1 = nacitajMaticu(subor1.pointer);
			hodnoty.error = matica1.chyba;
			
			
			if( fclose(subor1.pointer)==EOF ) {
				hodnoty.error = EFCLOSE;
			}
		} else {
			hodnoty.error = subor1.chyba;
		}
	}
	
	// ak mame aj druhy subor
	if( argc == 4 && hodnoty.error == EOK ) {
		
		subor2 = otvorSubor(hodnoty.text2);
		
		if( subor2.chyba == EOK ) {
		
			matica2 = nacitajMaticu(subor2.pointer);
			
			hodnoty.error = matica1.chyba;
			
			if( fclose(subor2.pointer)==EOF ) {
				hodnoty.error = EFCLOSE;
			}
		} else {
			hodnoty.error = subor2.chyba;
		}
	}
	
	if( hodnoty.error == EOK ) {
		switch( hodnoty.status ) {
			
			case STATUSNAPOVEDA:
				printf("%s", NAPOVEDATEXT);
				break;
			case STATUSTEST:
				chyba = programTest(&matica1);
				break;
			case STATUSVADD:
				chyba = programVadd(&matica1, &matica2, &matica3);
				if( chyba==EOK ) freeMatrix( &matica3 );
				break;
			case STATUSVSCAL:
				chyba = programVscal(&matica1, &matica2);
				break;
			case STATUSMMULT:
				chyba = programMmult(&matica1, &matica2, &matica3);
				vypisMaticu( &matica3 );
				if( chyba==EOK ) freeMatrix( &matica3 );
				break;
			case STATUSMEXPR:
				chyba = programMexpr(&matica1, &matica2, &matica3 );
				vypisMaticu( &matica3 );
				break;
			case STATUSEIGHT:
				chyba = programEight(&matica1, &matica2);
				break;
			case STATUSBUBBLES:
				chyba = programBubbles(&matica1, hodnoty.status);
				break;
			case STATUSEXTBUBBLES:
				chyba = programBubbles(&matica1, hodnoty.status);
				break;
			default:
				return EXIT_FAILURE;
				break;
		}
		
		// chyba za behu 
		if( chyba != EOK ) zobrazChybu( chyba );
		
	} else {
		if( hodnoty.error != EOK ) zobrazChybu( hodnoty.error );
	}
	
	// uvolnenie matice
	if( argc >= 3 ) {
		if( matica1.chyba != EMALLOC && subor1.chyba == EOK ) {
			freeMatrix( &matica1 );
		}
	}
	if( argc == 4 ) {
		if( matica2.chyba != EMALLOC && subor1.chyba == EOK && subor2.chyba == EOK ) {
			freeMatrix( &matica2 );
		}
	}
	
	return hodnoty.error == EOK ? EXIT_SUCCESS : EXIT_FAILURE;

}
