#define LED_DDR DDRB
#define LED_PORT PORTB	
LED_DDR   = 0xFF;		// LED Port Richtung definieren (Ausgang)
LED_PORT  = 0xFF;		// LEDs ausschalten
LED_PORT &= ~((1 << PBX)); 	// loescht Bit an PortB - LED an
LED_PORT |=  ((1 << PBX)); 	// setzt  Bit an PortB - LED aus
