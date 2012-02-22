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
