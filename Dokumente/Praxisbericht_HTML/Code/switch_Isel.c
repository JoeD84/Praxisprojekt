// Uebersetzung Isel
void 	switch_Isel			(char * str_rx) {
	const char* pOptions[] = {
			"XXXXXXX", 	// 0 - Reserve
			"!CLS",    	// 1 - LC-Display loeschen
			"Test", 	// 2 - Test
			"@01",  	// 3 - Achse auswaehlen
			"@0R", 		// 4 - Status abfrage
			"@0M", 		// 5 - Gehe zu Position
			0 };
	switch (FindStringInArray(str_rx, pOptions, 3))