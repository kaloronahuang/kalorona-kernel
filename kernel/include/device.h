// device.h
#ifndef DEVICE

#define DEVICE

#include <device/libfdt/libfdt.h>

extern struct fdt_header *flatten_device_tree;

// Device Definitions;

enum device_type
{
    BLOCK_IO_DEVICE,
    UART_IO_DEVICE,
    UNRECOGNIZED_DEVICE
};

struct device_struct
{
    char *name;
    uint devId;
    enum device_type devType;
    struct driver_struct *driver;
    void *driver_internal;

    struct device_struct *parent;
    struct device_struct *children;

    struct device_struct *next;
};

struct driver_struct
{
    char *name;
    int version;
    char *developer;
    bool (*recognize_device)(struct fdt_header, int);
    void (*init)(struct fdt_header, int);
};

extern struct driver_struct device_drivers[];

int device_register(struct device_struct *);

struct device_struct *device_alloc_device(void);

void device_discover(void);

struct fdt_header *device_fdt_init(struct fdt_header *fdt);

void device_init(void);

void hal_init(void);

#endif
