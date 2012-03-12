#define MCU_CLK F_CPU
#include "tinymenu/spin_delay.h"
#define CONFIG_TINYMENU_USE_CLEAR
#include "tinymenu/tinymenu.h"
#include "tinymenu/tinymenu_hw.h"
#include "mymenu.h"
// Gekuerzte Main-Funktion
int main(void) {
	while (1) { 	// In Endlosschleife wechseln
		wdt_reset();	// Watchdog zuruecksetzen
		if (get_key_press(1 << KEY1))	// 1 - Zurueck
			menu_exit(&menu_context);
		if( get_key_press(1 << KEY2))	// 2 - Hoch
			menu_prev_entry(&menu_context);
		if (get_key_press(1 << KEY3))	// 3 - Runter
			menu_next_entry(&menu_context);
		if (get_key_press(1 << KEY4))	// 4 - Ok
			menu_select(&menu_context);
	}
}
// Funktion zum senden der Menuepunkte ueber die serielle Schnittstelle
void 	menu_puts		(void *arg, char *name) { // Menu/Sende Funktion
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