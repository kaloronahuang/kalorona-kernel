// plic.c
// reference from xv6;
// RISC-V Platform Level Interrupt Controller (PLIC).
#include <device.h>
#include <kmem.h>
#include <hal/trap.h>
#include <proc.h>
#include <utilities/bytes.h>

// Macros;
#define PLIC_REG(base, reg_id) ((volatile unsigned char *)(PMA_PA2VA(((ulong)base) + ((ulong)reg_id))))
#define PLIC_PRIORITY(base) PLIC_REG(base, 0x0)
#define PLIC_PENDING(base) PLIC_REG(base, 0x1000)
#define PLIC_MENABLE(base, hart) PLIC_REG(base, 0x2000 + (hart)*0x100)
#define PLIC_SENABLE(base, hart) PLIC_REG(base, 0x2080 + (hart)*0x100)
#define PLIC_MPRIORITY(base, hart) PLIC_REG(base, 0x200000 + (hart)*0x2000)
#define PLIC_SPRIORITY(base, hart) PLIC_REG(base, 0x201000 + (hart)*0x2000)
#define PLIC_MCLAIM(base, hart) PLIC_REG(base, 0x200004 + (hart)*0x2000)
#define PLIC_SCLAIM(base, hart) PLIC_REG(base, 0x201004 + (hart)*0x2000)

struct plic_internal_struct
{
    dev_t devId;
    uint64 reg_base;
    uint64 reg_size;
};

bool plic_driver_recognize_device(struct fdt_header *fdt, int node_offset);
struct device_struct *plic_driver_init(int devId, struct fdt_header *fdt, int node_offset);

void set_u32_map_volatile_bit(volatile uint32 *map, ulong index, int b)
{
    if (b)
        map[index >> 5] |= (1 << (index & 0x1f));
    else
        map[index >> 5] &= (~(((uint32)1) << (index & 0x1f)));
}

struct driver_struct plic_driver = {
    .developer = "Kalorona Huang",
    .name = "RISC-V Platform Level Interrupt Controller Driver",
    .version = 0,
    .init = plic_driver_init,
    .recognize_device = plic_driver_recognize_device};

// HAL event handlers;

void plic_driver_hal_enable_src(struct device_struct *dev, uint src_id)
{
    struct plic_internal_struct *u = (struct plic_internal_struct *)dev->driver_internal;
    *(uint32 *)(PLIC_REG(u->reg_base, 4 * src_id)) = 1;
    for (int i = 0; i < MAX_CPU; i++)
        if (harts[i].enabled)
            set_u32_map_volatile_bit((uint32 *)PLIC_SENABLE(u->reg_base, i), src_id, 1);
}

void plic_driver_hal_disable_src(struct device_struct *dev, uint src_id)
{
    struct plic_internal_struct *u = (struct plic_internal_struct *)dev->driver_internal;
    *(uint32 *)(PLIC_REG(u->reg_base, 4 * src_id)) = 0;
    for (int i = 0; i < MAX_CPU; i++)
        if (harts[i].enabled)
            set_u32_map_volatile_bit((uint32 *)PLIC_SENABLE(u->reg_base, i), src_id, 0);
}

int plic_driver_hal_check_src_status(struct device_struct *dev, uint src_id)
{
    struct plic_internal_struct *u = (struct plic_internal_struct *)dev->driver_internal;
    return *(uint32 *)(PLIC_REG(u->reg_base, 4 * src_id));
}

uint64 plic_driver_hal_claim(struct device_struct *dev)
{
    struct plic_internal_struct *u = (struct plic_internal_struct *)dev->driver_internal;
    return *(uint32 *)(PLIC_SCLAIM(u->reg_base, current_hart_id()));
}

void plic_driver_hal_complete(struct device_struct *dev, uint irq_id)
{
    struct plic_internal_struct *u = (struct plic_internal_struct *)dev->driver_internal;
    *(uint32 *)(PLIC_SCLAIM(u->reg_base, current_hart_id())) = irq_id;
}

void plic_driver_hal_init(struct device_struct *dev, struct irq_struct *irq_list)
{
    for (struct irq_struct *irq = irq_list->next; irq != NULL; irq = irq->next)
        plic_driver_hal_enable_src(dev, irq->interrupt_src_id);
}

bool plic_driver_recognize_device(struct fdt_header *fdt, int node_offset)
{
    const char compatible_str[] = "riscv,plic";
    const char intc_str[] = "interrupt-controller@";
    return (strncmp(fdt_get_name(fdt, node_offset, NULL), intc_str, sizeof(intc_str) - 1) == 0 &&
            strncmp(fdt_get_property(fdt, node_offset, "compatible", NULL)->data, compatible_str, sizeof(compatible_str) - 1) == 0);
}

struct device_struct *plic_driver_init(int devId, struct fdt_header *fdt, int node_offset)
{
    struct plic_internal_struct *u = kmem_alloc(sizeof(struct plic_internal_struct));
    if (u == NULL)
        return NULL;
    struct device_struct *dev = kmem_alloc(sizeof(struct device_struct));
    if (dev == NULL)
    {
        kmem_free(u);
        return NULL;
    }
    struct interruptor_struct *hal_entry = kmem_alloc(sizeof(struct interruptor_struct));
    if (hal_entry == NULL)
    {
        kmem_free(u);
        kmem_free(dev);
        return NULL;
    }
    u->devId = devId;
    // read from fdt;
    const struct fdt_property *prop = fdt_get_property(fdt, node_offset, "reg", NULL);
    if (prop == NULL || fdt32_to_cpu(prop->len) != 2 * sizeof(uint64))
        goto error_cleanup;
    u->reg_base = fdt64_to_cpu(*((uint64 *)prop->data));
    u->reg_size = fdt64_to_cpu(*(((uint64 *)prop->data) + 1));
    // enable plic for every hart;
    for (int i = 0; i < MAX_CPU; i++)
        if (harts[i].enabled)
            *(uint32 *)(PLIC_SPRIORITY(u->reg_base, i)) = 0;
    // setup the device descriptor;
    dev->devId = devId;
    dev->fdt_node_offset = node_offset;
    dev->driver = &plic_driver;
    dev->driver_internal = (void *)u;
    dev->name = "RISC-V Platform Level Interrupt Controller";
    dev->children = dev->parent = dev->next = NULL;
    // register at the HAL;
    hal_entry->dev = dev;
    hal_entry->init = plic_driver_hal_init;
    hal_entry->enable_src = plic_driver_hal_enable_src;
    hal_entry->disable_src = plic_driver_hal_disable_src;
    hal_entry->check_src_status = plic_driver_hal_check_src_status;
    hal_entry->claim = plic_driver_hal_claim;
    hal_entry->complete = plic_driver_hal_complete;
    hal_trap_register_interruptor(hal_entry);
    return dev;
error_cleanup:
    kmem_free(u);
    kmem_free(dev);
    kmem_free(hal_entry);
    return NULL;
}
