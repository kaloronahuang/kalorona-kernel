// device.h
#ifndef DEVICE

#define DEVICE

#include <device/libfdt/libfdt.h>

extern struct fdt_header *flatten_device_tree;

// Device Definitions;

struct device_struct
{
    char *name;
    dev_t devId;
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
    // fdt_header, node_offset;
    bool (*recognize_device)(struct fdt_header *, int);
    // devId, fdt_header, node_offset;
    void (*init)(int, struct fdt_header *, int);
    struct driver_struct *next;
};

int device_register(struct device_struct *);

struct device_struct *device_alloc_device(void);

void device_discover(void);

struct fdt_header *device_fdt_init(struct fdt_header *fdt);

void device_drivers_load(void);

void device_init(void);

void hal_init(void);

#endif
