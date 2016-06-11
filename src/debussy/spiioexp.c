#ifndef ARCH_X86_64
#  include <avr.h>
#  include <chipselect.h>
#  include <spiioexp.h>

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
        return;
}

static uint8_t __spiioexp_read_byte()
{
        while(!GET_BIT(SPSR, SPIF)) {
                continue;
        }
        return SPDR;
}

static void __spiioexp_write_byte(const uint8_t byte)
{
        SPDR = byte;
        while(!GET_BIT(SPSR, SPIF)) {
                continue;
        }
}

static void __spiioexp_configure(struct spiioexp* self, const bool is_read)
{
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
        // op code (write).
        __spiioexp_write_byte(0b01000000);
        // select IODIR register.
        __spiioexp_write_byte(0x0);
        // set it to iodir (0X00, output mode; 0XFF input mode).
        __spiioexp_write_byte(is_read ? 0XFF : 0X00);
}

void spiioexp_init_read_mode(struct spiioexp* self, const struct pin* cs)
{
        self->cs = *cs;

        SET_BIT(*self->cs.ddr, self->cs.pinno);
        SET_BIT(*self->cs.port, self->cs.pinno);
        CLR_BIT(*self->cs.port, self->cs.pinno);
        {
                spiioexp_init_read_mode2(self);
        }
        SET_BIT(*self->cs.port, self->cs.pinno);
}

void spiioexp_init_write_mode(struct spiioexp* self, const struct pin* cs)
{
        self->cs = *cs;

        SET_BIT(*self->cs.ddr, self->cs.pinno);
        SET_BIT(*self->cs.port, self->cs.pinno);
        CLR_BIT(*self->cs.port, self->cs.pinno);
        {
                spiioexp_init_write_mode2(self);
        }
        SET_BIT(*self->cs.port, self->cs.pinno);
}

uint8_t spiioexp_read_pins(const struct spiioexp* self)
{
        // fetch data from GPIO register
        uint8_t data;
        CLR_BIT(*self->cs.port, self->cs.pinno);
        {
                data = spiioexp_read_pins2(self);
        }
        SET_BIT(*self->cs.port, self->cs.pinno);
        return data;
}

void spiioexp_write_pins(const struct spiioexp* self, const uint8_t data)
{
        // put data to GPIO register
        CLR_BIT(*self->cs.port, self->cs.pinno);
        {
                spiioexp_write_pins2(self, data);
        }
        SET_BIT(*self->cs.port, self->cs.pinno);
}

// chip select routine is left to the user.
void spiioexp_init_read_mode2(struct spiioexp* self)
{
        __spiioexp_configure(self, true);
}

void spiioexp_init_write_mode2(struct spiioexp* self)
{
        __spiioexp_configure(self, false);
}

uint8_t spiioexp_read_pins2(const struct spiioexp* self)
{
        // op code (read).
        __spiioexp_write_byte(0b01000001);
        // select GPIO register address.
        __spiioexp_write_byte(0x9);
        // data transmission.
        return __spiioexp_read_byte();
}

void spiioexp_write_pins2(const struct spiioexp* self, const uint8_t data)
{
        // op code (write).
        __spiioexp_write_byte(0b01000000);
        // select GPIO register address.
        __spiioexp_write_byte(0x9);
        // data transmission.
        __spiioexp_write_byte(data);
}

/*
 * <spiioexp> test cases
 */
void spiioexp_test_blink_led()
{
        uint8_t led = 0, led2 = 0;
        struct pin cs, cs2;
        pin_init(&cs, PORTD, DDRD, 0);
        pin_init(&cs2, PORTD, DDRD, 1);
        struct spiioexp ioexp, ioexp2;
        spiioexp_init_write_mode(&ioexp, &cs);
        spiioexp_init_write_mode(&ioexp2, &cs2);

        while (1) {
                SET_BIT(led, 5);
                spiioexp_write_pins(&ioexp, led);
                CLR_BIT(led2, 5);
                spiioexp_write_pins(&ioexp2, led2);

                avr_wait(500);

                CLR_BIT(led, 5);
                spiioexp_write_pins(&ioexp, led);
                SET_BIT(led2, 5);
                spiioexp_write_pins(&ioexp2, led2);
                avr_wait(500);
        }
}

/*
 * <spiioexp2> public
 */
// use logical chip select routine.
void spiioexp2_init_read_mode(struct spiioexp2* self, const uint8_t logic_cs)
{
        self->logic_cs = logic_cs;
        cs_enable_exclusive(self->logic_cs);
        {
                spiioexp_init_write_mode2(nullptr);
        }
        cs_disable_exclusive(self->logic_cs);
}

void spiioexp2_init_write_mode(struct spiioexp2* self, const uint8_t logic_cs)
{
        self->logic_cs = logic_cs;
        cs_enable_exclusive(self->logic_cs);
        {
                spiioexp_init_write_mode2(nullptr);
        }
        cs_disable_exclusive(self->logic_cs);
}

uint8_t spiioexp2_read_pins(const struct spiioexp2* self)
{
        // fetch data from GPIO register
        uint8_t data;
        cs_enable_exclusive(self->logic_cs);
        {
                data = spiioexp_read_pins2(nullptr);
        }
        cs_disable_exclusive(self->logic_cs);
        return data;
}

void spiioexp2_write_pins(const struct spiioexp2* self, const uint8_t data)
{
        // put data to GPIO register
        cs_enable_exclusive(self->logic_cs);
        {
                spiioexp_write_pins2(nullptr, data);
        }
        cs_disable_exclusive(self->logic_cs);
}

#endif // ARCH_X86_64
