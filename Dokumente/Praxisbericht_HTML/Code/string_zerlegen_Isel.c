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