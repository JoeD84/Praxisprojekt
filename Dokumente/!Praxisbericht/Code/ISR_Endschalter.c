PCMSK3   |= ( 1 << PCINT28 ); 	// Interrupts definierenPD4 als Interrupt zulassen
PCICR    |= ( 1 << PCIE3   ); 	// Pin Change Interrupt ControlR PCIE3 setzen fuer PCINT30
ISR(PCINT3_vect){		// + Endschalter Position erreicht
	lcd_clrscr();
	lcd_puts("Obere\nEndposition\nErreicht!");
	LED_PORT ^= (1 << LED3);
}
ISR(PCINT2_vect){		// - Endschalter Position erreicht
	lcd_clrscr();
	lcd_puts("Untere\nEndposition\nErreicht!");
	LED_PORT ^= (1 << LED3);
}