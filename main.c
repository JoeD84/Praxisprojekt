/*
 * main.c
 *
 * Created: 05.10.2011 17:36:28
 *  Author: JoeD
 */
#define BAUD 9600

#ifndef F_CPU
#define F_CPU 8000000
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>
#include <stdlib.h>
#include "mystuff.h"
#include "Debounce.h"
#include "lcd.h"
#include <string.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <string.h>
#include <avr/pgmspace.h>

#define B_OK "0\r\n"
#define D_RapidForm 0
#define D_Stepper 	1

#define MCU_CLK F_CPU
#include "tinymenu/spin_delay.h"

#define CONFIG_TINYMENU_USE_CLEAR
#include "tinymenu/tinymenu.h"
#include "tinymenu/tinymenu_hw.h"

int blub = 0;
int move = 0;
char str_rx[100];

// UART Stuff
void 	uart_init			();
void 	uart_put_charater	(unsigned char c, int dir);
void 	uart_put_string		(char *s, int dir);
int  	uart_get_character	(int dir);
void 	uart_get_string		(char * string_in, int dir);
void 	uart_rx				(int dir);
// String Stuff
int 	FindStringInArray	(const char* pInput, const char* pOptions[], int cmp_length);
void 	String_zerlegen_Isel(char * str_rx, char * Position);
void 	String_zerlegen_csg	(char * str_rx);
// Hilfs Funktionen
void 	csg_Status_melden	();
// Auswerte Logik
int 	switch_Motor		(char * str_rx);
void 	switch_Stepper		(char * str_rx);
void 	switch_Isel			(char * str_rx);
void 	switch_csg			(char * str_rx);
// LCD und LED Stuff
void 	lcd_my_type			(char *s) {
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
void 	lcd_spielereien		(void) {
	_delay_ms(100);
	lcd_my_type("Hello Joe!\n");
	_delay_ms(200);
	lcd_my_type("Have a nice Day!\n");
	_delay_ms(500);
	lcd_my_type("Be a Honey Bee!\n");
	_delay_ms(600);
	lcd_clrscr();
}
void 	led_spielerein		(void) {

	for (int i = 1; i < 9; i++) // LEDs durchlaufen
	{
		_delay_ms(80); // Eine Sekunde +/-1/10000 Sekunde warten...
		LED_PORT &= ~((1 << i)); // löscht Bit an PortB - LED an
		LED_PORT |= ((1 << (i - 1))); // setzt  Bit an PortB - LED aus
		//wdt_reset();
	}
}
void 	debounce_init		(void) {
	///////////////// Debounce Stuff ////////////////////

	// Configure debouncing routines
	KEY_DDR &= ~ALL_KEYS; // configure key port for input
	KEY_PORT |= ALL_KEYS; // and turn on pull up resistors

	TCCR0B = (1 << CS02) | (1 << CS00); // divide by 1024
	TCNT0 = (uint8_t) (int16_t) -(F_CPU / 1024 * 10 * 10e-3 + 0.5); // preload for 10ms
	TIMSK0 |= 1 << TOIE0; // enable timer interrupt

	sei();
	///////////////// Debounce Stuff ////////////////////
}
void 	led_lauflicht		(void) {
	uint8_t i = LED_PORT;
	i = (i & 0x07) | ((i << 1) & 0xF0);
	if (i < 0xF0)
		i |= 0x08;
	LED_PORT = i;
}
// Menu Stuff

void 	mod_manual			(void *arg, void *name) {
	lcd_puts("Manueller Modus\n");
	lcd_puts("Aufnahme starten\n");
	lcd_puts("Nach Aufnahme SW3 drücken!\n");
	if (get_key_press(1 << KEY3))
		uart_put_string("M 16000\r", D_Stepper);
}
void 	my_select			(void *arg, char *name) {
	lcd_clrscr();
	lcd_puts("Selected: ");
	lcd_puts(name);

	ms_spin(750);
}
void 	menu_puts			(void *arg, char *name) {
	//my_select(arg, name);
	uart_put_string(arg, D_Stepper);
	lcd_clrscr();
	lcd_puts("Send: ");
	lcd_puts(arg);
	lcd_puts("\n");
	ms_spin(100);
	//if ((UCSR1A & (1 << RXC1)))
	uart_rx(D_Stepper);
	ms_spin(1000);
}
#include "mymenu.h"

// Init Stuff
void init_WDT(void) {
	cli();
	wdt_reset();
	WDTCSR |= (1 << WDCE) | (1 << WDE);
	WDTCSR = (1 << WDE) | (1 << WDIE) | (1 << WDP3) | (1 << WDP0); //Watchdog 8s
	//WDTCSR = 0x0F; //Watchdog Off
	sei();
}

void 	init				(void);


//////////////////////////////
//
//      Hauptschleife
//
//////////////////////////////
int main(void) {
	init();
	while (1) {
		wdt_reset();
		if (get_key_press(1 << KEY0) || get_key_rpt(1 << KEY0))
			led_lauflicht(); // LED Lauflicht
		if (get_key_press(1 << KEY1))
			uart_put_string("0\n", D_RapidForm);
		if( get_key_press( 1<<KEY2 ) )
			lcd_clrscr();


		if (get_key_press(1 << KEY3)) {
			lcd_puts("Betrete Menü!\n");
			menu_enter(&menu_context, &menu_main);
		}

		if (get_key_press(1 << KEY4))
			menu_select(&menu_context); // 4 - Auswählen
		if (get_key_press(1 << KEY5) || get_key_rpt(1 << KEY5)) // 5 - Next
			menu_next_entry(&menu_context);
		if (get_key_press(1 << KEY6) || get_key_rpt(1 << KEY6)) // 6 - Previous
			menu_prev_entry(&menu_context);
		if (get_key_press(1 << KEY7))
			menu_exit(&menu_context); // 7 - Menü zurück

		if ((UCSR0A & (1 << RXC0)))
			uart_rx(D_RapidForm);
		if ((UCSR1A & (1 << RXC1)))
			uart_rx(D_Stepper);
	}
}
//////////////////////////////
//
//    Hauptschleife Ende
//
//////////////////////////////

// Interrupt Stuff
ISR(WDT_vect)
//Interrupt Service Routine
{
		LED_PORT ^= (1 << LED0);
}
ISR(PCINT3_vect)
//Interrupt Service Routine
{
	//led_lauflicht();
	//uart_put_string("2H\n", D_Stepper);
	uart_put_string("1H\n", D_Stepper);
	LED_PORT ^= (1 << LED0);
}
// UART Stuff
void 	uart_init			() {
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
void 	uart_put_charater	(unsigned char c, int dir) {
	// To Rapidform
	if (dir == D_RapidForm) {
		while (!(UCSR0A & (1 << UDRE0))) //warten bis Senden moeglich
		{
		}
		UDR0 = c; // sende Zeichen
	}
	// To Stepper
	else {
		while (!(UCSR1A & (1 << UDRE1))) //warten bis Senden moeglich
		{
		}
		UDR1 = c; // sende Zeichen
	}
	//return 0;
}
void 	uart_put_string		(char *s, int dir) {
	while (*s) // so lange *s != '\0' also ungleich dem "String-Endezeichen(Terminator)"
	{
		uart_put_charater(*s, dir);
		s++;
	}
}
int 	uart_get_character	(int dir) {
	if (dir == D_RapidForm) {
		while (!(UCSR0A & (1 << RXC0)))
			// warten bis Zeichen verfuegbar
			;
		return UDR0; // Zeichen aus UDR an Aufrufer zurueckgeben
	}
	if (dir == D_Stepper) {
		while (!(UCSR1A & (1 << RXC1)))
			// warten bis Zeichen verfuegbar
			;
		return UDR1; // Zeichen aus UDR an Aufrufer zurueckgeben
	}
	return -1;
}
void 	uart_get_string		(char * string_in, int dir) {
	char c;
	int i = 0;
	do {
		c = uart_get_character(dir);
		if (c != '\r') {
			*string_in = c;
			string_in += 1;
			i++;
		}
	} while (i < 100 && c != '\r' && c != '\n');
	*string_in = '\0';
}

// String Stuff


#define M_ISEL 		0
#define M_CSG  		1
#define M_ZETA 		2
#define M_TERMINAL	3
#define M_NOTI		-1
#define M_UNK		-2

#define P_INIT 		0
#define P_FINISH 	1
#define P_AROT 		2
#define P_STOP		3
#define P_HOME		4
#define P_STEP		5
#define P_TIMEOUT	6

#define E_CLS		10
#define E_TEST		11


#define MENU_ENTRY_NAMELEN 19
#define RETURN_LEN 40






typedef struct Entry_s {
	char Name[19];           					// Name zum Anzeigen
	char Input[40];							// Vergleichswert
	char Output[40];                 			// Ausgabebefehl
} PROGMEM Entry_t;											// Ergeben Struct P_Entry_t

typedef struct Motor_s {
	uint8_t 	num_Befehle;
	Entry_t 	*Befehl;						// 4 Motoren vom Typ Befehle_t
} Motor_t;											// Ergeben Struct Motor_t

typedef struct Protokoll {
	uint8_t 	num_Motor;
	Motor_t 	Motor[4];
} Protokoll_t;

Entry_t progmem_Befehl[] = {   // <===
	{	// Befehl[0] Init
		.Name = "Init\n",
		.Input = "@01",
		.Output = "0\r\n",
	},
	{	// Befehl[1] Home
		.Name = "Home\n",
		.Input = "@01",
		.Output = "0\r\n",
	},
};

Protokoll_t Protokoll = {
	.num_Motor = 4,
	.Motor[M_ISEL] = {	// Motor[0] Isel
			.num_Befehle = 7,
			.Befehl = progmem_Befehl,
	},
	.Motor[M_ZETA] = {
			.num_Befehle = 7,
			.Befehl = progmem_Befehl,
	}
};


int 	FindStringInArray	(const char* pInput, const char* pOptions[], int cmp_length) {
	int n = -1;
	while (pOptions[++n]) {
		//lcd_puts(pOptions[n]);
		//lcd_puts("\n");
		if (!strncmp(pInput, pOptions[n], cmp_length)){
			return n;
		}
	}
	return 99;
}
void 	String_zerlegen_Isel(char * str_rx, char * Position) {
	//0M5200, +600
	//Achse M Position, +Geschwindigkeit
	char * Achse="0";
	Achse[0] = str_rx[1];
	Achse[1] = '\0';
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
	lcd_puts("Test: ");
	lcd_puts(Position);
	char c;
	int i = 0;
	do {
		c = str_rx[i + 3];
		if (c != ',') {
			Position[i] = c;
			i++;
		}
	} while (i < 20 && c != '\0' && c != ',');
	Position[i] = '\0';
	int32_t z;
	z = atoi(Position);
	z = (z * 71111)  /4096;
	ltoa(z,Position,10);
	//lcd_puts("Position: ");
	//lcd_puts(Position);
	//lcd_puts("\n");
}
void 	String_zerlegen_csg	(char * str_rx) {
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
	// TODO: Range überprüfen! 50-20000
	//uart_puts();

	////////////////////////
	//
	//  Max Speed (50 - 20000)
	//
	////////////////////////
	i++; // Stuerzeichen überspringen
	j = 0; // Variablenzähler zurücksetzen
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
	i++; // Stuerzeichen überspringen
	j = 0; // Variablenzähler zurücksetzen
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

	//////////////////////////
	//
	//  Blub um eins hochzählen
	//
	//////////////////////////
	blub++;
	//uart_put_string("0\n", D_Stepper);
	uart_put_string(B_OK, D_RapidForm);
}
// Hilfs Funktionen
void 	csg_Status_melden	() {
	int k = 0;
	if (blub < 100000)
		k = 5;
	if (blub < 10000)
		k = 4;
	if (blub < 1000)
		k = 3;
	if (blub < 100)
		k = 2;
	if (blub < 10)
		k = 1;
	if (blub == 0)
		k = 0;
	blub++;
	switch (k) {
	case 0:
		lcd_puts("Status gemeldet!");
		uart_put_string("         0,         0,K,K,R\r\n", D_RapidForm);			// Status an RapidForm zurückmelden
		break;
	case 1:
		uart_put_string("         9,         0,K,K,R\r\n", D_RapidForm);			// Status an RapidForm zurückmelden
		break;
	case 2:
		uart_put_string("        99,         0,K,K,R\r\n", D_RapidForm);			// Status an RapidForm zurückmelden
		break;
	case 3:
		uart_put_string("       999,         0,K,K,R\r\n", D_RapidForm);			// Status an RapidForm zurückmelden
		break;
	default:
		uart_put_string(" 999999999,         0,K,K,R\r\n", D_RapidForm); // Status an RapidForm zurückmelden
	}
}
void 	Position_Zeta		(char * Position) {
    char c;
    int i = 0;
    do{
        c = str_rx[i + 2];
        if(c != ','){
            Position[i] = c;
            i++;
        }
    }
     while(i < 20 && c != '\0' && c != ',');
    Position[i] = '\0';
    int32_t z;

    //z = atol(Position);
    //z = z / 9;
    //ltoa(z, Position, 10);
    lcd_puts("Position: ");
    lcd_puts(Position);
    lcd_puts("\n");
    ms_spin(1000);
}
// Vearbeitungs Logik

int Initialized = M_NOTI;
void 	switch_Stepper		(char * str_rx) {
	const char* pOptions[] = {
			"#", 	// 0 - Stepper Karte Befehl erkannt
			"E", 	// 1 - Error
			"!CLS", // 2 - Clear Screen
			"Test", // 3 - Test
			0 };
	switch (FindStringInArray(str_rx, pOptions, 1)) {
	case 0:
		lcd_puts("Erfolgreich\n");
		//uart_put_string("0\n\r", D_RapidForm);
		break;
	case 1:
		lcd_puts("Error\n");
		uart_put_string("1\r\n", D_RapidForm);
		break;
	case 2:
		lcd_clrscr();
		break;
	case 3:
		lcd_puts("Test bestanden\n");
		//uart_put_string("Test bestanden\n\r", D_RapidForm);
		break;
	default:
		lcd_puts("Antwort: ");
		lcd_puts(str_rx);
		lcd_puts("!\n");
		//uart_put_string(str_rx, D_RapidForm);
	}
}
void 	switch_Isel			(char * str_rx) {
	const char* pOptions[] = {
			"XXXXXXX", 	// 0 - Reserve
			"!CLS",    	// 1 - LC-Display löschen
			"Test", 	// 2 - Test
			"@01",  	// 3 - Achse auswählen
			"@0R", 		// 4 - Status abfrage
			"@0M", 		// 5 - Gehe zu Position MX , +600
			0 };

	int Ret_Val = FindStringInArray(str_rx, pOptions, 3);
	switch (Ret_Val) {
	case 0: 		// 0 - Reserve
		lcd_puts("Reserve\r\n");
		break;
	case 1: 		// 1 - LC-Display löschen
		lcd_clrscr();
		break;
	case 2:			// 2 - Test
		lcd_puts("Test bestanden\n");
		uart_put_string("Test bestanden\r\n", D_RapidForm);
		//lcd_puts(Protokoll.Motoren.M_Motor[M_ISEL].P_Init);
		break;
	case 3:			// 3 - Achse auswählen
		ms_spin(10);
		/*
	    char buf[32];
	    PGM_P p;
	    int i;

	    memcpy_P(&p, &Protokoll.Motor[M_ISEL].Befehl[0].Name[0], sizeof(PGM_P));
	    strcpy_P(buf, p);
	    */
        /*
		char string_in[40];
		char c;

		char * str_in_p = &string_in;

		do{
			c = pgm_read_byte( s_ptr );
			*str_in_p = c;
			str_in_p += 1;
			s_ptr++; // Increase string pointer
		} while( pgm_read_byte( s_ptr ) != 0x00 );  // End of string
		*/

		//lcd_puts( buf );
		lcd_puts("Init");
		//String_zerlegen_Isel(str_rx, Position);
		uart_put_string("0\r\n", D_RapidForm);
		//uart_put_string(Protokoll.Motor[M_ISEL].Befehl[0].Output, D_RapidForm);
		break;
	case 4:			// 4 - Status abfrage
		lcd_puts("Statusabfrage:     \n");
		uart_put_string("A\n", D_Stepper);
		ms_spin(50);
		if ((UCSR1A & (1 << RXC1)))
			uart_rx(D_Stepper);
		if (!strcmp(str_rx,"0#"))
			uart_put_string("0\r\n", D_RapidForm);
		else {
			lcd_puts("Fehlgeschlagen     \n");
			uart_put_string("-1\r\n", D_RapidForm);
		}
		break;
	case 5:			// 5 - Gehe zu Position MX , +600
		ms_spin(10);
		char Position[33];
		memset(Position, '\0', 33);
		String_zerlegen_Isel(str_rx, Position);
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

		uart_put_string(Move_To, D_Stepper);
		ms_spin(50);
		if ((UCSR1A & (1 << RXC1)))
			uart_rx(D_Stepper);
		else {
			//lcd_puts("Befehl n. bestaetig\n");
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
			uart_put_string("A\n", D_Stepper);
			ms_spin(50);
			if ((UCSR1A & (1 << RXC1))){
				uart_rx(D_Stepper);
				lcd_clrscr();
				lcd_puts("running to\n");
				lcd_puts("Position: ");
				lcd_puts(Position);
			}
			else {
				lcd_puts("Keine Antwort\n");
			}
			wdt_reset();
		}
		lcd_puts("Position: \n");
		lcd_puts(Position);
		lcd_puts(" Erreicht\n");
		uart_put_string("0\r\n", D_RapidForm);
		break;
	default:
		lcd_puts("ISEL:    \n");
		lcd_puts(str_rx);
	}
}
void 	switch_csg			(char * str_rx) {
	const char* pOptions[] = {
			"Test2", // 0 - Stepper Karte Befehl erkannt
			"!CLS", // 1 - LC-Display löschen
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
	case 1: // Display löschen
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
void 	switch_Zeta			(char * str_rx) {
	const char* pOptions[] = {
			"!CLS", // 0 - LC-Display löschen
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
	case 0: // Display löschen
		lcd_clrscr();
		break;
	case 1: // Interner Test
		lcd_puts("Test bestanden     \n");
		break;
	case 2: // Go
		ms_spin(100);
		strcat(Move_To, Position);
		strcat(Move_To, "\n");
		//lcd_puts("Pos:");
		//lcd_puts(Move_To);

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
			uart_put_string("A\n", D_Stepper);
			ms_spin(50);
			if ((UCSR1A & (1 << RXC1))){
				uart_rx(D_Stepper);
				lcd_clrscr();
				lcd_puts("running\n");
			}
			else {
				lcd_puts("Keine Antwort\n");
			}
			wdt_reset();
		}
		lcd_puts("Position: \n");
		lcd_puts(Position);
		lcd_puts(" Erreicht\n");
		uart_put_string("\r\n>\040\r\n>\040\r\n>\040\r\n>\040", D_RapidForm);
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
		memset(Position, '\0', 33);			// Array mit Nullen befüllen
		Position_Zeta(Position);
		break;
	case 8:
		break;
	case 9:
		//lcd_puts("Speed set");
		break;
	case 10:
		//lcd_puts("Echo off           \n");
		//uart_put_string(str_rx, D_RapidForm);
		uart_put_string("ECHO0\r", D_RapidForm);
		break;
	case 11:
		break;
	default:
		lcd_puts("Z:");
		lcd_puts(str_rx);
		lcd_puts("       \n");
		//Initialized = switch_Inputs(str_rx);
	}
}
int init_T = 0;
void 	switch_Terminal			(char * str_rx) {
	const char* pOptions[] = {
			"!CLS", // 0 - LC-Display löschen
			"Test",	// 1 - Test
			"!Manual",// 2 - Ignorieren
			"!YYYY",// 3 - Wait till motor stops
			0 };

	if (init_T == 0){
		init_T = 1;
		uart_put_string("Willkommen im Terminal Modus\r\n",D_RapidForm);
		uart_put_string("mögliche Befehle sind: \r\n",D_RapidForm);
		uart_put_string(" A - Motorstatus\r\n M - Move Steps\r\n", D_RapidForm);
	}
	switch (FindStringInArray(str_rx, pOptions, 2)) {
	case 0: // Display löschen
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

int 	switch_Motor		(char * str_rx) {
	const char* pOptions[] = {
			"@01", 		// 0 - Isel
			"Q:",    	// 1 - CSG
			"ECHO0", 	// 2 - Zeta
			"!Terminal",	// 3 - Terminal ansteuerung!
			0 };
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
void 	uart_rx				(int dir) {
	uart_get_string(str_rx, dir);
	if (dir == D_Stepper)
		switch_Stepper(str_rx);
	else{
		if(Initialized == M_UNK){
			lcd_puts("Unbekannter Motor!\n");
			//lcd_puts(str_rx);
			Initialized = M_NOTI;
		}
		if(Initialized == M_NOTI){
			Initialized = switch_Motor(str_rx);
		}
		if(Initialized == M_ISEL)
			switch_Isel(str_rx);
		if(Initialized == M_CSG)
			switch_csg(str_rx);
		if(Initialized == M_ZETA)
			switch_Zeta(str_rx);
		if(Initialized == M_TERMINAL)
			switch_Terminal(str_rx);
	}
}



void init() {
	// Watchdog Initialisieren oder Abschalten
	init_WDT();
	// LED Port definieren
	LED_DDR = 0xFF;
	LED_PORT = 0xFF;
	// Interrupts definieren
	PCMSK3 |= (1 << PCINT28); // PD4 als Interrupt zulassen
	PCICR |= (1 << PCIE3); //Pin Change Interrupt Control Register - PCIE3 setzen für PCINT30
	// Startup kennzeichnen
	led_spielerein();
	// LC Display initialisieren
	lcd_init(LCD_DISP_ON_CURSOR);
	lcd_clrscr();
	lcd_home();
	//lcd_spielereien();
	// Taster entprellen
	debounce_init();
	// RS-232 Verbindung initialisieren
	uart_init();
	//menu_enter(&menu_context, &menu_main);
}
