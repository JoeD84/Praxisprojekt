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