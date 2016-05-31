#ifndef SPIIOEXP_H_INCLUDED
#define SPIIOEXP_H_INCLUDED

#include <avr.h>
#include <chipselect.h>


/*
 * <spiioexp> decl
 */
struct spiioexp {
        // physical chip select pin.
        struct pin cs;
};

/*
 * <spiioexp> public
 */
void            spiioexp_sys_init();
void            spiioexp_init_read_mode(struct spiioexp* self, const struct pin* cs);
void            spiioexp_init_write_mode(struct spiioexp* self, const struct pin* cs);
uint8_t         spiioexp_read_pins(const struct spiioexp* self);
void            spiioexp_write_pins(const struct spiioexp* self, const uint8_t data);

// chip select routine is left to the user.
void            spiioexp_init_read_mode2(struct spiioexp* self);
void            spiioexp_init_write_mode2(struct spiioexp* self);
uint8_t         spiioexp_read_pins2(const struct spiioexp* self);
void            spiioexp_write_pins2(const struct spiioexp* self, const uint8_t data);

/*
 * <spiioexp> test cases
 */
void spiioexp_test_blink_led();

/*
 * <spiioexp2> decl
 */
struct spiioexp2 {
        // logical chip select pin.
        uint8_t logic_cs;
};
/*
 * <spiioexp2> public
 */
// use logical chip select routine.
void            spiioexp2_init_read_mode(struct spiioexp2* self, const uint8_t logic_cs);
void            spiioexp2_init_write_mode(struct spiioexp2* self, const uint8_t logic_cs);
uint8_t         spiioexp2_read_pins(const struct spiioexp2* self);
void            spiioexp2_write_pins(const struct spiioexp2* self, const uint8_t data);


#endif // SPIIOEXP_H_INCLUDED
