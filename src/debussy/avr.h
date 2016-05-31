#ifndef AVR_H_INCLUDED
#define AVR_H_INCLUDED

#define F_CPU   16000000UL

// exclude incorrect definitions
#define _INTTYPES_H
#define _STDINT_H

// use the correct definitions.
typedef unsigned long   uint32_t;
typedef unsigned int    uint16_t;
typedef unsigned char   uint8_t;
typedef signed long     int32_t;
typedef signed int      int16_t;
typedef signed char     int8_t;
typedef unsigned char   bool;
#define true            1
#define false           0
typedef int32_t         int_farptr_t;
typedef uint32_t        uint_farptr_t;
#define nullptr         (void*) 0X0
struct pin {
        uint8_t* port;
        uint8_t* ddr;
        uint8_t  pinno;
};
#define pin_init(__self, __port, __ddr, __pinno)        \
{                                                       \
        (__self)->port = (uint8_t *) &(__port);         \
        (__self)->ddr = (uint8_t *) &(__ddr);           \
        (__self)->pinno = (__pinno);                    \
}

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <avr/io.h>

#define XTAL_FRQ F_CPU

#define SET_BIT(p,i) ((p) |=  (1 << (i)))
#define CLR_BIT(p,i) ((p) &= ~(1 << (i)))
#define GET_BIT(p,i) ((p) &   (1 << (i)))

#define WDR() asm volatile("wdr"::)
#define NOP() asm volatile("nop"::)
#define RST() for(;;);

void avr_init();
void avr_wait(uint16_t msec);
void avr_wait2(uint16_t msec, uint16_t (*callback) (), void* data);
void avr_wait_micro(uint16_t microsec);
void avr_nop(uint16_t ms);
void avr_set_timer1_ctc(uint32_t milli);


#endif  // AVR_H_INCLUDED
