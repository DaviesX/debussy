#ifndef KEYPAD_H_INCLUDED
#define KEYPAD_H_INCLUDED

enum KEY {
        KEY_D,
        KEY_C,
        KEY_B,
        KEY_A,
        KEY_POUND,
        KEY_NUM9,
        KEY_NUM6,
        KEY_NUM3,
        KEY_NUM0,
        KEY_NUM8,
        KEY_NUM5,
        KEY_NUM2,
        KEY_STAR,
        KEY_NUM7,
        KEY_NUM4,
        KEY_NUM1,
        KEY_NULL
};

/*
 * <keypad> public
 */
uint8_t key_get_pressed(uint16_t milli);
uint8_t key_to_digit(uint8_t key_code);


#endif // KEYPAD_H_INCLUDED
