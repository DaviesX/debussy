#include <stdlib.h>
#include <avr.h>
#include <chipselect.h>
#include <extsram.h>

#define CE              // this is handled by the user.
#define OE              PB6
#define WE              PB3
#define CTRL_PORT       PORTB
#define CTRL_DDR        DDRB
#define ADDR0_7_PORT    PORTC
#define ADDR0_7_DDR     DDRC
#define ADDR8_15_PORT   PORTD
#define ADDR8_15_DDR    DDRD
#define ADDR16_18_PORT  PORTB
#define ADDR16_18_DDR   DDRB
#define DATA_PORT       PORTA
#define DATA_DDR        DDRA

/*
 * <extsram> public
 */
void extsram_sys_init()
{
}

void extsram_init_read_mode(struct extsram* self)
{
        CTRL_DDR = 0XFF;
        CLR_BIT(CTRL_PORT, OE);
        SET_BIT(CTRL_PORT, WE);

        ADDR0_7_DDR = 0XFF;
        ADDR8_15_DDR = 0XFF;
        ADDR16_18_DDR = 0XFF;

        DATA_DDR = 0X00;
}

void extsram_init_write_mode(struct extsram* self)
{
        CTRL_DDR = 0XFF;
        CLR_BIT(CTRL_PORT, WE);

        ADDR0_7_DDR = 0XFF;
        ADDR8_15_DDR = 0XFF;
        ADDR16_18_DDR = 0XFF;

        DATA_DDR = 0XFF;
}

void extstram_free(struct extsram* self)
{
        // High Z I/O
        SET_BIT(CTRL_PORT, WE);
        SET_BIT(CTRL_PORT, OE);

        ADDR0_7_DDR = 0X00;
        ADDR8_15_DDR = 0X00;
        ADDR16_18_DDR = 0X00;

        DATA_DDR = 0X00;
}

//#define __extsram_serial_read_write_fast(__self, __addr, __data, __length, __iodir)     \
//{                                                                                       \
//        ADDR0_7_PORT = (__addr)->addr0_7;                                               \
//        ADDR8_15_PORT = (__addr)->addr8_15;                                             \
//        ADDR16_18_PORT = (__addr)->addr16_18 & 0b00000111;                              \
//                                                                                        \
//                                                                                        \
//        uint16_t i = 0;                                                                 \
//        while ((__length) > 0) {                                                        \
//                /* save clock manipulation cycles. */                                   \
//                uint16_t end = ADDR0_7_PORT + (__length);                               \
//                const uint8_t end_addr0_7 = end > 255 ? 255 : end;                      \
//                (__length) -= end_addr0_7 - ADDR0_7_PORT + 1;                           \
//                                                                                        \
//                for ( ; ADDR0_7_PORT < end_addr0_7; ) {                                 \
//                        if (__iodir == 0) DATA_PORT = (__data)[i];                      \
//                        else              (__data)[i] = DATA_PORT;                      \
//                        ADDR0_7_PORT ++;                                                \
//                        i ++;                                                           \
//                }                                                                       \
//                                                                                        \
//                /* last increment. */                                                   \
//                if (__iodir == 0) DATA_PORT = (__data)[i];                              \
//                else              (__data)[i] = DATA_PORT;                              \
//                                                                                        \
//                uint32_t address = ((ADDR0_7_PORT | ((uint16_t) ADDR8_15_PORT << 8)) |  \
//                                    ((uint32_t) ADDR16_18_DDR << 16)) + 1;              \
//                ADDR16_18_DDR = address >> 16;                                          \
//                ADDR8_15_PORT = (uint8_t) (address >> 8);                               \
//                ADDR0_7_PORT = (uint8_t) address;                                       \
//                                                                                        \
//                i ++;                                                                   \
//        }                                                                               \
//}
//
#define __extsram_serial_read_write(__self, __addr, __data, __length, __iodir)          \
{                                                                                       \
        ADDR0_7_PORT = (__addr)->addr0_7;                                               \
        ADDR8_15_PORT = (__addr)->addr8_15;                                             \
        ADDR16_18_PORT = (__addr)->addr16_18 & 0b00000111;                              \
                                                                                        \
        uint16_t i = 0;                                                                 \
        while (i < (__length)) {                                                        \
                if (__iodir == 0) DATA_PORT = (__data)[i];                              \
                else              (__data)[i] = DATA_PORT;                              \
                                                                                        \
                uint32_t address = ((ADDR0_7_PORT | ((uint16_t) ADDR8_15_PORT << 8)) |  \
                                    ((uint32_t) ADDR16_18_DDR << 16)) + 1;              \
                ADDR16_18_DDR = address >> 16;                                          \
                ADDR8_15_PORT = (uint8_t) (address >> 8);                               \
                ADDR0_7_PORT = (uint8_t) address;                                       \
                                                                                        \
                i ++;                                                                   \
        }                                                                               \
}

void extsram_write(const struct extsram* self,
                   const struct extsram_addr* addr,
                   const void* data,
                   uint16_t length)
{
        __extsram_serial_read_write(self, addr, (uint8_t*) data, length, 0);
}

void extsram_read(const struct extsram* self,
                  const struct extsram_addr* addr,
                  void* data,
                  uint16_t length)
{
        __extsram_serial_read_write(self, addr, (uint8_t*) data, length, 1);
}

/*
 * <extsram> test cases
 */
void extsram_test_read_write()
{
        volatile uint8_t bytes[512], fetched[512];
        const uint8_t magic_byte = 0XE5;
        uint16_t i;
        for (i = 0; i < sizeof(bytes); i ++) {
                bytes[i] = rand()%255;
                fetched[i] = 0;
        }
        struct extsram_addr addr;
        extsram_addr_init2(&addr, 0X0);

        struct extsram sram;
        extsram_init_write_mode(&sram);
        extsram_write(&sram, &addr, bytes, sizeof(bytes));
        extsram_init_read_mode(&sram);
        extsram_read(&sram, &addr, fetched, sizeof(bytes));

        for (i = 0; i < sizeof(bytes)m; i ++) {
                if (fetched[i] != bytes[i])
                        goto failed;
        }

        goto pass;
pass:
        CLR_BIT(PORTB, 7);
        while (1) {
                SET_BIT(DDRB, 5);
                SET_BIT(PORTB, 5);
        }
failed:
        CLR_BIT(PORTB, 5);
        while (1) {
                SET_BIT(DDRB, 7);
                SET_BIT(PORTB, 7);
        }
}
