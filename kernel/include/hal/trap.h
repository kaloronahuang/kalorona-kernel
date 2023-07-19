// trap.h

#ifndef HAL_TRAP

#define HAL_TRAP

#include <types.h>

struct interruptor_struct
{
    uint64 (*claim)(void);
    void (*complete)(void);
    struct interruptor_struct *next;
};

struct irq_struct
{
    int interrupt_source_id;
    void (*handler)(void);
    struct irq_struct *next;
};

void hal_trap_register_interruptor(struct interruptor_struct *dev);

void hal_trap_register_irq(struct irq_struct *irq);

void hal_trap_handler(uint64 cause);

void hal_trap_init();

#endif
