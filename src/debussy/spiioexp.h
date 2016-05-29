#ifndef SPIIOEXP_H_INCLUDED
#define SPIIOEXP_H_INCLUDED

#include <avr.h>

/*
 * <spi_io_expander> decl
 */
struct spiioexp {
        struct pin cs;
};

/*
 * <spi_io_expander> public
 */
void            spiioexp_sys_init();
void            spiioexp_init_write_mode(struct spiioexp* self, struct pin* cs);
void            spiioexp_init_read_mode(struct spiioexp* self, struct pin* cs);
uint8_t         spiioexp_read_pins(struct spiioexp* self);
void            spiioexp_write_pins(struct spiioexp* self, uint8_t data);


#endif // SPIIOEXP_H_INCLUDED
