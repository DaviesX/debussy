#include <avr.h>
#include <lcd.h>
#include <spiioexp.h>

#ifdef DEBUG
void test_gpio_flash_led()
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

void test_extsram_read_write()
{
}
#endif // DEBUG


int main()
{
        spiioexp_sys_init();
        extsram_sys_init();
#ifdef DEBUG
        // test_gpio_flash_led();
        test_extsram_read_write();
#endif // DEBUG

        while(1) {
                continue;
        }
        return 0;
}
