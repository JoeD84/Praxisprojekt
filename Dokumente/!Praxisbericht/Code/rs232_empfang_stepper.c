if ((UCSR1A & (1 << RXC1))){		// Stepper Polling
	LED_PORT &= ( 1 << LED3 );		// LED einschalten
	uart_rx(D_Stepper);			// Register auslesen
}
// UART Empfangsregister auslesen
void 	uart_rx				(int dir) {
	uart_get_string(str_rx, dir);	// String aus Empfangsregister auslesen
	if (dir == D_Stepper)		// Empfangsregister Stepper
		switch_Stepper(str_rx);	// Uebersetzungsfunktion fuer Stepper aufrufen
	else{				// Empfangsregister RapidForm
		// Wird spaeter erklaert
	}
}