// hal.c
#include <device.h>
#include <hal/trap.h>
#include <hal/uart_io.h>
#include <hal/block_io.h>

void hal_init(void)
{
    hal_trap_init();
    // activate HAL modules;
}
