if ((UCSR1A & (1 << RXC1))){
	LED_PORT &= ( 1 << LED3 );
	uart_rx(D_Stepper);
}

void 	uart_rx				(int dir) {
	uart_get_string(str_rx, dir);
	if (dir == D_Stepper)
		switch_Stepper(str_rx);
	else{
		...
	}
}