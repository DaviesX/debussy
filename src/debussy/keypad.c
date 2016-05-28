#include <avr.h>
#include <lcd.h>
#include <keypad.h>

#define KEY_PORT        PORTA
#define KEY_PIN         PINA
#define KEY_DDR         DDRA

static bool __key_is_pressed(uint8_t r, uint8_t c)
{
        KEY_DDR = 0;
        KEY_PORT = 0XFF;                // turn it to Z state.
        SET_BIT(KEY_DDR, r);
        CLR_BIT(KEY_PORT, r);           // logic low to the row to probe.
        avr_nop(1);
        if (!GET_BIT(KEY_PIN, c + 4)) {
                // if connected, the pin will read as a logic low.
                return true;
        } else {
                return false;
        }
}

uint8_t key_get_pressed(uint16_t milli)
{
        int c, r;
        for (r = 0; r < 4; r ++) {
                for (c = 0; c < 4; c ++) {
                        if (__key_is_pressed(r, c)) {
                                avr_nop(milli);
                                return c + 4*r;
                        }
                }
        }
        return KEY_NULL;
}

uint8_t key_to_digit(uint8_t key_code)
{
        switch (key_code) {
        case KEY_NUM1: return 1;
        case KEY_NUM2: return 2;
        case KEY_NUM3: return 3;
        case KEY_NUM4: return 4;
        case KEY_NUM5: return 5;
        case KEY_NUM6: return 6;
        case KEY_NUM7: return 7;
        case KEY_NUM8: return 8;
        case KEY_NUM9: return 9;
        case KEY_NUM0: return 0;
        default:       return -1;
        }
}

char key_to_char(uint8_t key_code)
{
        switch (key_code) {
        case KEY_D:
                return 'D';
        case KEY_C:
                return 'C';
        case KEY_B:
                return 'B';
        case KEY_A:
                return 'A';
        case KEY_POUND:
                return '#';
        case KEY_NUM9:
                return '9';
        case KEY_NUM6:
                return '6';
        case KEY_NUM3:
                return '3';
        case KEY_NUM0:
                return '0';
        case KEY_NUM8:
                return '8';
        case KEY_NUM5:
                return '5';
        case KEY_NUM2:
                return '2';
        case KEY_STAR:
                return '*';
        case KEY_NUM7:
                return '7';
        case KEY_NUM4:
                return '4';
        case KEY_NUM1:
                return '1';
        default:
        case KEY_NULL:
                return '\0';
        }
}
