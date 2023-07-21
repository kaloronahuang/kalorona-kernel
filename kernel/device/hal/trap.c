// trap.c
#include <device.h>
#include <hal/trap.h>

struct hal_trap_manager_struct
{
    struct interruptor_struct *interruptor;
    struct irq_struct irqs_head;
} hal_trap_manager;

int hal_trap_register_interruptor(struct interruptor_struct *dev)
{
    if (dev == NULL || dev->claim == NULL || dev->complete == NULL)
        return -1;
    hal_trap_manager.interruptor = dev;
    return 0;
}

int hal_trap_register_irq(struct irq_struct *irq)
{
    if (irq == NULL || irq->handler == NULL || irq->interrupt_src_id <= 0 || irq->interrupt_src_id > 1023)
        return -1;
    irq->next = hal_trap_manager.irqs_head.next;
    hal_trap_manager.irqs_head.next = irq;
    return 0;
}

void hal_trap_handler(uint64 cause)
{
    if (hal_trap_manager.interruptor == NULL)
        return;
    struct device_struct *dev = hal_trap_manager.interruptor->dev;
    uint64 irq = hal_trap_manager.interruptor->claim(dev);
    for (struct irq_struct *i = hal_trap_manager.irqs_head.next; i != NULL; i = i->next)
        if (i->interrupt_src_id == irq)
        {
            i->handler(i->dev);
            break;
        }
    hal_trap_manager.interruptor->complete(dev, irq);
}

void hal_trap_init(void)
{
    // device drivers already activated and configured the HAL;
    if (hal_trap_manager.interruptor != NULL)
        hal_trap_manager.interruptor->init(hal_trap_manager.interruptor->dev, &(hal_trap_manager.irqs_head));
}
