case 4:			// 4 - Status abfrage
	lcd_puts("Statusabfrage:     \n");
	uart_put_string("A\n", D_Stepper);	// Statusabfrage an Stepper senden
	ms_spin(50);					// Verarbeitungszeit gewaehren
	if ((UCSR1A & (1 << RXC1)))			// Wenn ein Zeichen empfangen wurde
		uart_rx(D_Stepper);			// Zeichen auslesen
	if (!strcmp(str_rx,"0#"))			// Empfangenes Zeichen ueberpruefen
		uart_put_string("0\r\n", D_RapidForm); 	// Antwort Ok an RF melden
	else {
		lcd_puts("Fehlgeschlagen     \n");	   // Fehler auf Display anzeigen
		uart_put_string("1\r\n", D_RapidForm); // Fehler an RapidForm melden
	}
	break;