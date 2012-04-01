/*
Stepper Translator - Recieve commands over RS-232, translate them and transmit them over RS-232.
Copyright (C) 2011  Johannes Dielmann

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define BAUD 9600			// BAUD Rate definieren
// Falls nicht bereits gesetzt, Taktfrequenz definieren
#ifndef F_CPU
#define F_CPU 8000000
#endif
// AVR Includes
#include <avr/io.h>			// Standardbibliothek
#include <util/delay.h>		// Verzoegerungen
#include <util/setbaud.h>	// UART Funktionen
#include <stdlib.h>			// Standardbibliothek
#include <string.h>			// String Funktionen nutzen (strncmp)
#include <avr/interrupt.h>	// Interrupts
#include <avr/wdt.h>		// Watchdog
#include <avr/pgmspace.h>	// Stringspeicher vorbelegen
// Meine Includes
#include "mystuff.h"		// Eigene Makrodefinitionen fuer erhoehte Lesbarkeit
#include "Debounce.h"		// Taster entprellen
//#include "lcd.h"			// LC-Display (wird automatisch gebaut)
// Globale Variablen
#define D_RapidForm 0
#define D_Stepper 	1
// Motor Protokolle
#define M_UNK	   -2
#define M_NOTI	   -1
#define M_ISEL 		0
#define M_CSG  		1
#define M_ZETA 		2
#define M_TERMINAL	3
// Protokoll Befehle
#define P_INIT 		0
#define P_FINISH 	1
#define P_AROT 		2
#define P_STOP		3
#define P_HOME		4
#define P_STEP		5
#define P_TIMEOUT	6
// Erweiterte Befehle
#define E_CLS		10
#define E_TEST		11
// Antworten
#define B_Zeta 		"\r\n>\040\r\n>\040\r\n>\040\r\n>\040"
#define B_OK 		"0\r\n"
// Variablen Definitionen
#define MENU_ENTRY_NAMELEN 19
#define RETURN_LEN 40
int 	Initialized = M_NOTI;
int 	move = 0;
int 	init_T = 0;
char 	str_rx[100];

//// Tinymenu
#define MCU_CLK F_CPU		// MCU_CLK = F_CPU fuer TinyMenu
#include "tinymenu/spin_delay.h"
#define CONFIG_TINYMENU_USE_CLEAR
#include "tinymenu/tinymenu.h"
#include "tinymenu/tinymenu_hw.h"

//// Funktionsdefinitionen
// UART Stuff
void 	uart_init			();
void 	uart_put_charater	(unsigned char c, int dir);
void 	uart_put_string		(char *s, int dir);
int  	uart_get_character	(int dir);
void 	uart_get_string		(char * string_in, int dir);
void 	uart_rx				(int dir);
// String Stuff
int 	FindStringInArray	(const char* pInput, const char* pOptions[], int cmp_length);
void 	String_zerlegen_Isel(char * str_rx, char * Position, char * Winkel);
void 	String_zerlegen_csg	(char * str_rx);
// Hilfs Funktionen
void 	csg_Status_melden	();
// Auswerte Logik
int 	switch_Motor		(char * str_rx);
void 	switch_Stepper		(char * str_rx);
void 	switch_Isel			(char * str_rx);
void 	switch_csg			(char * str_rx);
// LCD und LED Stuff
void 	lcd_my_type			(char *s);
void 	lcd_boot			(void);
void 	led_boot			(void);
void 	debounce_init		(void);
void 	led_lauflicht		(void);
// Menu Stuff
void 	mod_manual			(void *arg, void *name);
void 	my_select			(void *arg, char *name);
void 	menu_puts			(void *arg, char *name);
#include "mymenu.h"
// Init Stuff
void 	init_WDT			(void);
void 	init				(void);

//////////////////////////////
//
//      Hauptschleife
//
//////////////////////////////
int main(void) {
	init();		// Komponenten Initialisieren
	while (1) { // In Endlosschleife wechseln
		wdt_reset();	// Watchdog zuruecksetzen
		if (get_key_press(1 << KEY1))	// 1 - Back
			menu_exit(&menu_context);
		if( get_key_press(1 << KEY2))	// 2 - Hoch
			menu_prev_entry(&menu_context);
		if (get_key_press(1 << KEY3))	// 3 - Runter
			menu_next_entry(&menu_context);
		if (get_key_press(1 << KEY4))	// 4 - Select
			menu_select(&menu_context);
		if ((UCSR0A & (1 << RXC0))){	// RapidForm Polling
			LED_PORT &= ( 1 << LED2 );	// LED einschalten
			uart_rx(D_RapidForm);		// Register auslesen
		}
		if ((UCSR1A & (1 << RXC1))){	// Stepper Polling
			LED_PORT &= ( 1 << LED3 );	// LED einschalten
			uart_rx(D_Stepper);			// Register auslesen
		}
	}
}
//////////////////////////////
//
//    	Hauptschleife Ende
//
//////////////////////////////

// Interrupt Stuff
ISR(WDT_vect){ 					// Watchdog ISR
		LED_PORT &=~(1 << LED4);// LED5 einschalten
		lcd_clrscr();
		lcd_puts("Something went \nterribly wrong!\nRebooting!");
}
ISR(PCINT3_vect){				// + Endschalter Position erreicht
	lcd_clrscr();
	lcd_puts("Obere\nEndposition\nErreicht!");
	LED_PORT ^= (1 << LED3);
}
ISR(PCINT2_vect){				// - Endschalter Position erreicht
	lcd_clrscr();
	lcd_puts("Untere\nEndposition\nErreicht!");
	LED_PORT ^= (1 << LED3);
}
// UART Stuff
void 	uart_init			() {// UART Initialisieren
	// UART 0 - IN (Rapidform Software/Terminal)
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
	UCSR0C = (3 << UCSZ00);
	UCSR0B |= (1 << TXEN0); //Transmitter Enabled
	UCSR0B |= (1 << RXEN0); // UART RX einschalten
	// UART 1 - OUT (Stepper Karte/Drehtisch)
	UBRR1H = UBRRH_VALUE;
	UBRR1L = UBRRL_VALUE;
	UCSR1C = (3 << UCSZ00);
	UCSR1B |= (1 << TXEN1); //Transmitter Enabled
	UCSR1B |= (1 << RXEN1); // UART RX einschalten
}
void 	uart_put_charater	(unsigned char c, int dir) { // UART Zeichen senden
	if (dir == D_RapidForm) {// To Rapidform
		while (!(UCSR0A & (1 << UDRE0))){} //warten bis Senden moeglich
		UDR0 = c; // sende Zeichen
	}
	else {// To Stepper
		while (!(UCSR1A & (1 << UDRE1))){} //warten bis Senden moeglich
		UDR1 = c; // sende Zeichen
	}
}
void 	uart_put_string		(char *s, int dir) { 		 // UART String senden
	while (*s){ // so lange *s != '\0' also ungleich dem "String-Endezeichen(Terminator)"
		uart_put_charater(*s, dir); // Zeichenweise senden
		s++;
	}
}
int 	uart_get_character	(int dir) {			 // UART Zeichen empfangen
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
void 	uart_get_string		(char * string_in, int dir) {// UART String empfangen
	char c;	// Einzelnes Zeichen
	int i = 0;	// Zaehlvariable
	do {
		c = uart_get_character(dir); // Einzelnes Zeichen holen
		if (c != '\r') {			 // Wenn keinn \r
			*string_in = c;			 // Zeichen in Empfangsstring schreiben
			string_in += 1;			 // Adresse des Empfangsstring um 1 inkrementieren
			i++;					 // Zaehlvariable um 1 erhoehen
		}
	} while (i < 100 && c != '\r' && c != '\n'); // So lange bis \r \n oder ueber 100 Zeichen
	*string_in = '\0';				 // 0 Terminieren
	if (dir == D_Stepper)
		LED_PORT |= ( 1 << LED3 );	 // "Daten Vorhanden" LED ausschalten
	else
		LED_PORT |= ( 1 << LED2 );	 // "Daten Vorhanden" LED ausschalten
}

// String Stuff
// Auffinden eines Strings aus einem vorgegebenen Array
int 	FindStringInArray	(const char* pInput, const char* pOptions[], int cmp_length) {
	int n = -1;
	while (pOptions[++n]) {	//Array durchlaufen bis 0 terminiert
		//Wenn pInput == pOptions dann gib Array Position zurueck
		if (!strncmp(pInput, pOptions[n], cmp_length))	return n;
	}
	return 99; // Wenn keine uebereinstimmung, gib 99 zurueck
}
// Strings zerlegen
void 	String_zerlegen_Isel(char * str_rx, char * Position, char * Winkel) {
	//0M5200, +600
	//Achse M Position, +Geschwindigkeit
	char * Achse="0";
	Achse[0] = str_rx[1];	// Achse setzen
	Achse[1] = '\0';
	// Ausgeben welche Achse gewaehlt wurde
	if(atoi(Achse)==0){
		lcd_puts("Achse: ");
		lcd_puts(Achse);
		lcd_puts(" (Rotation)\n");
	}
	if(atoi(Achse)==1){
		lcd_puts("Achse: ");
		lcd_puts(Achse);
		lcd_puts(" (Hoehe)   \n");
	}
	// Anzahl der Schritte aus dem String auslesen
	char c;
	int i = 0;
	do {
		c = str_rx[i + 3];
		if (c != ',') {
			Position[i] = c;
			i++;
		}
	} while (i < 20 && c != '\0' && c != ',');
	Position[i] = '\0'; // String 0 Terminieren
	int32_t z;
	int32_t y;
	z = atol(Position);	// String in Zahl(long) umwandeln
	y = z / 7200;		// Berechnung des Winkel
	z = (z * 71111)  /1024;	// Berechnung der Schritte
	ltoa(y, Winkel,		10 ); // Winekl in String umwandeln
	ltoa(z, Position,	10 ); // Schritte in String umwandeln
}
void 	String_zerlegen_csg	(char * str_rx) { 	// Unvollstaendig?
	//012 3456 78901 2345 6789 01234 5678
	//D:2 S500 F5000 R200 S500 F5000 R200.
	//D:2S500F5000R200S500F5000R200

	// Format:
	// D:[Speed range]S[Minimum speed]F[Maximum Speed]R[Acceleration/Deceleration time]
	// |------------------------------Axis1 parameters--------------------------------|
	// S[Minimum speed for Axis 2] S[Minimum speed for Axis 2]F[Maximum speed]R[Acceleration/Deceleration time]
	// |--------------------------------------Axis 2 parameters-----------------------------------------------|


	int i = 4; // Index Input String | Bei 4. Zeichen Beginnen. Die ersten 3 Zeichen sind Fix.
	int j = 0; // Index Variable
	char c; // Zu kopierendes Zeichen
	// Variablen Deklaration und Initialisierung mit Defaultwerten
	char Speed_Range[2] = "2";
	char ONE_Min_Speed[6] = "200";
	char ONE_Max_Speed[6] = "2000";
	char ONE_Acc_Speed[5] = "200";

	////////////////////////
	//
	//  Speed Range (1 || 2)
	//
	////////////////////////
	Speed_Range[0] = str_rx[2];
	Speed_Range[1] = '\0';

	////////////////////////
	//
	//  Min Speed (50 - 20000)
	//
	////////////////////////
	do {
		c = str_rx[i];
		if (c != 'F') {
			ONE_Min_Speed[j] = c;
			j++;
			i++;
		}
	} while (j < 6 && c != '\0' && c != 'F');
	ONE_Min_Speed[j] = '\0';

	lcd_puts("1_MIN_SPEED:");
	lcd_puts(ONE_Min_Speed);
	lcd_puts("\n");
	// TODO: Range ueberpruefen! 50-20000
	//uart_puts();

	////////////////////////
	//
	//  Max Speed (50 - 20000)
	//
	////////////////////////
	i++; // Stuerzeichen ueberspringen
	j = 0; // Variablenzaehler zuruecksetzen
	do {
		c = str_rx[i];
		if (c != 'R') {
			ONE_Max_Speed[j] = c;
			i++;
			j++;
		}
	} while (j < 6 && c != '\0' && c != 'R');
	ONE_Max_Speed[j] = '\0';

	lcd_puts("1_MAX_SPEED:");
	lcd_puts(ONE_Max_Speed);
	lcd_puts("\n");

	////////////////////////
	//
	//  Acceleration (0 - 1000)
	//
	////////////////////////
	i++; // Stuerzeichen ueberspringen
	j = 0; // Variablenzaehler zuruecksetzen
	do {
		c = str_rx[i];
		if (c != 'S') {
			ONE_Acc_Speed[j] = c;
			i++;
			j++;
		}
	} while (j < 4 && c != '\0' && c != 'S');
	ONE_Acc_Speed[j] = '\0';

	lcd_puts("1_ACC_SPEED:");
	lcd_puts(ONE_Acc_Speed);
	lcd_puts("\n");

	//uart_put_string("0\n", D_Stepper);
	uart_put_string(B_OK, D_RapidForm);
}
// 		Hilfs Funktionen
void 	csg_Status_melden	(void) {			// Unvollstaendig?
		uart_put_string("         0,         0,K,K,R\r\n", D_RapidForm); // Status an RapidForm zurueckmelden
}
void 	Position_Zeta		(char * Position) { // Schritte Auslesen - Zeta
    char c;
    int i = 0;
    do{
        c = str_rx[i + 1];
        if(c != ','){
            Position[i] = c;
            i++;
        }
    }
     while(i < 20 && c != '\0' && c != ',');
    Position[i] = '\0';
	int32_t z;
	z = atol(Position);
	z = z/9;
	ltoa(z,Position,10);
}
// 		Uebersetzungs Logik
void 	switch_Stepper		(char * str_rx) {	// Uebersetzung Schrittmotorkarte
	const char* pOptions[] = {	// Array mit bekannten Befehlen
			"#", 	// 0 - Stepper Karte hat Befehl erkannt
			"E", 	// 1 - Stepper Karte meldet Error
			"!CLS", // 2 - Clear Screen (Debugging)
			"Test", // 3 - Test (Debugging)
			0 };
	switch (FindStringInArray(str_rx, pOptions, 1)) { // String gegen bekannte Antworten pruefen
	case 0:			// 0 - Stepper Karte hat Befehl erkannt
		lcd_puts("Erfolgreich\n");
		break;
	case 1:			// 1 - Stepper Karte meldet Error
		lcd_puts("Error\n");
		uart_put_string("1\r\n", D_RapidForm);
		break;
	case 2:			// 2 - Clear Screen (Debugging)
		lcd_clrscr();
		break;
	case 3:			// 3 - Test (Debugging)
		lcd_puts("Test bestanden\n");
		break;
	default:
		ms_spin(10);
	}
}
void 	switch_Isel			(char * str_rx) {	// Uebersetzung Isel
	const char* pOptions[] = {
			"XXXXXXX", 	// 0 - Reserve
			"!CLS",    	// 1 - LC-Display loeschen
			"Test", 	// 2 - Test
			"@01",  	// 3 - Achse auswaehlen
			"@0R", 		// 4 - Status abfrage
			"@0M", 		// 5 - Gehe zu Position
			0 };
	switch (FindStringInArray(str_rx, pOptions, 3)) {
	case 0: 		// 0 - Reserve
		lcd_puts("Reserve\r\n");
		break;
	case 1: 		// 1 - LC-Display loeschen
		lcd_clrscr();
		break;
	case 2:			// 2 - Test
		lcd_puts("Test bestanden\n");
		uart_put_string("Test bestanden\r\n", D_RapidForm);
		break;
	case 3:			// 3 - Achse auswaehlen
		ms_spin(10);
		lcd_puts("Init");
		uart_put_string("0\r\n", D_RapidForm);
		break;
	case 4:			// 4 - Status abfrage
		lcd_puts("Statusabfrage:     \n");
		uart_put_string("A\n", D_Stepper);	// Statusabfrage an Stepper senden
		ms_spin(50);						// Verarbeitungszeit gewaehren
		if ((UCSR1A & (1 << RXC1)))			// Wenn ein Zeichen empfangen wurde
			uart_rx(D_Stepper);				// Zeichen auslesen
		if (!strcmp(str_rx,"0#"))			// Empfangenes Zeichen ueberpruefen
			uart_put_string("0\r\n", D_RapidForm); // Antwort Ok an RapidForm melden
		else {
			lcd_puts("Fehlgeschlagen     \n");	   // Fehler auf Display anzeigen
			uart_put_string("1\r\n", D_RapidForm); // Fehler an RapidForm melden
		}
		break;
	case 5:			// 5 - Gehe zu Position MX , +600
		ms_spin(10);
		char Position[33], Winkel[6];
		memset(Position, '\0', 33); // Strign 0 Terminiert vorbelegen
		memset(Winkel, '\0', 6);    // String 0 Terminiert vorbelegen
		String_zerlegen_Isel(str_rx, Position, Winkel); // String auswerten
		// String fuer Stepper vorbereiten
		char Move_To[40];
		memset(Move_To,  '\0', 40);
		Move_To[0] = 'M';
		Move_To[1] = 'A';
		Move_To[2] = ' ';
		Move_To[3] = '\0';
		strcat(Move_To, Position);
		strcat(Move_To, "\n");
		lcd_puts("Pos:");
		lcd_puts(Move_To);
		// String an Stepper senden
		uart_put_string(Move_To, D_Stepper);
		ms_spin(50);
		if ((UCSR1A & (1 << RXC1)))
			uart_rx(D_Stepper);	// Antwort des Stepper auslesen
		else {
			break; // Bei Fehler abbrechen
		}
		// Status des Stepper Abfragen
		uart_put_string("A\n", D_Stepper);
		ms_spin(50);
		// Antwort des Stepper Abfragen
		if ((UCSR1A & (1 << RXC1)))
			uart_rx(D_Stepper);
		else {
			lcd_puts("Keine Bewegung!\n");
		}
		// So lange der Stepper Bewegung meldet erneut Statusabfrage
		while (!strcmp(str_rx,"1#")){
			// Statusabfrage an Stepper
			uart_put_string("A\n", D_Stepper);
			ms_spin(50);
			// Statusabfrage auslesen und auswerten
			if ((UCSR1A & (1 << RXC1))){
				uart_rx(D_Stepper);
				lcd_clrscr();
				lcd_puts("Gehe zu Winkel: ");
				lcd_puts(Winkel);
				lcd_puts("\n");
			}
			else {
				lcd_puts("Keine Antwort\n");
			}
			wdt_reset();
		}
		lcd_puts("Winkel: ");
		lcd_puts(Winkel);
		lcd_puts(" Erreicht\n");
		// Bewegung abgeschlossen an RapidForm melden
		uart_put_string("0\r\n", D_RapidForm);
		break;
	default: // Unbekannte Befehle auf dem Display anzeigen
		lcd_puts(str_rx);
	}
}
void 	switch_csg			(char * str_rx) {	// Uebersetzung CSG Unvollstaendig?
	const char* pOptions[] = {
			"Test2", // 0 - Stepper Karte Befehl erkannt
			"!CLS", // 1 - LC-Display loeschen
			"Test", // 2 - Test
			"Q:", // 3 - Status abfrage
			"D:2", // 4 - D:2S500F5000R200S500F5000R200.
			"H:", // 5 - H:
			"G", // 6 - Motor starten
			"M:", // 7 - Move by Pulses
			"!", // 8 - Busy Ready ?
			"H1",
			0 };
	switch (FindStringInArray(str_rx, pOptions, 2)) {
	case 0: // Motorkarte Erfolgreich angesprochen
		lcd_puts("!");
		break;
	case 1: // Display loeschen
		lcd_clrscr();
		break;
	case 2: // Interner Test
		lcd_puts("!T");
		//uart_puts("Test bestanden\n\r");
		break;
	case 3: // Status abfrage von Software
		lcd_puts("Statusabfrage     \n");
		csg_Status_melden();
		break;
	case 4:
		String_zerlegen_csg(str_rx);

		break;
	case 5:
		lcd_puts("H:                 \n");
		uart_put_string(B_OK, D_RapidForm);
		break;
	case 6:
		lcd_puts("Motor starten\n");
		//uart_put_string(B_OK, D_RapidForm);
		break;
	case 7:
		move++;
		char it[10];
		itoa(move, it, 10);
		lcd_puts(it);
		lcd_puts("_Move!\n");
		uart_put_string("M 160000\r\n",D_Stepper);

		break;
	case 8:
		lcd_puts("R/B?");
		uart_put_string("R\r\n", D_RapidForm);
		break;
	case 9:
		lcd_puts("H1 empfangen       \n");
		break;
	default:
		lcd_puts("U_B: ");
		lcd_puts(str_rx);
		lcd_puts("!END       \n");
	}
}
void 	switch_Zeta			(char * str_rx) {	// Uebersetzung Zeta
	const char* pOptions[] = {
			"!CLS", // 0 - LC-Display loeschen
			"Test",	// 1 - Test
			"GO", 	// 2 - Motor Starten
			"WAIT",	// 3 - Wait till motor stops
			"!XXXX",// 4 - Reserve
			"COMEX",// 5 - *COMEXC0
			"MA1", 	// 6 - Absolute Positioning
			"D1125",// 7 - Position
			"A8", 	// 8 - Accelartion 8
			"V8",	// 9 - Velocity 8
			"ECHO0",// 10 - Echo abschalten
			"PSET0",// 11 - Ursprung setzen
			0 };
	char Position[33];
	char Move_To[40];
	memset(Move_To,  '\0', 40);
	Move_To[0] = 'M';
	Move_To[1] = 'A';
	Move_To[2] = ' ';
	Move_To[3] = '\0';
	switch (FindStringInArray(str_rx, pOptions, 1)) {
	case 0: // Display loeschen
		lcd_clrscr();
		break;
	case 1: // Interner Test
		lcd_puts("Test bestanden     \n");
		break;
	case 2: // Go
		ms_spin(100);
		strcat(Move_To, Position);
		strcat(Move_To, "\n");

		uart_put_string(Move_To, D_Stepper);
		ms_spin(50);
		if ((UCSR1A & (1 << RXC1)))
			uart_rx(D_Stepper);
		else {
			lcd_puts("Befehl n. bestaetig\n");
			break;
		}

		uart_put_string("A\n", D_Stepper);
		ms_spin(50);
		if ((UCSR1A & (1 << RXC1)))
			uart_rx(D_Stepper);
		else {
			lcd_puts("Keine Bewegung!\n");
		}

		while (!strcmp(str_rx,"1#")){
			uart_put_string("W\n", D_Stepper);
			ms_spin(100);
			if ((UCSR1A & (1 << RXC1))){
				uart_rx(D_Stepper);
				lcd_clrscr();
				lcd_puts("Position(Akt/Ges): \n");
				lcd_puts(str_rx);
				lcd_puts(" / ");
			}
			else {
				lcd_puts("Keine Antwort\n");
			}
			wdt_reset();

			uart_put_string("A\n", D_Stepper);
			ms_spin(50);
			if ((UCSR1A & (1 << RXC1))){
				uart_rx(D_Stepper);
				//lcd_clrscr();
				//lcd_puts("running to\n");
				//lcd_puts("Position: ");
				lcd_puts(Position);
				lcd_puts("\n");
			}
			else {
				lcd_puts("Keine Antwort\n");
			}
			wdt_reset();
		}
		lcd_puts("Position: \n");
		lcd_puts(Position);
		lcd_puts(" Erreicht\n");
		uart_put_string(B_Zeta, D_RapidForm);
		break;
	case 3: // WAIT
		break;
	case 4: // Reserve
		break;
	case 5: // COMEXC0
		break;
	case 6:
		//lcd_puts("MA1 empfangen     \n");
		break;
	case 7: // Position Setzen
		memset(Position, '\0', 33);			// Array mit Nullen befuellen
		Position_Zeta(Position);
		break;
	case 8:
		break;
	case 9:		//V8
		lcd_puts("Speed set          \n");
		//uart_put_string(B_Zeta_Return, D_RapidForm);
		break;
	case 10:
		lcd_puts("Echo off           \n");
		//uart_put_string(str_rx, D_RapidForm);
		//uart_put_string("ECHO0\r", D_RapidForm);
		break;
	case 11:
		break;
	default:
		lcd_puts("Z:");
		lcd_puts(str_rx);
		lcd_puts("   \n");
		//Initialized = switch_Inputs(str_rx);
	}
}
void 	switch_Terminal		(char * str_rx) {	// Uebersetzung Terminal Unvollstaendig
	const char* pOptions[] = {
			"!CLS", // 0 - LC-Display loeschen
			"Test",	// 1 - Test
			"!Manual",// 2 - Ignorieren
			"!YYYY",// 3 - Wait till motor stops
			0 };

	if (init_T == 0){
		init_T = 1;
		uart_put_string("Willkommen im Terminal Modus\r\n",D_RapidForm);
		uart_put_string("moegliche Befehle sind: \r\n",D_RapidForm);
		uart_put_string(" A - Motorstatus\r\n M - Move Steps\r\n", D_RapidForm);
	}
	switch (FindStringInArray(str_rx, pOptions, 2)) {
	case 0: // Display loeschen
		lcd_clrscr();
		break;
	case 1: // Interner Test
		lcd_puts("Test bestanden     \n");
		uart_put_string("Test bestanden", D_RapidForm);
		break;
	case 2: // Reserve 1

	case 3: // Reserve 2

		break;
	default:
		//lcd_puts("Z:");
		lcd_puts(str_rx);
		lcd_puts("       \n");
		uart_put_string(str_rx,D_Stepper);
		uart_put_string("\n",D_Stepper);
	}
}
int 	switch_Motor		(char * str_rx) {	// Automatische Befehlssatzwahl
	const char* pOptions[] = {	// Array mit Initialisierungsbefehlen
			"@01", 		// 0 - Isel
			"Q:",    	// 1 - CSG
			"ECHO0", 	// 2 - Zeta
			"!Terminal",	// 3 - Terminal ansteuerung!
			0 };
	// Ankommenden String gegen Array pruefen
	switch (FindStringInArray(str_rx, pOptions, 3)) {
	case 0: 		// 0 - ISEL
		return M_ISEL;
		break;
	case 1: 		// 1 - CSG
		return M_CSG;
		break;
	case 2:			// 2 - Zeta
		return M_ZETA;
		break;
	case 3:			// 3 - Terminal ansteuerung
		return M_TERMINAL;
		break;
	default:
		return M_UNK;
	}
}
void 	uart_rx				(int dir) {			// UART Empfangsregister auslesen
	uart_get_string(str_rx, dir);	// String aus Empfangsregister auslesen
	if (dir == D_Stepper)			// Empfangsregister Stepper
		switch_Stepper(str_rx);		// Uebersungsfunktion fuer Stepper aufrufen
	else{							// Empfangsregsiter RapidForm
		// Uebersetzungsfunktion auswaehlen
		if(Initialized == M_UNK){	// Unbekannter Initialisierungsbefehl
			lcd_puts("Unbekannter Motor!\n");
			Initialized = M_NOTI;	// Variable Initialized zuruecksetzen
		}
		if(Initialized == M_NOTI){	// Befehlssatz bestimmen
			Initialized = switch_Motor(str_rx); //Automatische Befehlssatzwahl
		}
		if(Initialized == M_ISEL)	// Uebersetzung ISEL
			switch_Isel(str_rx);
		if(Initialized == M_CSG)	// Uebersetzung CSG
			switch_csg(str_rx);
		if(Initialized == M_ZETA)	// Uebersetzung Zeta
			switch_Zeta(str_rx);
		if(Initialized == M_TERMINAL)// Uebersetzung Terminal
			switch_Terminal(str_rx);
	}
}
// 		LCD und LED Stuff
void 	lcd_my_type			(char *s) {			// Zeichen auf Display ausgeben
	// Spielerei!
	// Zeichen mit unterschiedlicher Geschwindigkeit ausgeben
	// Dies Simuliert einen Menschlichen Benutzer...
	srand(TCNT0);
	int min = 10;
	int max = 250;
	int erg = 0;
	while (*s) // so lange *s != '\0' also ungleich dem "String-Endezeichen(Terminator)"
	{
		erg = (rand() % (max - min + 1) + min);
		lcd_putc(*s);
		s++;
		for (int i = 0; i < erg; i++)
			_delay_ms(1);
	}
}
void 	lcd_boot			(void) {			// Boot: Nachricht ausgeben
	_delay_ms(100);
	lcd_my_type("Guten Tag!\n");
	_delay_ms(400);
	lcd_my_type("Bereit!\n");
	_delay_ms(400);
	lcd_clrscr();
}
void 	led_boot			(void) {			// Boot: LEDs durchlaufen
	for (int i = 1; i < 9; i++) {
		_delay_ms(80); 					// warte 80ms
		LED_PORT &= ~((1 << i)); 	   	// loescht Bit an PortB - LED an
		LED_PORT |=  ((1 << (i - 1))); 	// setzt  Bit an PortB - LED aus
	}
}
void 	debounce_init		(void) {			// Taster entprellen
	KEY_DDR &= ~ALL_KEYS; // configure key port for input
	KEY_PORT |= ALL_KEYS; // and turn on pull up resistors
	TCCR0B = (1 << CS02) | (1 << CS00); // divide by 1024
	TCNT0 = (uint8_t) (int16_t) -(F_CPU / 1024 * 10 * 10e-3 + 0.5); // preload for 10ms
	TIMSK0 |= 1 << TOIE0; // enable timer interrupt
	sei();
}
void 	led_lauflicht 		(void) {			// LED Lauflicht
	uint8_t i = LED_PORT;
	i = (i & 0x00) | ((i << 1) & 0xFE);
	if (i < 0xFE) i |= 0x01;
	LED_PORT = i;
}
// 		Menu Stuff
void 	mod_manual			(void *arg, void *name) { // Manuelle Aufnahme
	lcd_puts("Manueller Modus\n");
	lcd_puts("Aufnahme starten!\n");
	lcd_puts("Danach Select\n");
	lcd_puts("-> Drehung um 45\n");
	if (get_key_press(1 << KEY4))
	uart_put_string("M 55750\r", D_Stepper);
}
void 	my_select			(void *arg, char *name) { // Deprecated?
	lcd_clrscr();
	lcd_puts("Selected: ");
	lcd_puts(name);
	ms_spin(750);
}
void 	menu_puts			(void *arg, char *name) { // Menu/Sende Funktion
	uart_put_string(arg, D_Stepper);	// Uebergebenen String an Stepper senden
	// Befehl auf Display ausgeben
	lcd_clrscr();
	lcd_puts("Sent: ");
	lcd_puts(arg);
	lcd_puts("\n");
	ms_spin(100);
	//if ((UCSR1A & (1 << RXC1)))
	uart_rx(D_Stepper);	// Antwort des Stepper empfangen
	ms_spin(1000);		// Antwort noch eine weile Anzeigen
}
// Init Stuff
void 	init_WDT			(void) {			// Watchdog Initialisieren
	// Vordefinierte Sequenz aus Anleitung
	cli();
	wdt_reset();
	WDTCSR |= (1 << WDCE) | (1 << WDE);
	WDTCSR = (1 << WDE) | (1 << WDIE) | (1 << WDP3) | (1 << WDP0); //Watchdog 8s
	//WDTCSR = 0x0F; //Watchdog Off
	sei();
}
void 	init				(void) {			// Initialisierung durchlaufen
	init_WDT();						// Watchdog Initialisieren oder Abschalten
	LED_DDR   = 0xFF;				// LED Port Richtung definieren (Ausgang)
	LED_PORT  = 0xFF;				// LEDs ausschalten
	PCMSK3   |= ( 1 << PCINT28 ); 	// Interrupts definierenPD4 als Interrupt zulassen
	PCICR    |= ( 1 << PCIE3   ); 	// Pin Change Interrupt Control Register - PCIE3 setzen fuer PCINT30
	DDRC     |= ( 1 << PB7     );	// Pin7 (Kontrast) als Ausgang definieren 	(Nur LCD an STK500)
	LCD_PORT &= ( 1 << PB7 	   );  	// Pin7 auf 0V legen 						(Nur LCD an STK500)
	lcd_init(LCD_DISP_ON_CURSOR);	// LC Display initialisieren
	lcd_boot();						// Kurze Startup Meldung zeigen
	led_boot();						// Starten des Mikrocontroller kennzeichnen
	debounce_init();				// Taster entprellen
	uart_init();					// RS-232 Verbindung initialisieren
	menu_enter(&menu_context, &menu_main); // Kommentar entfernen um Menue zu aktivieren
}
