

void 	switch_Stepper		(char * str_rx) {	//Auswerte-Funktion fuer die Schrittmotorkarte
	const char* pOptions[] = {	// Array mit moeglichen Antworten gegen das geprueft wird
			"#", 	// 0 - Stepper Karte Befehl erkannt
			"E", 	// 1 - Error
			"!CLS", // 2 - Clear Screen
			"Test", // 3 - Interner Test zum Debuggen
			0 };
	switch (FindStringInArray(str_rx, pOptions, 1)) {
	case 0:	// 0 - Stepper Karte Befehl erkannt
		lcd_puts("Erfolgreich\n");	// "Erfolgreich" auf dem Display anzeigen
		//uart_put_string("0\n\r", D_RapidForm);
		break;
	case 1:	// 1 - Error
		lcd_puts("Error\n");		// "Error" auf dem Display anzeigen
		uart_put_string("1\r\n", D_RapidForm); // "1" an RapidForm senden um einen Fehler zu melden
		break;
	case 2:	// 2 - Clear Screen
		lcd_clrscr();	// Debug: Loescht das Display
		break;
	case 3:	// 3 - Test
		lcd_puts("Test bestanden\n");	// Debug: gibt "Test bestanden" auf dem Display aus.
		//uart_put_string("Test bestanden\n\r", D_RapidForm);
		//uart_put_string("Test bestanden\n\r", D_Stepper);
		break;
	default: // Standardmaessig warte kurz.
		ms_spin(10);
		//lcd_puts("Stepper: "); // Debugging
		//lcd_puts(str_rx);
		//lcd_puts("!\n");
	}
}