// ns16550a.c
// reference from xv6;
#include <device.h>
#include <kmem.h>
#include <hal/trap.h>
#include <proc.h>
#include <hal/uart_io.h>

// Macros for UART driver;

#define UART_REG(base, reg_id) ((volatile unsigned char *)(PMA_PA2VA(((ulong)base) + ((ulong)reg_id))))

// receive holding register (for input bytes)
#define UART_RHR 0
// transmit holding register (for output bytes)
#define UART_THR 0
// interrupt enable register
#define UART_IER 1
#define UART_IER_RX_ENABLE (1 << 0)
#define UART_IER_TX_ENABLE (1 << 1)
// FIFO control register
#define UART_FCR 2
#define UART_FCR_FIFO_ENABLE (1 << 0)
// clear the content of the two FIFOs
#define UART_FCR_FIFO_CLEAR (3 << 1)
// interrupt status register
#define UART_ISR 2
// line control register
#define UART_LCR 3
#define UART_LCR_EIGHT_BITS (3 << 0)
// special mode to set baud rate
#define UART_LCR_BAUD_LATCH (1 << 7)
// line status register
#define UART_LSR 5
// input is waiting to be read from RHR
#define UART_LSR_RX_READY (1 << 0)
// THR can accept another character to send
#define UART_LSR_TX_IDLE (1 << 5)

#define TX_BUFFER_SIZE 64
#define RX_BUFFER_SIZE 64

struct ns16550a_internal_struct
{
    dev_t devId;
    int interrupt_src_id;
    int clock_freq;
    uint64 reg_base;
    uint64 reg_size;

    struct spinlock tx_lock;
    struct spinlock rx_lock;
    char tx_buffer[TX_BUFFER_SIZE];
    char rx_buffer[RX_BUFFER_SIZE];
    ulong tx_buffer_head, tx_buffer_tail;
    ulong rx_buffer_head, rx_buffer_tail;
};

bool ns16550a_driver_recognize_device(struct fdt_header *fdt, int node_offset);
struct device_struct *ns16550a_driver_init(int devId, struct fdt_header *fdt, int node_offset);

struct driver_struct ns16550a_driver = {
    .developer = "Kalorona Huang",
    .name = "NS16550A Compatible UART Driver",
    .version = 0,
    .init = ns16550a_driver_init,
    .recognize_device = ns16550a_driver_recognize_device};

void ns16550a_driver_flush(struct device_struct *dev)
{
    struct ns16550a_internal_struct *u = (struct ns16550a_internal_struct *)dev->driver_internal;
    // something to flush && device is ready;
    while (u->tx_buffer_head != u->tx_buffer_tail && ((*UART_REG(u->reg_base, UART_LSR)) & UART_LSR_TX_IDLE) != 0)
    {
        int c = u->tx_buffer[u->tx_buffer_head % TX_BUFFER_SIZE];
        u->tx_buffer_head += 1;
        (*UART_REG(u->reg_base, UART_THR)) = c;
        proc_wakeup(&u->tx_buffer_head);
    }
}

void ns16550a_driver_putc_async(struct device_struct *dev, char c)
{
    // putting things in the line;
    struct ns16550a_internal_struct *u = (struct ns16550a_internal_struct *)dev->driver_internal;
    spinlock_acquire(&(u->tx_lock));
    while (u->tx_buffer_head + TX_BUFFER_SIZE == u->tx_buffer_tail)
        proc_sleep(&u->tx_buffer_head, &(u->tx_lock));
    u->tx_buffer[u->tx_buffer_tail % TX_BUFFER_SIZE] = c;
    u->tx_buffer_tail++;
    ns16550a_driver_flush(dev);
    spinlock_release(&(u->tx_lock));
}

void ns16550a_driver_putc_sync(struct device_struct *dev, char c)
{
    struct ns16550a_internal_struct *u = (struct ns16550a_internal_struct *)dev->driver_internal;
    pushoff_hart();
    while (((*UART_REG(u->reg_base, UART_LSR)) & UART_LSR_TX_IDLE) == 0)
        ;
    (*UART_REG(u->reg_base, UART_THR)) = c;
    popoff_hart();
}

char ns16550a_driver_getc(struct device_struct *dev)
{
    struct ns16550a_internal_struct *u = (struct ns16550a_internal_struct *)dev->driver_internal;
    if ((*UART_REG(u->reg_base, UART_LCR)) & 0x01)
        return *UART_REG(u->reg_base, UART_RHR);
    else
        return -1;
}

char ns16550a_driver_readc(struct device_struct *dev)
{
    struct ns16550a_internal_struct *u = (struct ns16550a_internal_struct *)dev->driver_internal;
    spinlock_acquire(&(u->rx_lock));
    while (u->rx_buffer_head == u->rx_buffer_tail)
        // empty;
        proc_sleep(&(u->rx_buffer_head), &(u->rx_lock));
    char ret = u->rx_buffer[u->rx_buffer_head % RX_BUFFER_SIZE];
    u->rx_buffer_head++;
    spinlock_release(&(u->rx_lock));
    return ret;
}

int ns16550a_driver_interrupt_handler(struct device_struct *dev)
{
    struct ns16550a_internal_struct *u = (struct ns16550a_internal_struct *)dev->driver_internal;
    char rec_c = ns16550a_driver_getc(dev);
    while (rec_c != -1)
    {
        u->rx_buffer[u->rx_buffer_tail % RX_BUFFER_SIZE] = rec_c;
        u->rx_buffer_tail++;
        proc_wakeup(&(u->rx_buffer_head));
        rec_c = ns16550a_driver_getc(dev);
    }
    spinlock_acquire(&(u->tx_lock));
    ns16550a_driver_flush(dev);
    spinlock_release(&(u->tx_lock));
}

bool ns16550a_driver_recognize_device(struct fdt_header *fdt, int node_offset)
{
    return (strncmp(fdt_get_name(fdt, node_offset, NULL), "uart@", sizeof("uart@") - 1) == 0 &&
            strcmp(fdt_get_property(fdt, node_offset, "compatible", NULL)->data, "ns16550a") == 0);
}

struct device_struct *ns16550a_driver_init(int devId, struct fdt_header *fdt, int node_offset)
{
    struct ns16550a_internal_struct *u = kmem_alloc(sizeof(struct ns16550a_internal_struct));
    if (u == NULL)
        return NULL;
    struct device_struct *dev = kmem_alloc(sizeof(struct device_struct));
    if (dev == NULL)
    {
        kmem_free(u);
        return NULL;
    }
    struct irq_struct *u_irq = kmem_alloc(sizeof(struct irq_struct));
    if (u_irq == NULL)
    {
        kmem_free(u), kmem_free(dev);
        return NULL;
    }
    struct hal_uart_device_struct *hal_u = kmem_alloc(sizeof(struct hal_uart_device_struct));
    if (u_irq == NULL)
        goto error_cleanup;

    u->devId = devId;

    // read the fdt;
    struct fdt_property *prop = fdt_get_property(fdt, node_offset, "interrupts", NULL);
    if (prop == NULL || fdt32_to_cpu(prop->len) != sizeof(int))
        goto error_cleanup;
    u->interrupt_src_id = fdt32_to_cpu(*((int *)prop->data));

    prop = fdt_get_property(fdt, node_offset, "clock-frequency", NULL);
    if (prop == NULL || fdt32_to_cpu(prop->len) != sizeof(int))
        goto error_cleanup;
    u->clock_freq = fdt32_to_cpu(*((int *)prop->data));

    prop = fdt_get_property(fdt, node_offset, "reg", NULL);
    if (prop == NULL || fdt32_to_cpu(prop->len) != 2 * sizeof(uint64))
        goto error_cleanup;
    u->reg_base = fdt64_to_cpu(*((uint64 *)prop->data));
    u->reg_size = fdt64_to_cpu(*(((uint64 *)prop->data) + 1));

    // set device info;
    dev->name = "NS16550A Compatible UART Device";
    dev->fdt_node_offset = node_offset;
    dev->devId = devId;
    dev->driver = &ns16550a_driver;
    dev->driver_internal = (void *)u;
    dev->children = dev->parent = dev->next = NULL;

    // register the irq event;
    u_irq->handler = ns16550a_driver_interrupt_handler;
    u_irq->interrupt_src_id = u->interrupt_src_id;
    u_irq->dev = dev;
    if (hal_trap_register_irq(u_irq) != 0)
    {
        kmem_free(u_irq);
        goto error_cleanup;
    }

    // init the device;
    // - init the spinlock;
    spinlock_init(&(u->tx_lock), "ns16550a_tx_lock");
    spinlock_init(&(u->rx_lock), "ns16550a_rx_lock");
    // - disable the interrupt for now;
    *UART_REG(u->reg_base, UART_IER) = 0;
    // - set the baud rate;
    *UART_REG(u->reg_base, UART_LCR) = UART_LCR_BAUD_LATCH;
    // (exp) may test the given rate in FDT;
    *UART_REG(u->reg_base, 0) = 0x03;
    *UART_REG(u->reg_base, 1) = 0x00;
    // - set the word length;
    *UART_REG(u->reg_base, UART_LCR) = UART_LCR_EIGHT_BITS;
    // - reset and enable interrupt;
    *UART_REG(u->reg_base, UART_FCR) = (UART_FCR_FIFO_ENABLE | UART_FCR_FIFO_CLEAR);
    *UART_REG(u->reg_base, UART_IER) = (UART_IER_TX_ENABLE | UART_IER_RX_ENABLE);

    // HAL registry;
    hal_u->dev = dev;
    hal_u->write = ns16550a_driver_putc_async;
    hal_u->read = ns16550a_driver_readc;
    hal_uart_register_device(hal_u);
    return dev;

error_cleanup:
    kmem_free(u);
    kmem_free(dev);
    kmem_free(hal_u);
    return NULL;
}
