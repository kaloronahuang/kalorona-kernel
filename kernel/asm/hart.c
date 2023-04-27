// hart.c
#include <types.h>
#include <asm/hart.h>
#include <asm/registers.h>
#include <asm/scsr.h>
#include <signal.h>

size_t hart_count;

struct hart_struct harts[MAX_CPU];

uint current_hart_id()
{
    return r_tp();
}

struct hart_struct *current_hart()
{
    return &(harts[current_hart_id()]);
}

void pushoff_hart()
{
    int prev_mode = read_interrupt();

    disable_interrupt();
    struct hart_struct* chart = current_hart();
    if (chart->trap_pushoff_dep == 0)
        chart->trap_enabled = prev_mode;
    chart->trap_pushoff_dep += 1;
}

void popoff_hart()
{
    struct hart_struct* chart = current_hart();
    if (read_interrupt())
        panic("popoff_hart - already enabled;");
    if (chart->trap_pushoff_dep < 1)
        panic("popoff_hart - non-matching popoff;");
    chart->trap_pushoff_dep -= 1;
    if (chart->trap_pushoff_dep == 0 && chart->trap_enabled)
        enable_interrupt();
}