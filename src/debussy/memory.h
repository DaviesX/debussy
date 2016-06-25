#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED


#include <types.h>

#ifndef ARCH_X86_64
typedef struct {
        union {
                struct {
                        uint8_t addr0_7;
                        uint8_t addr8_15;
                        uint8_t addr16_18;
                };
                uint32_t        addr;
                uint8_t         addr_parts[3];
        };
} mem_addr_t;
#else
typedef void*   mem_addr_t;

/*
 * <memory> public
 */
mem_addr_t      mem_alloc(size_t n_bytes);
mem_addr_t      mem_realloc(mem_addr_t addr, size_t n_bytes);
void            mem_free(mem_addr_t addr);
void            mem_read(mem_addr_t addr, size_t n_bytes, void* buf);
void            mem_write(mem_addr_t addr, size_t n_bytes, const void* buf);

#endif // ARCH_X86_64



#endif // MEMORY_H_INCLUDED
