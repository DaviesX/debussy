#ifndef CHIPSELECT_H_INCLUDED
#define CHIPSELECT_H_INCLUDED

#ifndef ARCH_X86_64
#  include <avr.h>

// logical chip select pins.
enum LatchPins {
        CS_LATCH0,
        CS_LATCH1,
        CS_LATCH2,
        CS_LATCH3,
        CS_LATCH4,
        CS_LATCH5,
        CS_LATCH6,
        CS_LATCH7,
};

/*
 * <chip_select> public
 */
void cs_sys_init();

// fast circuit implementation.
#define CS_ADDRESS_PORT         PORTD
#define CS_ADDRESS_DDR          DDRD

static inline void __cs2_clear()
{
        CS_ADDRESS_PORT |= 7;
}

static inline void cs2_sys_init()
{
        SET_BIT(CS_ADDRESS_DDR, 0);
        SET_BIT(CS_ADDRESS_DDR, 1);
        SET_BIT(CS_ADDRESS_DDR, 2);

        __cs2_clear();
}

static inline void __cs2_enable_exclusive(const uint8_t pin)
{
        CS_ADDRESS_PORT |= pin;
}

static inline void __cs2_disable_exclusive(const uint8_t pin)
{
        __cs2_clear();
}

static inline void __cs2_enable_inclusive(const uint8_t pin)
{
        // not supported.
}

static inline void __cs2_disable_inclusive(const uint8_t pin)
{
        // not supported.
}


// fallback io expander implementations.
void __cs_enable_exclusive(const uint8_t pin);
void __cs_disable_exclusive(const uint8_t pin);
void __cs_enable_inclusive(const uint8_t pin);
void __cs_disable_inclusive(const uint8_t pin);
void __cs_clear();

// use fast implementation.
#define cs_enable_exclusive     __cs2_enable_exclusive
#define cs_disable_exclusive    __cs2_disable_exclusive
#define cs_enable_inclusive
#define cs_disable_inclusive
#define cs_clear                __cs2_clear();

#endif // ARCH_X86_64

#endif // CHIPSELECT_H_INCLUDED
