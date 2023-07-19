// trap.c
#include <device.h>
#include <hal/trap.h>

struct hal_trap_manager_struct
{
    struct interruptor_struct interruptors_head;
    struct irq_struct irqs_head;
} hal_trap_manager;

void hal_trap_register_interruptor(struct interruptor_struct *dev)
{
    if (dev == NULL || dev->claim == NULL || dev->complete == NULL)
        panic("hal_trap_register_interruptor - broken dev");
    dev->next = hal_trap_manager.interruptors_head.next;
    hal_trap_manager.interruptors_head.next->next = dev;
}

void hal_trap_register_irq(struct irq_struct *irq)
{
    if (irq == NULL || irq->handler == NULL || irq->interrupt_source_id <= 0 || irq->interrupt_source_id > 1023)
        panic("hal_trap_register_irq - broken irq");
    irq->next = hal_trap_manager.irqs_head.next;
    hal_trap_manager.irqs_head.next->next = irq;
}

void hal_trap_handler(uint64 cause)
{
    // TODO;
}

void hal_trap_init()
{
    // TODO;
}
