#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#define nullptr         (void*) 0X0

#ifndef ARCH_X86_64
// exclude incorrect definitions
#  define _INTTYPES_H
#  define _STDINT_H

// use the correct definitions.
#  define uint32_t      unsigned long
#  define uint16_t      unsigned short
#  define uint8_t       unsigned char
#  define int32_t       signed long
#  define int16_t       signed short
#  define int8_t        signed char
#  define __int8_t_defined
#  define bool          unsigned char
#  define int_farptr_t  int32_t
#  define uint_farptr_t uint32_t
#  define true          1
#  define false         0
#else
#  include <stdint.h>
#  include <stdbool.h>
#endif // ARCH_X86_64


#endif // TYPES_H_INCLUDED
