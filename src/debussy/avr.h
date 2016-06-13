#ifndef AVR_H_INCLUDED
#define AVR_H_INCLUDED

#include <types.h>

#define DEBUSSY_VERSION_STRING   "debussy - 0.0.1"


#ifndef ARCH_X86_64
#  define F_CPU 16000000UL
#  include <avr/interrupt.h>
#  include <avr/sleep.h>
#  include <avr/pgmspace.h>
#  include <util/delay.h>
#  include <avr/io.h>
#  include <avr/wdt.h>

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

#endif // ARCH_X86_64


#endif  // AVR_H_INCLUDED
