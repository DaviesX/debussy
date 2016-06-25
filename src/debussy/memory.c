#include <chipselect.h>
#include <extsram.h>
#include <memory.h>


#ifndef ARCH_X86_64
#else
#  include <stdlib.h>
#  include <string.h>

/*
 * <memory> public
 */
mem_addr_t mem_alloc(size_t n_bytes)
{
        return malloc(n_bytes);
}

mem_addr_t mem_realloc(mem_addr_t addr, size_t n_bytes)
{
        return realloc(addr, n_bytes);
}

void mem_free(mem_addr_t addr)
{
        free(addr);
}

void mem_read(mem_addr_t addr, size_t n_bytes, void* buf)
{
        memcpy(buf, addr, n_bytes);
}

void mem_write(mem_addr_t addr, size_t n_bytes, const void* buf)
{
        memcpy(addr, buf, n_bytes);
}


#endif // ARCH_X86_64
