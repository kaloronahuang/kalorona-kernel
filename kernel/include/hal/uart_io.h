// uart_io.h

#ifndef HAL_UART_IO

#define HAL_UART_IO

#include <types.h>

struct hal_uart_device_struct
{
    struct device_struct *dev;
    void (*write)(struct device_struct *, char);
    char (*read)(struct device_struct *);
    struct hal_uart_device_struct *next;
};

struct hal_uart_manager_struct
{
    struct hal_uart_device_struct dev_list;
};

extern struct hal_uart_manager_struct hal_uart_manager;

int hal_uart_register_device(struct hal_uart_device_struct *dev);

void hal_uart_write(struct device_struct *dev, char c);

char hal_uart_read(struct device_struct *dev);

#endif
