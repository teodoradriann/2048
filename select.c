/* Programarea Calculatoarelor, seria CC
 * Tema2 - 2048
 */
#include <stdio.h>
#include <string.h>
#include <sys/select.h>

#define FOREVER 		1
#define STR_MAX_LEN 		100
#define S_TO_WAIT 		2
#define MILIS_TO_WAIT		0
#define KEYBOARD		0
#define QUIT			"quit"
#define SELECT_ERROR		-1
#define SELECT_EVENT		1
#define SELECT_NO_EVENT		0

int main(void)
{
	int nfds, sel;
	char s[STR_MAX_LEN];
	fd_set read_descriptors;
	struct timeval timeout;

	/* Funcția select va 'asculta' tastatura
	 * La scrierea unui șir de caractere + apăsarea ENTER, 
	 * funcția select va întoarce o valoare > 0
	 * Notă: pentru programul cu ncurses,
	 * select va întoarce o valoare > 0 atunci când se introduce
	 * un singur caracter 
	 */

	/* Funcția select va asculta 'dispozitivele' aflate în mulțimea read_descriptors
	 * Numărul de elemente din mulțime = nfds = 1
	 * Pentru select, tastatura este identificată prin numărul 0
	 */
	nfds = 1;
	/* Se curăță mulțimea de lucru pentru select */
	FD_ZERO(&read_descriptors);
	/* Se adaugă tastatura la mulțime */
	FD_SET(KEYBOARD, &read_descriptors);

	/* Funcția select va așteapta evenimente de la tastatură - introducere de șiruri urmate de ENTER
	 * Dacă după un timp (timeout) nu au loc evenimente, atunci se iese din funcție,
	 * iar funcția returnează valoarea 0 
	 * În timeout, specificăm câte secunde (tv_sec) și câte milisecunde (tv_usec)
	 * vrem să așteptăm
	 */
	timeout.tv_sec = S_TO_WAIT;
	timeout.tv_usec = MILIS_TO_WAIT;
		
	/* Se iese din while la introducerea șirului quit urmat de ENTER */	
	while (FOREVER) {
		sel = select(nfds, &read_descriptors, NULL, NULL, &timeout);

		switch (sel) {
		case SELECT_EVENT:
			/* select ne anunță faptul că date de la tastatură
			 * sunt gata de citit 
			 */
			scanf("%s", s);
			printf("Event on keyboard; you typed: %s.\n", s);
			
			if (!strcmp(s, QUIT)) {
				return 0;
			}
			break;
		
		case SELECT_NO_EVENT:
			/* A expirat timpul din timeout
			 * fără să primim nimic de la tastatură
			 */
			printf("Nothing happended; timeout fired.\n");
			break;
			
		case SELECT_ERROR:
			printf("Select error; exiting...\n");
			return 0;
		}
		
		/* Funcția select modifică read_descriptors și timeout
		 * Așadar, aceste variabile trebuie reinițializate
		 */	
		FD_SET(KEYBOARD, &read_descriptors);
		
		timeout.tv_sec = S_TO_WAIT;
		timeout.tv_usec = MILIS_TO_WAIT;
	}
	
	return 0;
}

/* Mai multe detalii despre select și prietenii în anul 2, 
 * semestrul 2, la materia Protocoale de Comunicații
 */
