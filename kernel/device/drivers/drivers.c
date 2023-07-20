// drivers.c
#include <device.h>

struct driver_manager_struct driver_manager;

void device_drivers_append(struct driver_struct *drv)
{
    if (drv == NULL)
        return;
    drv->next = driver_manager.driver_list.next;
    driver_manager.driver_list.next = drv;
}

// Built-in drivers:
extern struct driver_struct plic_driver;
extern struct driver_struct ns16550a_driver;

void device_drivers_load(void)
{
    // Load built-in drivers;
    device_drivers_append(&plic_driver);
    device_drivers_append(&ns16550a_driver);
}
