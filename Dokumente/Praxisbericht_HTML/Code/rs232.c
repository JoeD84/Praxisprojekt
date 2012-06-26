#define BAUD 9600		// BAUD Rate definieren
#include <util/setbaud.h>	// UART Funktionen
// UART Initialisieren
void 	uart_init		() {	
	// UART 0 - IN (Rapidform Software/Terminal)
	UBRR0H = UBRRH_VALUE;	
	UBRR0L = UBRRL_VALUE;
	UCSR0C = (3 << UCSZ00);
	UCSR0B |= (1 << TXEN0); 	//Transmitter Enabled
	UCSR0B |= (1 << RXEN0); 	// UART RX einschalten
	// UART 1 - OUT (Stepper Karte/Drehtisch)
	UBRR1H = UBRRH_VALUE;		
	UBRR1L = UBRRL_VALUE;
	UCSR1C = (3 << UCSZ00);
	UCSR1B |= (1 << TXEN1); 	//Transmitter Enabled
	UCSR1B |= (1 << RXEN1); 	// UART RX einschalten
}
// UART Zeichen senden
void 	uart_put_charater	(unsigned char c, int dir) {	
	if (dir == D_RapidForm) {	// To Rapidform
		while (!(UCSR0A & (1 << UDRE0))) {}//warten bis Senden moeglich
		UDR0 = c; 		// sende Zeichen
	}
	else {				// To Stepper
		while (!(UCSR1A & (1 << UDRE1))) {}//warten bis Senden moeglich
		UDR1 = c;		// sende Zeichen
	}
}
// UART String senden
void 	uart_put_string		(char *s, int dir) {	
	while (*s){ // so lange *s != '\0' Terminierungszeichen
		uart_put_charater(*s, dir); // Zeichenweise senden
		s++;
	}
}
// UART Zeichen empfangen
int 	uart_get_character	(int dir) {	
	if (dir == D_RapidForm) {	// Aus RapidForm Register auslesen
		while (!(UCSR0A & (1 << RXC0))) ; // warten bis Zeichen verfuegbar
		return UDR0; // Zeichen aus UDR an Aufrufer zurueckgeben
	}
	if (dir == D_Stepper) {		// Aus Schrittmotor Register auslesen
		while (!(UCSR1A & (1 << RXC1))) ; // warten bis Zeichen verfuegbar
		return UDR1; // Zeichen aus UDR an Aufrufer zurueckgeben
	}
	return -1;	// Wenn nichts ausgelesen wurde -1 zurueckgeben
}
// UART String empfangen
void 	uart_get_string		(char * string_in, int dir) {
	char c;		// Einzelnes Zeichen
	int i = 0;	// Zaehlvariable
	do {
		c = uart_get_character(dir); 	// Einzelnes Zeichen holen
		if (c != '\r') {		// Wenn keinn \r
			*string_in = c;		// Zeichen in Empfangsstring schreiben
			string_in += 1;		// Adresse des Empfangsstring um 1 ink
			i++;			// Zaehlvariable um 1 erhoehen
		}
	} while (i < 100 && c != '\r' && c != '\n'); // So lange bis \r \n o. >100 Zeichen
	*string_in = '\0';			// 0 Terminieren
	if (dir == D_Stepper)
		LED_PORT |= ( 1 << LED3 );	// "Daten Vorhanden" LED ausschalten
	else
		LED_PORT |= ( 1 << LED2 );	// "Daten Vorhanden" LED ausschalten
}