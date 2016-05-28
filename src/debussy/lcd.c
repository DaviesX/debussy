#include <avr.h>
#include <lcd.h>

#define DDR     DDRB
#define PORT    PORTB
#define RS_PIN  0
#define RW_PIN  1
#define EN_PIN  2

static inline void set_data(unsigned char x)
{
        PORTD = x;
        DDRD = 0xff;
}

static inline unsigned char get_data()
{
        DDRD = 0x00;
        return PIND;
}

static inline void sleep_700ns()
{
        NOP();
        NOP();
        NOP();

        NOP();
        NOP();
        NOP();
}

static unsigned char input(unsigned char rs)
{
        unsigned char d;
        if (rs) SET_BIT(PORT, RS_PIN);
        else CLR_BIT(PORT, RS_PIN);
        SET_BIT(PORT, RW_PIN);
        get_data();
        SET_BIT(PORT, EN_PIN);
        sleep_700ns();
        d = get_data();
        CLR_BIT(PORT, EN_PIN);
        return d;
}

static void output(unsigned char d, unsigned char rs)
{
        if (rs) SET_BIT(PORT, RS_PIN);
        else CLR_BIT(PORT, RS_PIN);
        CLR_BIT(PORT, RW_PIN);
        set_data(d);
        SET_BIT(PORT, EN_PIN);
        sleep_700ns();
        CLR_BIT(PORT, EN_PIN);
}

static void write(unsigned char c, unsigned char rs)
{
        while (input(0) & 0x80);
        output(c, rs);
}

void lcd_init()
{
        SET_BIT(DDR, RS_PIN);
        SET_BIT(DDR, RW_PIN);
        SET_BIT(DDR, EN_PIN);
        avr_wait(16);
        output(0x30, 0);
        avr_wait(5);
        output(0x30, 0);
        avr_wait(1);
        write(0x3c, 0);
        write(0x0c, 0);
        write(0x06, 0);
        write(0x01, 0);
}

void lcd_clear()
{
        write(0x01, 0);
}

void lcd_pos(unsigned char r, unsigned char c)
{
        unsigned char n = r * 40 + c;
        write(0x02, 0);
        while (n--) {
                write(0x14, 0);
        }
}

void lcd_putc(char c)
{
        write(c, 1);
}

void lcd_puts_pgm(const char *s)
{
        char c;
        while ((c = pgm_read_byte(s++)) != 0) {
                write(c, 1);
        }
}

void lcd_puts(const char *s)
{
        char c;
        while ((c = *(s++)) != 0) {
                write(c, 1);
        }
}

void lcd_puti16(uint16_t value)
{
        static char buf[6] = {0};
        uint8_t i = 5;
        do {
                buf[--i] = (value%10) + '0';
                value /= 10;
        } while (value != 0);
        lcd_puts(&buf[i]);
}

void lcd_puti16n(uint16_t value, int n)
{
        static char buf[6] = {0};
        uint8_t i = 5;
        const uint8_t d = 5 - n;
        while (i > d) {
                buf[-- i] = (value%10) + '0';
                value /= 10;
        }
        lcd_puts(&buf[i]);
}
