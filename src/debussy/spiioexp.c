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

static uint8_t __spiioexp_read_byte()
{
        while(!GET_BIT(SPSR, SPIF)) {
                continue;
        }
        return SPDR;
}

static void __spiioexp_write_byte(uint8_t byte)
{
        SPDR = byte;
        while(!GET_BIT(SPSR, SPIF)) {
                continue;
        }
}

static void __spiioexp_configure(struct spiioexp* self, const struct pin* cs, const bool is_read)
{
        self->cs = *cs;

        SET_BIT(*self->cs.ddr, self->cs.pinno);
        SET_BIT(*self->cs.port, self->cs.pinno);

        // configure to be master output.
        if (is_read)
                SET_BIT(SPI_DDR, MISO);
        else
                SET_BIT(SPI_DDR, MOSI);
        SET_BIT(SPI_DDR, SCK);
        SET_BIT(SPCR, SPE);
        SET_BIT(SPCR, MSTR);
        // double the spi clock rate.
        SET_BIT(SPSR, SPI2X);

        // configure IO direction
        CLR_BIT(*self->cs.port, self->cs.pinno);
        {
                // op code (write).
                __spiioexp_write_byte(0b01000000);
                // select IODIR register.
                __spiioexp_write_byte(0x0);
                // set it to iodir (0X00, output mode; 0XFF input mode).
                __spiioexp_write_byte(is_read ? 0XFF : 0X00);
        }
        SET_BIT(*self->cs.port, self->cs.pinno);
}

void spiioexp_init_read_mode(struct spiioexp* self, struct pin* cs)
{
        __spiioexp_configure(self, cs, true);
}

uint8_t spiioexp_read_pins(struct spiioexp* self)
{
        // fetch data from GPIO register
        uint8_t data;
        CLR_BIT(*self->cs.port, self->cs.pinno);
        {
                // op code (read).
                __spiioexp_write_byte(0b01000001);
                // select GPIO register address.
                __spiioexp_write_byte(0x9);
                // data transmission.
                data = __spiioexp_read_byte();
        }
        SET_BIT(*self->cs.port, self->cs.pinno);
        return data;
}

void spiioexp_init_write_mode(struct spiioexp* self, struct pin* cs)
{
        __spiioexp_configure(self, cs, false);
}

void spiioexp_write_pins(struct spiioexp* self, uint8_t data)
{
        // put data to GPIO register
        CLR_BIT(*self->cs.port, self->cs.pinno);
        {
                // op code (write).
                __spiioexp_write_byte(0b01000000);
                // select GPIO register address.
                __spiioexp_write_byte(0x9);
                // data transmission.
                __spiioexp_write_byte(data);
        }
        SET_BIT(*self->cs.port, self->cs.pinno);
}
