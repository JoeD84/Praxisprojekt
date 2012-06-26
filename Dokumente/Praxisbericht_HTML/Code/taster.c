#include "Debounce.h"				// Taster entprellen
void 	debounce_init		(void) { 	// Taster entprellen
	KEY_DDR &= ~ALL_KEYS; 			// configure key port for input
	KEY_PORT |= ALL_KEYS; 			// and turn on pull up resistors
	TCCR0B = (1 << CS02) | (1 << CS00); 	// divide by 1024
	// preload for 10ms
	TCNT0 = (uint8_t) (int16_t) -(F_CPU / 1024 * 10 * 10e-3 + 0.5); 
	TIMSK0 |= 1 << TOIE0; 			// enable timer interrupt
	sei(); 					// global enable Interrupts
}
if (get_key_press(1 << KEY4))
	menu_select(&menu_context); 		// Aktuellen Menuepunkt auswaehlen