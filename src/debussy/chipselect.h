#ifndef CHIPSELECT_H_INCLUDED
#define CHIPSELECT_H_INCLUDED

#include <avr.h>

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

// fast implementation.
static inline void __cs2_enable_exclusive(const uint8_t pin)
{
}

static inline void __cs2_disable_exclusive(const uint8_t pin)
{
}

static inline void __cs2_enable_inclusive(const uint8_t pin)
{
}

static inline void __cs2_disable_inclusive(const uint8_t pin)
{
}

static inline void __cs2_clear()
{
}

// fallback implementations.
void __cs_enable_exclusive(const uint8_t pin);
void __cs_disable_exclusive(const uint8_t pin);
void __cs_enable_inclusive(const uint8_t pin);
void __cs_disable_inclusive(const uint8_t pin);
void __cs_clear();

// use fallback technology.
#define cs_enable_exclusive     __cs_enable_exclusive
#define cs_disable_exclusive     __cs_enable_exclusive
#define cs_enable_inclusive     __cs_enable_exclusive
#define cs_disable_inclusive     __cs_enable_exclusive


#endif // CHIPSELECT_H_INCLUDED
