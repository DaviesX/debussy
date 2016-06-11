#ifndef ARCH_X86_64
#  include <spiioexp.h>
#  include <chipselect.h>


/*
 * <chipselect> public
 */
void cs_sys_init()
{
}

// fallback implementations.
void __cs_enable_exclusive(const uint8_t pin)
{
}

void __cs_disable_exclusive(const uint8_t pin)
{
}

void __cs_enable_inclusive(const uint8_t pin)
{
}

void __cs_disable_inclusive(const uint8_t pin)
{
}

void __cs_clear()
{
}

#endif // ARCH_X86_64
