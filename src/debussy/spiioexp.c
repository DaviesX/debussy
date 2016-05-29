#include <avr.h>
#include <chipselect.h>
#include <spiioexp.h>

#define SPI_PORT        PORTB
#define SPI_PIN         PINB
#define SPI_DDR         DDRB
#define SS              PB4
#define MOSI            PB5
#define MISO            PB6
#define SCK             PB7

/*
 * <spi> public
 */
void spiioexp_sys_init()
{
}

uint8_t spiioexp_read_pins(struct spiioexp* self)
{
        return 0;
}

static void __spiioexp_write_byte(uint8_t byte)
{
        SPDR = byte;
        while(!GET_BIT(SPSR, SPIF)) {
                continue;
        }
}

void spiioexp_init_write_mode(struct spiioexp* self, struct pin* cs)
{
        self->cs = *cs;

        SET_BIT(*self->cs.ddr, self->cs.pinno);
        SET_BIT(*self->cs.port, self->cs.pinno);

        // configure to be master output.
        SET_BIT(SPI_DDR, MOSI);
        SET_BIT(SPI_DDR, SCK);
        SET_BIT(SPCR, SPE);
        SET_BIT(SPCR, MSTR);
        // SET_BIT(SPSR, SPI2X);
        SET_BIT(SPCR, SPR0);

        CLR_BIT(SPI_PORT, SS);
        // configure IO direction
        CLR_BIT(*self->cs.port, self->cs.pinno);
        {
                // op code.
                __spiioexp_write_byte(0b01000000);
                // select IODIR register.
                __spiioexp_write_byte(0x0);
                // set it to output.
                __spiioexp_write_byte(0x0);
        }
        SET_BIT(*self->cs.port, self->cs.pinno);
}

void spiioexp_write_pins(struct spiioexp* self, uint8_t data)
{
        // put data to GPIO register
        CLR_BIT(*self->cs.port, self->cs.pinno);
        {
                // op code.
                __spiioexp_write_byte(0b01000000);
                // slect GPIO register address.
                __spiioexp_write_byte(0x9);
                // data transmission.
                __spiioexp_write_byte(data);
        }
        SET_BIT(*self->cs.port, self->cs.pinno);
}
