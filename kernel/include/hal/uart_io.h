// uart_io.h

#ifndef HAL_UART_IO

#define HAL_UART_IO

#include <types.h>

struct hal_uart_device_struct
{
    dev_t devId;
    char *name;
    void (*write)(char);
    char (*read)(void);
    struct hal_uart_device_struct *next;
};

int hal_uart_register_device(struct hal_uart_device_struct *dev);

void hal_uart_write(dev_t devId, char c);

char hal_uart_read(dev_t devId);

#endif
