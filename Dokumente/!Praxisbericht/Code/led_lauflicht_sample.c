
void 	led_lauflicht		(void) {
	uint8_t i = LED_PORT;
	i = (i & 0x07) | ((i << 1) & 0xF0);
	if (i < 0xF0)
		i |= 0x08;
	LED_PORT = i;
}
