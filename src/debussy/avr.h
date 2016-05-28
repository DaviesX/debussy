#ifndef __avr__
#define __avr__

#define F_CPU   16000000UL

#include <inttypes.h>

typedef int32_t int_farptr_t;
typedef uint32_t uint_farptr_t;
typedef unsigned char bool;
#define true    1
#define false   0
#define nullptr (void*) 0X0

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
void avr_nop(uint16_t ms);
void avr_set_timer1_ctc(uint32_t milli);

#endif
