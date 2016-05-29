#ifndef CHIPSELECT_H_INCLUDED
#define CHIPSELECT_H_INCLUDED

#include <avr.h>

// 8 bit chipselect D-latch device
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
 * <chipselect> public
 */
inline void cs_init()
{
}

inline void cs_enable_exclusive(uint8_t pin)
{
}

inline void cs_disable_exclusive(uint8_t pin)
{
}

inline void cs_enable_inclusive(uint8_t pin)
{
}

inline void cs_disable_inclusive(uint8_t pin)
{
}

inline void cs_clear()
{
}


#endif // CHIPSELECT_H_INCLUDED
