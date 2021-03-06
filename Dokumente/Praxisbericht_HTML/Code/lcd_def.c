/**< Use 0 for HD44780 controller,  1 for KS0073 controller */
#define LCD_CONTROLLER_KS0073 0  
#define LCD_LINES           4     	/**< number of visible lines of the display */
#define LCD_DISP_LENGTH    19     	/**< visibles characters per line of the display */
#define LCD_LINE_LENGTH  0x40     	/**< internal line length of the display   */
#define LCD_START_LINE1  0x00     	/**< DDRAM address of first char of line 1 */
#define LCD_START_LINE2  0x40     	/**< DDRAM address of first char of line 2 */
#define LCD_START_LINE3  0x14     	/**< DDRAM address of first char of line 3 */
#define LCD_START_LINE4  0x54     	/**< DDRAM address of first char of line 4 */
#define LCD_WRAP_LINES      1     	/**< 0: no wrap, 1: wrap at end of visibile line */
// Funktionen zum Ansteuern des Displays:
extern void lcd_init(uint8_t dispAttr);
extern void lcd_clrscr(void);
extern void lcd_home(void);
extern void lcd_gotoxy(uint8_t x, uint8_t y);
extern void lcd_putc(char c);
extern void lcd_puts(const char *s);
extern void lcd_puts_p(const char *progmem_s);
extern void lcd_command(uint8_t cmd);
extern void lcd_data(uint8_t data);
#define lcd_puts_P(__s) lcd_puts_p(PSTR(__s))