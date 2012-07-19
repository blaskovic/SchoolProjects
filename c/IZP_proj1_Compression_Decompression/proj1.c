/*
 * Subor:
 * Datum:	oktober 2010
 * Autor:	Branislav Blaskovic xblask00@stud.fit.vutbr.cz
 * Projekt:	Jednoduchá kompresia a dekompresia textu, projekt 1 pre predmet IZP
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

/**
 * Struktura na zachytavanie parametrov, chyb, statusu
 */
typedef struct params
{
	unsigned int N;
	int error;
	int status;
} TParams;

/**
 * chyby
 * */
enum ERRORS
{
	EOK = 0,		// Bez chyby
	ECHYBATERMINAL,	// Zle parametre
	ENEZNAMA,		// Neznámá chyba
	EZLEPARAM,		// Zle parametre
	ECISLON,		// N nie je > 0
	EKOMPCISLO,		// pri komprimacii sa vyskytlo cislo
	EMALLOC,		// nepodarilo sa vyhradit miesto
	EVSTUP			// neplatny vstup
};
/**
 * texty k chybam
 * */
const char *ERRORSTEXT[] =
{
	// EOK
	"Vsetko v poriadku.",
	// ECHYBATERMINAL
	"Zle parametre v terminali!",
	// ENEZNAMA
	"3-2-1 BOOOOOM!\n",
	// EZLEPARAM
	"Zadajte -c N pre kompresiu, -d N pre dekompresiu",
	// ECISLON
	"Cislo N musi byt vacsie alebo rovne 1 do rozsahu unsigned int",
	// EKOMPCISLO
	"Cislo sa nemoze nachadzat v komprimacii",
	// EMALLOC
	"Nepodarilo sa vyhradit miesto funkcii malloc()",
	// EVSTUP
	"Neplatny vstup",
};

/**
 * status
 * */
enum STATUS
{
	STATUSNAPOVEDA,			// help
	STATUSKOMP,				// komprimacia
	STATUSDEKOMP,			// dekomprimacia
};

/**
 * text k napovede
 * */
const char *NAPOVEDATEXT =
	"Program:\n\tJednoducha kompresia\n"
	"Autor:\n\tBranislav Blaskovic xblask00\n"
	"Pouzitie:\n"
	"\tproj1 -h - zobrazi napovedu \n"
	"\tproj1 -c N - kompresia textu s parametrom N\n"
	"\tproj1 -d N - dekompresia textu s parametrom N \n"
	"Popis parametra N:\n"
	"Parameter N musi byt cislo v rozsahu unsigned int vacsie ako 0.\n";

/**
 * Vypise chybu podla kodu
 * */
void zobrazChybu(int kodChyby)
{
	fprintf(stderr," *Chyba: %s* ", ERRORSTEXT[kodChyby]);
}

/**
 * ziska parametre z Main() a vyhodnocuje ich
 * */
TParams ziskajParametre(int argc, char *argv[])
{
	char *endptr;
	
	TParams vysledok =
	{
		.N = 0,
		.error = EOK,
		.status = STATUSKOMP,
	};


	if (argc == 2 && strcmp("-h", argv[1]) == 0) {

		// napoveda
		vysledok.status = STATUSNAPOVEDA;

	} else if (argc == 3) {
		
		vysledok.N = strtoul (argv[2], &endptr, 10);

		if (endptr == argv[2] || (vysledok.N >= UINT_MAX) || vysledok.N == 0 || *endptr != 0 || argv[2][0]==' ' || argv[2][0]=='-') {
			vysledok.error = ECISLON;
			return vysledok;
		}		
		
		// dva parametre -c|-d a N
		if(strcmp("-c", argv[1]) == 0 && vysledok.error == EOK) {
			// kompresia
			vysledok.status = STATUSKOMP;

		} else if (strcmp("-d", argv[1]) == 0 && vysledok.error == EOK) {
			// dekompresia
			vysledok.status = STATUSDEKOMP;

		} else {
			vysledok.error = EZLEPARAM;
		}

	} else {
		// vela parametrov
		vysledok.error = ECHYBATERMINAL;
	}

	return vysledok;

}
/**
 * vypisuje pole podla zadaneho poctu znakov N
 * */
void vypisPole(char *pole, unsigned int *pocet)
{
	unsigned int i;
	
	for(i = 0; i < *pocet; i++) {
		if(pole[i]!='\0') {
			putchar(pole[i]);
		}
	}
}

/**
 *
 * Dekompresia
 *
 * */
int dekompresia(TParams hodnoty)
{

	unsigned int *pocet = &hodnoty.N;
	unsigned int pocetNacitavanie;
	int input, pocetOpakovani;
	unsigned int i;
	int opakujem = 0;
	int e = 0;
	unsigned int pocetZnakov = *pocet + 1;
	char *retazec = malloc(pocetZnakov * sizeof(char));
		if(retazec == NULL) return EMALLOC;

	i = 0;
	memset(retazec, '\0', pocetZnakov);

	while( (input = getchar()) != EOF )
	{
		if(pocetNacitavanie!=0 && opakujem==1) {
			if(isdigit(input)) {
				zobrazChybu(EVSTUP);
				input = '\0';
			}
			retazec[i] = input;
			pocetNacitavanie--;
			i++;
		} else {
			pocetNacitavanie = *pocet;

			i = 0;

			if(opakujem==1) {
				for(e=1;e<=pocetOpakovani-48;e++) {
					vypisPole(retazec, pocet);
				}
				opakujem = 0;
			}

			if( input>='1' && input<='9' ) {
				opakujem = 1;
				pocetOpakovani = input;
			} else {
				putchar(input);
			}
		}
	}
	// na zaver, posledne pole, ak bola zhoda
	if(pocetNacitavanie!=0 && opakujem==1) {
		zobrazChybu(EVSTUP);
	} else if(opakujem==1) {
		for(e=1;e<=pocetOpakovani-48;e++) {
			vypisPole(retazec, pocet);
		}
		opakujem = 0;
	}
	return 0;
}
/**
 * zistuje, ci su v poli len nulte bajty
 * */
int zistiNulteBajty(char *pole, unsigned int *pocet)
{
	unsigned int i;
	int nulteBajty = 1;
	
	for(i = 0; i < *pocet; i++) {
		if( pole[i] != '\0' ) nulteBajty = 0;
	}
	
	return nulteBajty;
}
/**
 * zistuje, ci su polia zhodne
 * */
int porovnajPolia(char *pole, char *pole2, unsigned int *pocet)
{
	if(strcmp(pole,pole2) == 0 && zistiNulteBajty(pole,pocet) == 0 && zistiNulteBajty(pole2,pocet) == 0) {
		return 1;
	} else {
		return 0;
	}
}
/**
 * vypisuje pocet opakovani
 * ked je pocet vacsi ako 9, rozpisuje na mensie casti
 * */
void vypisOpakovanie(char *pole, unsigned int *pocet, int *pocetOpakovani)
{
	while(*pocetOpakovani > 0)
	{
		if(*pocetOpakovani < 9) {
			if( *pocetOpakovani != 1) printf("%d", *pocetOpakovani);
		} else printf("9");
		
		vypisPole(pole, pocet);
		*pocetOpakovani = *pocetOpakovani - 9;	
	}
}
/**
 * po ukonceni nacitavania znakov spracovava posledne znaky
 * */
void upratovanie(char *pole, char *pole2, unsigned int *pocet, int *pocetOpakovani, unsigned long long *pocitadlo)
{
	if(porovnajPolia(pole, pole2, pocet)==0) {
	
		if( *pocetOpakovani > 1) {
			vypisOpakovanie(pole, pocet, pocetOpakovani);
			vypisPole(pole2, pocet);
		} else {
			vypisPole(pole, pocet);
			vypisPole(pole2, pocet);
		}
		
	} else {
		(*pocetOpakovani)++;
		if(*pocetOpakovani > 1) {
			vypisOpakovanie(pole, pocet, pocetOpakovani);
		}
	}
	
	memset(pole,'\0',*pocet + 1);
	memset(pole2,'\0',*pocet + 1);
	*pocetOpakovani = 1;
	*pocitadlo = 0;
}
/**
 * posuva znaky o jeden znak dolava v dvoch poliach
 * */
void posuvaniePola(char *pole, char *pole2, unsigned int *pocet)
{
	unsigned int i;
	
	for(i = 1; i < *pocet; i++) {
		pole[i-1] = pole[i];
	}
	pole[*pocet-1] = pole2[0];
	for(i = 1; i < *pocet; i++) {
		pole2[i-1] = pole2[i];
	}
}

/**
 *
 * kompresia
 *
 * */
int kompresia(TParams hodnoty)
{
	unsigned int *pocet = &hodnoty.N;
	unsigned int i = 0;
	unsigned long long *pocitadlo = malloc(1 * sizeof(long long));
		if(pocitadlo == NULL) return EMALLOC;
		*pocitadlo = 0;
	int pom = 1;
	int *pocetOpakovani = &pom;
	unsigned int pocetZnakov = *pocet+1;
	char *pole = malloc(pocetZnakov * sizeof(char)), *pole2 = malloc(pocetZnakov * sizeof(char));
		if(pole == NULL) return EMALLOC;
		if(pole2 == NULL) return EMALLOC;
		
	int input;
	
	memset(pole, '\0', pocetZnakov);
	memset(pole2, '\0', pocetZnakov);

	while( (input = getchar()) != EOF )
	{
		if(isdigit(input)) {
			zobrazChybu(EKOMPCISLO);
			input = '\0';
		}
		// prvotne naplnenie poli
		if(*pocitadlo < *pocet) {
			pole[*pocitadlo] = input;
			(*pocitadlo)++;
		} else if(*pocitadlo < *pocet*2) {
			pole2[*pocitadlo - *pocet] = input;
			(*pocitadlo)++;
		} else {
			
			// zacina porovnavanie
			if(porovnajPolia(pole, pole2, pocet)) {
				// retazce sa zhoduju
				for(i = 0; i < *pocet; i++) pole[i] = pole2[i];
				memset(pole2, '\0', pocetZnakov);
				pole2[0] = input;
				(*pocetOpakovani)++;
				*pocitadlo = *pocet + 1;
				
			} else {
				// retazce sa nezhoduju
				if(*pocetOpakovani > 1) {
					vypisOpakovanie(pole, pocet, pocetOpakovani);
					memset(pole,'\0',pocetZnakov);
					*pocetOpakovani = 1;
				} else {
					if(pole[0] != '\0') putchar(pole[0]);
				}
				
				// posuvanie pola
				posuvaniePola(pole,pole2, pocet);
				pole2[*pocet - 1] = input;
				
				
			}
		}
	}
	// zaverecne upratovanie
	upratovanie(pole, pole2, pocet, pocetOpakovani, pocitadlo);
		
	// uvolnenie pamate
	free(pole);
	free(pole2);
	free(pocitadlo);
	return 0;
}

/**
 *
 * hlavny program
 *
 * */

int main(int argc, char *argv[])
{
	int kodChyby;
	TParams hodnoty = ziskajParametre(argc, argv);

	// zobrazovanie chyb
	if (hodnoty.error != EOK) {
				zobrazChybu(hodnoty.error);
				return EXIT_FAILURE;
	}

	// zobrazenie napovedy
	if (hodnoty.status == STATUSNAPOVEDA)
	{
		printf("%s", NAPOVEDATEXT);
		return EXIT_SUCCESS;
	}

	// volanie kompresie a dekompresie
	if (hodnoty.status == STATUSKOMP)
	{
		// kompresia
		kodChyby = kompresia(hodnoty);
		if( kodChyby != 0) {
			// nastala chyba
			zobrazChybu(kodChyby);
		}
		
	} else if (hodnoty.status == STATUSDEKOMP)
	{
		// dekompresia
		kodChyby = dekompresia(hodnoty);
		if( kodChyby != 0) {
			// nastala chyba
			zobrazChybu(kodChyby);
		}
	}
	return EXIT_SUCCESS;
}

/* koniec proj1.c */
