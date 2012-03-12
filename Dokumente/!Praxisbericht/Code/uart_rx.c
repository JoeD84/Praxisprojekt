// UART Empfangsregister auslesen
void 	uart_rx				(int dir) {
	uart_get_string(str_rx, dir);	// String aus Empfangsregister auslesen
	if (dir == D_Stepper)		// Empfangsregister Stepper
		switch_Stepper(str_rx);		// Uebersung Stepper
	else{				// Empfangsregsiter RapidForm
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
		if(Initialized == M_TERMINAL)	// Uebersetzung Terminal
			switch_Terminal(str_rx);
	}
}