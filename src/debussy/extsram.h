#ifndef EXTSRAM_H_INCLUDED
#define EXTSRAM_H_INCLUDED

#ifndef ARCH_X86_64
#  include <avr.h>

/*
 * <extsram_addr> decl
 */
struct extsram_addr {
        union {
                struct {
                        uint8_t addr0_7;
                        uint8_t addr8_15;
                        uint8_t addr16_18;
                };
                uint32_t        addr;
                uint8_t         addr_parts[3];
        };
};

static inline void extsram_addr_init(struct extsram_addr* self,
                                     const uint8_t addr0_7,
                                     const uint8_t addr8_15,
                                     const uint8_t addr16_18)
{
        self->addr0_7 = addr0_7;
        self->addr8_15 = addr8_15;
        self->addr16_18 = addr16_18;
}

static inline void extsram_addr_init2(struct extsram_addr* self, uint32_t address)
{
        self->addr = address;
}


static inline void extsram_addr_set_mem_select_bits(struct extsram_addr* self,
                                                    const uint8_t bits)
{
        self->addr16_18 |= (bits << 3);
}

static inline uint8_t extsram_addr_get_mem_select_bits(struct extsram_addr* self,
                                                       const uint8_t bits)
{
        return self->addr16_18 >> 3;
}

/*
 * <extsram> decl
 */
struct extsram {
};

/*
 * <extsram> public
 */
void extsram_sys_init();

// chip-select is handled by the user.
void extsram_init_read_mode(struct extsram* self);
void extsram_init_write_mode(struct extsram* self);
void extstram_free(struct extsram* self);
void extsram_write(const struct extsram* self, const struct extsram_addr* addr, const void* data, uint16_t length);
void extsram_read(const struct extsram* self, const struct extsram_addr* addr, void* data, uint16_t length);

/*
 * <extsram> test cases
 */
void extsram_test_read_write();

#endif // ARCH_X86_64


#endif // EXTSRAM_H_INCLUDED
