case 3:			// 3 - Achse auswaehlen (Initialisierung)
	ms_spin(10);
	lcd_puts("Init");
	uart_put_string("0\r\n", D_RapidForm);
	break;