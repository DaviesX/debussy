#ifndef __lcd__
#define __lcd__

/*
 * <lcd> public
 */
void lcd_init();
void lcd_clear();
void lcd_pos(unsigned char r, unsigned char c);
void lcd_putc(char c);
void lcd_puts_pgm(const char *s);
void lcd_puts(const char *s);
void lcd_puti16(uint16_t value);
void lcd_puti16n(uint16_t value, int n);

#endif
