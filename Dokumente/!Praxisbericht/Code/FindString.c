int 	FindStringInArray	(const char* pInput, const char* pOptions[], int cmp_length) {
	int n = -1;
	while (pOptions[++n]) {	//Array durchlaufen bis 0 terminiert
		//Wenn pInput == pOptions dann gib Array Position zurueck
		if (!strncmp(pInput, pOptions[n], cmp_length))	return n; 
	}
	return 99; // Wenn keine uebereinstimmung, gib 99 zurueck
} 