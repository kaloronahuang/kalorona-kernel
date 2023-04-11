// kmem.c
// kmem managaes all available physical memory pages through PMA in virtual memory mode;
#include <kmem.h>
#include <lock.h>
#include <signal.h>
#include <utilities/string.h>
#include <device/memory.h>
#include <kimage_defs.h>
#include <console.h>
#include <vmem.h>

struct kmem_struct kmem;

void kmem_init(void)
{
    for (int i = 0; i < ram_descriptor.available_ram_segments_num; i++)
    {
        char *beg = PMA_PA2VA(ram_descriptor.available_ram_segments[i].pa_begin);
    }
}
