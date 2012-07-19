/*******************************************************************************
* Subor:	barbers.c
* Datum:	2011
* Autor:	Branislav Blaskovic xblask00@stud.fit.vutbr.cz
* Projekt:	IOS 2. Projekt 2010/2011
* Tema:		Riesenie problem jedneho spiaceho holica
*******************************************************************************/

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <string.h>
#include <signal.h>

/*******************************************************************************
* enum ERRORS
* const char *ERRORS_TEXT[]
* 	- chybove cisla a texty k nim
*******************************************************************************/
enum ERRORS
{
	E_OK = 0,
	E_PARAMETRE,
	E_PAMAT,
	E_MAZANIE_PAMATE,
	E_SUBOR,
	E_SUBOR_ZATVOR,
	E_SEM_INIT,
	E_0_ZAKAZNIKOV,
};
const char *ERRORS_TEXT[] =
{
	// E_OK
	"Vsetko ok.\n",
	// E_PARAMETRE
	"Zadane zle parametre\n",
	// E_PAMAT
	"Chyba pri vytvarani segmentu zdielanej pamate",
	// E_MAZANIE_PAMATE
	"Chyba pri mazani zdielanej pamate",
	// E_SUBOR
	"Chyba pri otvarani subora",
	// E_SUBOR_ZATVOR
	"Chyba pri zatvarani subora",
	// E_SEM_INIT
	"Chyba pri inicializacii semafora",
	// E_0_ZAKAZNIKOV
	"Neprisiel ani jeden zakaznik",
};

/*******************************************************************************
* TParametre
* 	- Struktura pre uchovavanie udajov z parametrov
*******************************************************************************/
typedef struct parametre
{
	int stolicky;
	int interval;
	int obsluha;
	long spolu;
	char *subor;
	int chyba;
} TParametre;

/*******************************************************************************
* TUdaje
* 	- struktura pre zdielanu pamat
*******************************************************************************/
typedef struct udaje
{
	long poradie;
	sem_t zakazniciSEM;
	sem_t holicSEM;
	sem_t sedackySEM;
	sem_t strihanieSEM;
	sem_t vstupDoCakarneSEM;
	sem_t dokonceneStrihanieSEM;
	sem_t vypisSEM;
	sem_t zakaznikJeReadySEM;
	sem_t pracaSPamatouSEM;
	long volneSedacky;
	long zostavajuciZakaznici;
	int obsluha;
	FILE *handlerSubor;
} TUdaje;

/*******************************************************************************
* void zobrazChybu(int kodChyby)
* @arg int kodChyby	- kod chyby, ktoru chceme vypisat
* 	- vypise chybovu hlasku podla kodu
*******************************************************************************/
void zobrazChybu(int kodChyby)
{
	if( kodChyby > E_OK ) {
		fprintf(stderr, "%s\n", ERRORS_TEXT[kodChyby]);
	}
}

/*******************************************************************************
* int zmazPamat( int id_pamate )
* @arg int id_pamate	- id pamate, ktoru chcem zmazat
* @return					- vracia 1 pri chybe a 0 pri uspechu
* 	- zmaze zdielanu pamait
*******************************************************************************/
int zmazPamat( int id_pamate, TUdaje *udaje )
{
	struct shmid_ds shmbuffer;
	
	// znicim semafory
	sem_destroy( &udaje->pracaSPamatouSEM );
	sem_destroy( &udaje->sedackySEM );
	sem_destroy( &udaje->strihanieSEM );
	sem_destroy( &udaje->vstupDoCakarneSEM );
	sem_destroy( &udaje->vypisSEM );
	sem_destroy( &udaje->zakazniciSEM );
	sem_destroy( &udaje->zakaznikJeReadySEM );
	sem_destroy( &udaje->dokonceneStrihanieSEM );
	sem_destroy( &udaje->holicSEM );
	
	// odpojim pamat
	if( shmctl (id_pamate, IPC_RMID, &shmbuffer) == -1 ) {
		zobrazChybu(E_MAZANIE_PAMATE);
		return 1;
	} else {
		return 0;
	}
}

/*******************************************************************************
* int zatvorSubor( char *nazovSubora, FILE *handler )
* @arg char *nazovSubora	- nazov subora, ak je "-" tak sa nic nerobi
* @arg FILE *handler		- ukazatel na subor
* @return					- vracia 1 pri chybe a 0 pri uspechu
* 	- zatvara otvoreny subor (ak sme ho otvarali)
*******************************************************************************/
int zatvorSubor( char *nazovSubora, FILE *handler )
{
	if( strcmp(nazovSubora, "-") != 0 ) {
		if( fclose( handler ) == EOF ) {
			zobrazChybu( E_SUBOR_ZATVOR );
			return 1;
		} else {
			return 0;
		}
	} else {
		return 0;
	}
}

/*******************************************************************************
* void zaspi( int cas )
* @arg int cas	- cas v milisekundach
* 	- uspi na nahodny cas od 0 do 'int cas' milisekund
*******************************************************************************/
void zaspi( int cas )
{
	int nahodneCislo = (rand() % (cas+1) ) * 1000;
	usleep( nahodneCislo );
}

/*******************************************************************************
* TParametre spracujParametre(int argc, char *argv[])
* @arg int argc		- pocet argumentov volania programu
* @arg char *argv[]	- pole hodnot argumentov volania programu
* @return			- parametre v strukture TParametre, taktiez chybu
* 	- spracuje vstupne argumenty a vyhodnoti ich
*******************************************************************************/
TParametre spracujParametre(int argc, char *argv[])
{
	TParametre param;
	char *endptr;
	
	if( argc == 6 ) {
		
		param.chyba = E_OK;
		
		param.interval = strtol(argv[2], &endptr, 10);
		if( *endptr != 0 ) {
			param.chyba = E_PARAMETRE;
		}
		param.obsluha = strtol(argv[3], &endptr, 10);
		if( *endptr != 0 ) {
			param.chyba = E_PARAMETRE;
		}
		param.spolu = strtol(argv[4], &endptr, 10);
		if( *endptr != 0 ) {
			param.chyba = E_PARAMETRE;
		}
		param.stolicky = strtol(argv[1], &endptr, 10);
		if( *endptr != 0 ) {
			param.chyba = E_PARAMETRE;
		}
		param.subor = argv[5];
		
		// ak nepride ani 1 zakaznik
		if( param.spolu == 0 ) {
			zobrazChybu( E_0_ZAKAZNIKOV );
		}
		// ak boli zadanie zaporne hodnoty
		if( param.interval < 0 || param.obsluha < 0 || param.stolicky < 0 ||
			param.spolu < 0
		) {
			param.chyba = E_PARAMETRE;
		}
	} else {
		
		param.chyba = E_PARAMETRE;
	}
	
	return param;
}

/*******************************************************************************
* void napisAkciu( int *cisloAkcie, int kto, int cisloZakaznika, char *akcia,
*					sem_t *vypisSEM, FILE *handler )
* @arg int *cisloAkcie		- cislo prevadzanej akcie
* @arg int kto				- osoba akcie: 0 = barber, 1 = customer 
* @arg int cisloZakaznika	- cislo zakaznika/customer
* @arg char *akcia			- text akcie
* @arg sem_t *vypisSEM		- semafor pre vypis
* @arg FILE *handler		- ukazatel na subor, do ktoreho zapisujeme
* 	- vypisuje pozadovanu akciu do suboru, alebo na stdout
*******************************************************************************/
void napisAkciu( long *cisloAkcie, int kto, long cisloZakaznika, char *akcia,
					sem_t *vypisSEM, FILE *handler )
{
	sem_wait( vypisSEM );
	if( kto == 0 ) {
		// Barber == 0
		fprintf(handler, "%ld: barber: %s\n", ++(*cisloAkcie), akcia );
	} else {
		// Customer == 1
		fprintf(handler, "%ld: customer %ld: %s\n", ++(*cisloAkcie),
				cisloZakaznika, akcia);
	}
	sem_post( vypisSEM );
}

/*******************************************************************************
* int vytvorPamat()
* @return	- cislo pamate
* 	- vytvara zdielanu pamat
*******************************************************************************/
int vytvorPamat(char *nazovBinarky)
{
	int shmid;
	key_t key;
	key = ftok(nazovBinarky, 1);
	shmid = shmget(key , sizeof( TUdaje ), IPC_CREAT|0600);
	
	return shmid;
}

/*******************************************************************************
* void zakaznikMain(int cisloZakaznika, int id_pamate)
* @arg int CisloZakaznika	- cislo aktualneho zakaznika
* @arg int id_pamate		- cislo zdielanej pamate
* 	- hlavna funkcia pre proces zakaznika/customer
*******************************************************************************/
void zakaznikMain(long cisloZakaznika, int id_pamate)
{
	TUdaje *udaje;
	udaje = shmat( id_pamate , NULL, 0 );
	
	napisAkciu( &udaje->poradie, 1, cisloZakaznika+1, "created",
		&udaje->vypisSEM, udaje->handlerSubor );
	
	// caka na umoznenie vstupu do cakarne
	sem_wait( &udaje->vstupDoCakarneSEM );
	napisAkciu( &udaje->poradie, 1, cisloZakaznika+1, "enters",
				&udaje->vypisSEM, udaje->handlerSubor );

	if ( udaje->volneSedacky > 0 ) {
		// uvolnim vstup do cakarne
		sem_post( &udaje->vstupDoCakarneSEM );
		udaje->volneSedacky--;
		// upozornim holica
		sem_post( &udaje->zakazniciSEM );
		// uvolnim sedacky
		sem_post( &udaje->sedackySEM );
		// cakam na holica, kym sa odomkne
		sem_wait( &udaje->holicSEM );
		napisAkciu( &udaje->poradie, 1, cisloZakaznika+1, "ready",
					&udaje->vypisSEM, udaje->handlerSubor );
		// oznamime, ze je zakaznik ready
		sem_post( &udaje->zakaznikJeReadySEM );
		// caka na koniec strihania
		sem_wait( &udaje->strihanieSEM );
		
		sem_wait( &udaje->pracaSPamatouSEM );
		udaje->zostavajuciZakaznici--;
		sem_post( &udaje->pracaSPamatouSEM );
		
		napisAkciu( &udaje->poradie, 1, cisloZakaznika+1, "served",
					&udaje->vypisSEM, udaje->handlerSubor );
		// posle info, ze zaznamenal koniec strihania a vypisal served
		sem_post( &udaje->dokonceneStrihanieSEM );		

	} else { // nemal volnu stolicku v cakarni
		// uvolnim vstup do cakarne
		sem_post( &udaje->vstupDoCakarneSEM );
		// uvolnim sedacky
		sem_post( &udaje->sedackySEM );
		// nebol ostrihany ( nemal miesto )
		napisAkciu( &udaje->poradie, 1, cisloZakaznika+1, "refused",
					&udaje->vypisSEM, udaje->handlerSubor );
					
		sem_wait( &udaje->pracaSPamatouSEM );
		udaje->zostavajuciZakaznici--;
		sem_post( &udaje->pracaSPamatouSEM );
		
	}
	if(udaje->handlerSubor != stdout) fclose(udaje->handlerSubor);
	return;
}

/*******************************************************************************
* void holicMain( int id_pamate )
* @arg int id_pamate	- cislo zdielanej pamate
* 	- hlavna funkcia pre proces holica/barber
*******************************************************************************/
void holicMain( int id_pamate )
{
	TUdaje *udaje;
	udaje = shmat( id_pamate , NULL, 0 );
	
	while(1==1) {
		
		sem_wait( &udaje->pracaSPamatouSEM );
		if( udaje->zostavajuciZakaznici > 0 ) {		
			napisAkciu( &udaje->poradie, 0, 0, "checks", &udaje->vypisSEM,
						udaje->handlerSubor );
		}
		sem_post( &udaje->pracaSPamatouSEM );
		
		// caka na zakaznika, ked nie je, tak spi
		sem_wait( &udaje->zakazniciSEM );
		napisAkciu( &udaje->poradie, 0, 0, "ready", &udaje->vypisSEM,
					udaje->handlerSubor );
		// znizime pocet sedaciek
		sem_wait( &udaje->sedackySEM );
		// v cakarni sa uvolnila sedacka
		sem_wait( &udaje->pracaSPamatouSEM );
			udaje->volneSedacky++;
		sem_post( &udaje->pracaSPamatouSEM );
		// odomkneme holica, na ktoreho zakaznika caka
		sem_post( &udaje->holicSEM );
		// odomkneme kreslo
		sem_post( &udaje->sedackySEM );
		// pockame, kym pride zakaznik a posadi sa
		sem_wait(&udaje->zakaznikJeReadySEM);
		// strihame vlasy, uspime na chvilu program
		zaspi( udaje->obsluha );
		napisAkciu( &udaje->poradie, 0, 0, "finished", &udaje->vypisSEM,
					udaje->handlerSubor );
		// otvorime semafor strihania
		sem_post( &udaje->strihanieSEM );
		// cakame na zakaznik, aby zistil, ze bol ostrihany
		// osetrenie vypisu checks pred served
		sem_wait( &udaje->dokonceneStrihanieSEM );
		
	}
	
}

/*******************************************************************************
* int main(int argc, char *argv[])
* @arg int argc		- pocet argumentov volania programu
* @arg char *argv[]	- pole hodnot argumentov volania programu
* 	- hlavna funkcia pre beh programu
*******************************************************************************/
int main(int argc, char *argv[])
{
	// spracujeme parametre a ulozime do struktury
	TParametre parametre = spracujParametre(argc, argv);
	// zle parametre
	if( parametre.chyba != E_OK ) {
		zobrazChybu(parametre.chyba);
		return EXIT_FAILURE;
	}
	
	// premenne
	pid_t zakazniciPID[parametre.spolu];
	pid_t holicPID;
	int id_pamate = vytvorPamat(argv[0]);
	TUdaje *udaje;
	FILE *handlerSubor;
	
	// otvorime subor (ak je pozadovany zapis do subora)
	if( strcmp(parametre.subor, "-") != 0 ) {
		handlerSubor = fopen (parametre.subor, "w");
		if( handlerSubor == NULL ) {
			zobrazChybu( E_SUBOR );
			return EXIT_FAILURE;
		}
		// nastavime buffer aby sa nepredbiehali vypisy
		setbuf(handlerSubor, NULL);
	} else {
		// zapis do subora nebol pozadovany a preto vypisujem na stdout
		handlerSubor = stdout;
		// nastavime buffer aby sa nepredbiehali vypisy
		setbuf(stdout, NULL);
	}
	
	// nevytvorena pamat
	if( id_pamate == -1 ) {
		zobrazChybu( E_PAMAT );
		// zatvorim otvoreny subor
		zatvorSubor( parametre.subor, handlerSubor );
		return EXIT_FAILURE;
	}
	
	// zamiesame cisla
	srand ( time(NULL) );
	
	// naplnime strukturu s pociatocnymi udajmi
	udaje = shmat( id_pamate , NULL, 0 );
	
	// prednastavime semafory
	if( sem_init( &udaje->zakazniciSEM, 1, 0 ) ||
		sem_init( &udaje->holicSEM, 1, 0 ) ||
		sem_init( &udaje->strihanieSEM, 1, 0 ) ||
		sem_init( &udaje->sedackySEM, 1, 1) ||
		sem_init( &udaje->vstupDoCakarneSEM, 1, 1) ||
		sem_init( &udaje->dokonceneStrihanieSEM, 1, 0) ||
		sem_init( &udaje->vypisSEM, 1, 1) ||
		sem_init( &udaje->pracaSPamatouSEM, 1, 1) ||
		sem_init( &udaje->zakaznikJeReadySEM, 1, 0 )
	) {
		zobrazChybu( E_SEM_INIT );
		zmazPamat( id_pamate, udaje );
		zatvorSubor( parametre.subor, handlerSubor );
		return EXIT_FAILURE;
	}
	// prednastavime hodnoty
	udaje->volneSedacky = parametre.stolicky;
	udaje->zostavajuciZakaznici = parametre.spolu;
	udaje->obsluha = parametre.obsluha;
	udaje->handlerSubor = handlerSubor;
	udaje->poradie = 0;
	
	// spravime holica
	holicPID = fork();
	if( holicPID == 0 ) {
		holicMain( id_pamate );
		exit(EXIT_SUCCESS);
	}
	
	signal(SIGCHLD, SIG_IGN);
	
	// spravime zakaznikov
	for( long i = 0; i < parametre.spolu; i++ ) {
		
		zaspi( parametre.interval );
		zakazniciPID[i] = fork();
		
		// som v zakaznikovi
		if( zakazniciPID[i] == 0 ) {
			
			zakaznikMain(i, id_pamate);
			exit(EXIT_SUCCESS);
		}
		// zakaznik sa chudak nepodaril forknut
		if( zakazniciPID[i] == -1 ) {
			fprintf(stderr, "Zakaznik %ld sa nepodaril forknut\n", i);
			
			sem_wait( &udaje->pracaSPamatouSEM );
			udaje->zostavajuciZakaznici--;
			sem_post( &udaje->pracaSPamatouSEM );
		}
	}

	// cakam na skoncenie holica
	//waitpid(holicPID, NULL, 0);
	while( udaje->zostavajuciZakaznici > 0 ) {
		// nemozem zabit holica, pretoze zostali este nejaki zakaznici
	}

	kill( holicPID, SIGTERM );
	
	// cakam na skoncenie vsetkych zakaznikov
	for( long i = 0; i < parametre.spolu; i++ ) {
		waitpid(zakazniciPID[i], NULL, 0);
	}
	
	// mazem pamat
	if( zmazPamat( id_pamate, udaje ) == 1 ) {
		// zatvorim aj subor
		zatvorSubor( parametre.subor, handlerSubor );
		return EXIT_FAILURE;
	}
	
	// zatvaram subor
	if( zatvorSubor( parametre.subor, handlerSubor ) == 1 ) {
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
