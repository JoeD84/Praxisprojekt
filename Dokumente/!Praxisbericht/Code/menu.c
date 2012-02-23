#define MCU_CLK F_CPU
#include "tinymenu/spin_delay.h"
#define CONFIG_TINYMENU_USE_CLEAR
#include "tinymenu/tinymenu.h"
#include "tinymenu/tinymenu_hw.h"
#include "mymenu.h"

int main(void) {
  while (1) {
    if (get_key_press(1 << KEY0)) menu_enter(&menu_context, &menu_main);
    if (get_key_press(1 << KEY1)) menu_prev_entry(&menu_context);
    if (get_key_press(1 << KEY2)) menu_next_entry(&menu_context);
    if (get_key_press(1 << KEY4)) menu_select(&menu_context);
    if (get_key_press(1 << KEY4)) menu_exit(&menu_context); 
  }
}

void 	menu_puts			(void *arg, char *name) {
	uart_put_string(arg, D_Stepper);
	lcd_clrscr();
	lcd_puts("Send: ");
	lcd_puts(arg);
	lcd_puts("\n");
	ms_spin(100);
	//if ((UCSR1A & (1 << RXC1)))
	uart_rx(D_Stepper);
	ms_spin(1000);
}