// virtio_blk.c
// referenced from xv6;
#include "virtio.h"



struct device_struct *virtio_blk_driver_init(struct virtio_internal_struct *virtio_info)
{
    struct device_struct *dev = kmem_alloc(sizeof(struct device_struct));
    if (dev == NULL)
    {
        kmem_free(virtio_info);
        return NULL;
    }
    dev->driver_internal = virtio_info;
    dev->devId = virtio_info->devId;
    dev->children = dev->parent = dev->next = NULL;
    dev->driver = &virtio_driver;
    dev->name = "VirtIO Block Device";
    // init the block device;
    // - reset the device status;
    uint32 status = 0;
    *VIRTIO_MMIO_STATUS_REG(virtio_info->reg_base) = status;
    // - ack;
    status |= VIRTIO_CONFIG_S_ACKNOWLEDGE;
    *VIRTIO_MMIO_STATUS_REG(virtio_info->reg_base) = status;
    status |= VIRTIO_CONFIG_S_DRIVER;
    *VIRTIO_MMIO_STATUS_REG(virtio_info->reg_base) = status;
    // - features;
    uint64 feature = *(volatile uint64 *)(VIRTIO_MMIO_DEVICE_FEATURES_REG(virtio_info->reg_base));
    

    // register to HAL;

    return dev;
}
