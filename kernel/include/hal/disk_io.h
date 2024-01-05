// disk_io.h

#ifndef HAL_DISK_IO

#define HAL_DISK_IO

#include <types.h>

struct hal_disk_device_struct
{
    struct device_struct *dev;
    bool read_only;
    size_t block_size;
    size_t num_of_block;
    int (*read)(size_t block_idx, size_t num_of_block, const char *dst);
    int (*write)(size_t block_idx, size_t num_of_block, const char *src);
};

#endif
