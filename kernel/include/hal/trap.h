// trap.h

#ifndef HAL_TRAP

#define HAL_TRAP

#include <types.h>

struct irq_struct
{
    int interrupt_src_id;
    struct device_struct *dev;
    int (*handler)(struct device_struct *);
    struct irq_struct *next;
};

struct interruptor_struct
{
    struct device_struct *dev;
    void (*init)(struct device_struct *, struct irq_struct *);
    void (*enable_src)(struct device_struct *, uint);
    void (*disable_src)(struct device_struct *, uint);
    int (*check_src_status)(struct device_struct *, uint);
    uint64 (*claim)(struct device_struct *);
    void (*complete)(struct device_struct *, uint);
};

int hal_trap_register_interruptor(struct interruptor_struct *dev);

int hal_trap_register_irq(struct irq_struct *irq);

void hal_trap_handler(uint64 cause);

void hal_trap_init(void);

#endif
