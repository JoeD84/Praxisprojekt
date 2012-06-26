#define M_UNK		-2
#define M_NOTI		-1
#define M_ISEL 		 0
#define M_CSG  		 1
#define M_ZETA 		 2
#define M_TERMINAL	 3
int 	Initialized = M_NOTI;
// Automatische Befehlssatzwahl
int 	switch_Motor		(char * str_rx) {	
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