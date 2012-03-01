void 	uart_rx				(int dir) {
	uart_get_string(str_rx, dir);
	if (dir == D_Stepper)
		switch_Stepper(str_rx);
	else{
		if(Initialized == M_UNK){
			lcd_puts("Unbekannter Motor!\n");
			//lcd_puts(str_rx);
			Initialized = M_NOTI;
		}
		if(Initialized == M_NOTI){
			Initialized = switch_Motor(str_rx);
		}
		if(Initialized == M_ISEL)
			switch_Isel(str_rx);
		if(Initialized == M_CSG)
			switch_csg(str_rx);
		if(Initialized == M_ZETA)
			switch_Zeta(str_rx);
		if(Initialized == M_TERMINAL)
			switch_Terminal(str_rx);
	}
}