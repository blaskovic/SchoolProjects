/*
 * Subor:
 * Datum:	november 2010
 * Autor:	Branislav Blaskovic xblask00@stud.fit.vutbr.cz
 * Projekt:	Iteracne vypocty, projekt 2 pre predmet IZP
 * Popis:
 */

/**
 * Kniznice
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>
#include <math.h>

/**
 * konstanty potrebne pre vypocet
 * */  
#define LN10 2.3025850929940456840179914546843642076011014886287729760333279
/**
 * Struktura na zachytavanie parametrov, chyb, statusu
 */
typedef struct params
{
	int error;
	int status;
	int sigdig;
	double a;
} TParams;
/**
 * Struktura pre predavanie parametrov pri vypocte priemerov
 **/ 
typedef struct Wam
{
	double hodnota;
	double vaha;
	double citatel;
	double menovatel;
	int prepinac;
} TWam;

/**
 * chyby
 * */
enum ERRORS
{
	EOK = 0,		// Bez chyby
	EZLEPARAM,		// zle perametre
	EMALLOC,		// malloc chyba
	EZAKLADLOG,		// chybny zaklad logaritmu
	ESIGDIG			// zly sigdig
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
	// EZAKLADLOG
	"Chybny zaklad logaritmu",
	// ESIGDIG
	"Chybny sigdig"
};

/**
 * status
 * */
enum STATUS
{
	STATUSNAPOVEDA,			// help
	STATUSTANH,				// hyperbolicky tangens
	STATUSLOGAX,			// logaritmus
	STATUSWAM,				// vazeny aritmeticky priemer
	STATUSWQM,				// vazeny kvadraticky priemer
};

/**
 * text k napovede
 * */
const char *NAPOVEDATEXT =
	"Program:\n\tIteracne vypocty\n"
	"Autor:\n\tBranislav Blaskovic xblask00\n"
	"\txblask@stud.fit.vutbr.cz\n"
	"Pouzitie:\n"
	"\tproj2 -h - zobrazi napovedu \n"
	"\tproj2 --tanh sigdig - nacita zo stdin cisla a tlaci ich tanh() hodnotu na zadanu presnost sigdig\n"
	"\tproj2 -- logax sigdig a - nacita zo stdin cisla a tlaci ich logaritmus na zadanu presnost sigdig pri zaklade a\n"
	"\tproj2 --wam - nacita zo stdin cisla a priebezne tlaci hodnoty vazeneho aritmetickeho priemeru\n"
	"\tproj2 --wqm - nacita zo stdin cisla a priebezne tlaci hodnoty vazeneho kvadratickeho priemeru\n";

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
	char *endptr;
	
	TParams vysledok =
	{
		.error = EOK,
		.status = STATUSNAPOVEDA,
		.sigdig = 0.0,
		.a = 0,
	};
	
	if(argc == 2 && strcmp(argv[1],"-h") == 0) {
	
		vysledok.status = STATUSNAPOVEDA;
		
	} else if(argc == 2 && strcmp(argv[1],"--wam") == 0) {
	
		vysledok.status = STATUSWAM;
		
	} else if(argc == 2 && strcmp(argv[1],"--wqm") == 0) {
	
		vysledok.status = STATUSWQM;
		
	} else if(argc == 3 && strcmp(argv[1],"--tanh") == 0) {
		
		vysledok.status = STATUSTANH;
		vysledok.sigdig = atoi(argv[2]);
		if( vysledok.sigdig < 1 || vysledok.sigdig > 16 ) {
			vysledok.error = ESIGDIG;
		}
		
	} else if(argc == 4 && strcmp(argv[1],"--logax") == 0) {
		
		vysledok.status = STATUSLOGAX;
		vysledok.sigdig = atoi(argv[2]);
		if( vysledok.sigdig < 1 || vysledok.sigdig > 16 ) {
			vysledok.error = ESIGDIG;
		}
		
		vysledok.a = strtod (argv[3], &endptr);
		if (endptr == argv[3] || *endptr != 0) {
			vysledok.error = EZAKLADLOG;
		}
		if(vysledok.a == 1.0 || vysledok.a == 0.0 || vysledok.a == -1.0) {
			printf("%.10e", 0.0/0.0);
			vysledok.error = EZAKLADLOG;
		}
	} else {
	
		vysledok.error = EZLEPARAM;
		
	}

	return vysledok;

}
/**
 * funkcia na vypocet korektneho sigdigu
 **/ 
double epsilon(double cislo, int presnost)
{
	double eps;
	int pocet;
	
	if( cislo > 1 ) {
		pocet = 0;
		while(cislo > 1) {
		
			cislo = cislo / 10;
			pocet++;
		
		}
		eps = pow(0.1, presnost-pocet);
		
	} else {
		pocet = 0;
		while(cislo < 1) {
			
			
			cislo = cislo * 10;
			pocet++;
			
		
		}
		eps = pow(0.1, presnost+pocet);
		
		
	}
	
	return eps;
	
}
/**
 * vypocita hyperbolicky sinus zo zadaneho cisla na urcenu presnost
 * vracia vysledok v double 
 **/
double sinush(double x, int sigdig)
{
	double vysledok = x;
	double vysledokStary = 1;
	double citatel = x;
	double citatelMocnina = x*x;
	double faktorial = 6.0;
	double faktorialIndex = 3.0;
	
	while(epsilon(vysledok, sigdig) < fabs( vysledok - vysledokStary)) {
		vysledokStary = vysledok;
		citatel = citatel * citatelMocnina;
		vysledok = vysledok + ( citatel / faktorial );
		faktorialIndex = faktorialIndex + 2;
		faktorial = faktorial * ( faktorialIndex - 1 ) * faktorialIndex;
	}
	
	return vysledok;
}
/**
 * vypocita hyperbolicky kosinus zo zadaneho cisla na urcenu presnost
 * vracia vysledok v double 
 **/ 
double cosinush(double x, int sigdig)
{
	double vysledok = 1.0;
	double vysledokStary = 0.0;
	double citatel = 1.0;
	double citatelMocnina = x*x;
	double faktorial = 2.0;
	double faktorialIndex = 2.0;
	
	while(epsilon(vysledok, sigdig) < fabs( vysledok - vysledokStary)) {
		vysledokStary = vysledok;
		citatel = citatel * citatelMocnina;
		vysledok = vysledok + ( citatel / faktorial );
		faktorialIndex = faktorialIndex + 2;
		faktorial = faktorial * ( faktorialIndex - 1 ) * faktorialIndex;
	}
	
	return vysledok;
}
/**
 * vypocita hyperbolicky tangens zo zadaneho cisla na urcenu presnost
 * vracia vysledok v double 
 **/ 
double htangens(double x, int sigdig)
{
	return sinush(x, sigdig) / ( cosinush(x, sigdig));
}
/**
 * vstupom je struktura, zo zadanych udajov sa pocita vazeny aritmeticky/kvadraticky priemer
 * vracia vysledok v double 
 **/ 
double priemery_pocitaj(short int typ, TWam *struktura)
{
	double vysledok;
	
	if( typ == 1 ) struktura->hodnota = struktura->hodnota * struktura->hodnota;
	
	struktura->citatel = struktura->citatel + struktura->hodnota * struktura->vaha;
	struktura->menovatel = struktura->menovatel + struktura->vaha;
	
	vysledok = struktura->citatel / struktura->menovatel;
	
	if( typ == 0 ) {
		return vysledok;
	} else {
		return sqrt( vysledok );
	}
}
/**
 * stara sa o rozlisenie vstupnej hodnoty a priradi jej miesto v strukture
 * vola funkciu priemery_pocitaj na vypocet
 * vracia vysledok v double  
 **/ 
double priemery(short int typ, double hodnota, TWam *struktura)
{
	double vysledok;
	
	// typ 0 = wam
	// typ 1 = wqm
	
	switch( struktura->prepinac ) {
	
		case 1:
			struktura->hodnota = hodnota;
			break;
		case 2:
			struktura->vaha = hodnota;
			break;
			
	}
	if( struktura->prepinac == 2) {
	
		struktura->prepinac = 0;
		vysledok = priemery_pocitaj( typ, struktura );
		
	}
	
	struktura->prepinac++;
	
	return vysledok;
}
/**
 * vypocita prirodzeny logaritmus zo zadaneho cisla na danu presnost
 * vracia vysledok v double 
 **/ 
double logaritmusn(double x, int sigdig)
{
	double citatel;
	double menovatel = 1;
	double mocnina;
	double vysledok = 0;
	double vysledokStary = 1;
	int n = 1;
	int pocetDeleni = 0;
	
	
	while(x >= 2) {
		x = x/10;
		pocetDeleni++;
	}
	
	citatel = x - 1;
	mocnina = citatel;
	
	vysledok = citatel / menovatel;
	vysledok = vysledok + pocetDeleni * LN10;
	
	while( epsilon(vysledok,sigdig)  < fabs(vysledokStary - vysledok) ) {
		vysledokStary = vysledok;
		mocnina = -1 * mocnina * citatel;
		menovatel++;
		vysledok = vysledok + mocnina / menovatel;
		n++;
	}
	
	return vysledok;
}
/**
 * vypocita logaritmus zo zadaneho cisla, pri zadanom zaklade na urcenu presnost
 * vracia vysledok v double 
 **/ 
double logaritmus(double x, double zaklad, int sigdig)
{
	double vysledok;
	
	if( zaklad == 0.0 || x == 0.0 ) return 1.0/0.0;
	if( x < 0 || zaklad < 0 ) return 0.0/0.0;
	
	vysledok = logaritmusn(x, sigdig) / logaritmusn(zaklad, sigdig);
	
	return vysledok;
}

/**
 *
 * hlavny program
 *
 * */

int main(int argc, char *argv[])
{
	double hodnota;
	double vysledok = 0.0;
	int scan;
	TParams hodnoty = ziskajParametre(argc, argv);
	
	TWam *wamStruktura = malloc(sizeof(TWam));
	if( wamStruktura == NULL) {
		zobrazChybu(EMALLOC);
		return EXIT_FAILURE;
	}
		wamStruktura->hodnota = 0.0;
		wamStruktura->vaha = 0.0;
		wamStruktura->citatel = 0.0;
		wamStruktura->menovatel = 0.0;
		wamStruktura->prepinac = 1;
	
	if( hodnoty.error != EOK) {
		zobrazChybu(hodnoty.error);
		return EXIT_FAILURE;
	}
	
	if( hodnoty.status == STATUSNAPOVEDA ) {
		printf("%s", NAPOVEDATEXT);
		free(wamStruktura);
		return EXIT_SUCCESS;
	}
	
	// nacitavanie udajov
	while( (scan = scanf("%lf", &hodnota) ) != EOF ) {
		
		// chybny vstup
		if( scan == 0 ) {
			scanf("%*s");
			hodnota = 0.0/0.0;
		}
			// hlavne rozdelenie programu
			switch( hodnoty.status ) {
		
				case STATUSTANH:
					vysledok = htangens(hodnota, hodnoty.sigdig);
					break;
				case STATUSLOGAX:
					vysledok = logaritmus(hodnota, hodnoty.a, hodnoty.sigdig);
					break;
				case STATUSWAM:
					vysledok = priemery(0, hodnota, wamStruktura);
					break;
				case STATUSWQM:
					vysledok = priemery(1, hodnota, wamStruktura);

					break;
					
				default:
					free(wamStruktura);
					return EXIT_SUCCESS;
					break;
			}
			if(((hodnoty.status==STATUSWAM || hodnoty.status==STATUSWQM) && wamStruktura->prepinac==1) || hodnoty.status==STATUSTANH || hodnoty.status==STATUSLOGAX)
			{
				printf("%.10e\n", vysledok);
			}
	}
	
	// vypisuje 
	if( wamStruktura->prepinac == 2 ) {
		printf("%.10e", 0.0/0.0);
	}
	
	free(wamStruktura);
	return EXIT_SUCCESS;
}

/* koniec proj2.c */
