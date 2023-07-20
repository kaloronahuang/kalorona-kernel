// uart_io.c
#include <hal/uart_io.h>
#include <device.h>

struct hal_uart_manager_struct hal_uart_manager;

int hal_uart_register_device(struct hal_uart_device_struct *dev)
{
    if (dev == NULL)
        return -1;
    dev->next = hal_uart_manager.dev_list.next;
    hal_uart_manager.dev_list.next = dev;
    return 0;
}

struct hal_uart_device_struct *hal_uart_find_device(struct device_struct *dev)
{
    for (struct hal_uart_device_struct *udev = hal_uart_manager.dev_list.next; udev != NULL; udev = udev->next)
        if (udev->dev == dev)
            return udev;
    return NULL;
}

void hal_uart_write(struct device_struct *dev, char c)
{
    struct hal_uart_device_struct *udev = hal_uart_find_device(dev);
    if (udev == NULL)
        return;
    udev->write(dev, c);
}

char hal_uart_read(struct device_struct *dev)
{
    struct hal_uart_device_struct *udev = hal_uart_find_device(dev);
    if (udev == NULL)
        return;
    return udev->read(dev);
}
