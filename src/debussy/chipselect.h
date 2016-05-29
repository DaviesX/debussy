#ifndef CHIPSELECT_H_INCLUDED
#define CHIPSELECT_H_INCLUDED

// 8 bit chipselect GPIO device
enum {
        CS_GPIO0,
        CS_GPIO1,
        CS_GPIO2,
        CS_GPIO3,
        CS_GPIO4,
        CS_GPIO5,
        CS_GPIO6,
        CS_GPIO7,
};

// chipselect routines
#define EXTEEPROM0      CS_GPIO0
#define EXTEEPROM1      CS_GPIO1
#define EXTSRAM0        CS_GPIO2
#define EXTSRAM1        CS_GPIO3
#define R2RIOLOW        CS_GPIO4
#define R2RIOHIGH       CS_GPIO5
#define SPIMCUIO        CS_GPIO6
#define HIDUSB          CS_GPIO7


#endif // CHIPSELECT_H_INCLUDED
