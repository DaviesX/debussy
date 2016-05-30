#ifndef LCD_H_INCLUDED
#define LCD_H_INCLUDED

#include <avr.h>

/*
 * <lcd> public
 */
void lcd_sys_init();
void lcd_clear();
void lcd_pos(unsigned char r, unsigned char c);
void lcd_putc(char c);
void lcd_puts_pgm(const char *s);
void lcd_puts(const char *s);
void lcd_puti16(uint16_t value);
void lcd_puti16n(uint16_t value, int n);

#endif  // LCD_H_INCLUDED
