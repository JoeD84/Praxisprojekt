// Uebersetzung Schrittmotorkarte
void 	switch_Stepper		(char * str_rx) {	
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