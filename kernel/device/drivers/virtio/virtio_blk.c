// virtio_blk.c
// referenced from xv6;
#include <hal/disk_io.h>
#include <signal.h>
#include "virtio.h"



struct device_struct *virtio_blk_driver_init(struct virtio_internal_struct *virtio_info)
{
    struct device_struct *dev = kmem_alloc(sizeof(struct device_struct));
    if (dev == NULL)
        goto init_failed;
    struct virtio_blk_internal_struct *blk = kmem_alloc(sizeof(struct virtio_blk_internal_struct));
    if (blk == NULL)
        goto init_failed;
    struct hal_disk_device_struct *hal_interface = kmem_alloc(sizeof(struct hal_disk_device_struct));
    if (hal_interface == NULL)
        goto init_failed;
    blk->virtio_internal = virtio_info;
    spinlock_init(&(blk->lock), "virtio_blk_lock");
    dev->driver_internal = blk;
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
    // - driver bit;
    status |= VIRTIO_CONFIG_S_DRIVER;
    *VIRTIO_MMIO_STATUS_REG(virtio_info->reg_base) = status;
    // - features;
    blk->supported_features = *(VIRTIO_MMIO_DEVICE_FEATURES_REG(virtio_info->reg_base));
    blk->read_only = ((blk->supported_features >> VIRTIO_BLK_F_RO) & 1);
    uint32 feat = blk->supported_features;
    // -- current support, maybe extended later;
    // toggle off some features;
    feat &= (~(1 << VIRTIO_BLK_F_CONFIG_WCE));
    feat &= (~(1 << VIRTIO_BLK_F_MQ));
    feat &= (~(1 << VIRTIO_BLK_F_SCSI));
    /*
    Thus when using the legacy interface,
    the VIRTIO_F_ANY_LAYOUT feature indicates to both the
    device and the driver that no assumptions were made
    about framing. Requirements for transitional drivers
    when this is not negotiated are included in each
    device section.
    */
    feat &= (~(1 << VIRTIO_F_ANY_LAYOUT));
    // may support later, potential optimization can be made here;
    feat &= (~(1 << VIRTIO_F_INDIRECT_DESC));
    feat &= (~(1 << VIRTIO_F_EVENT_IDX));
    // set the features;
    *VIRTIO_MMIO_DEVICE_FEATURES_REG(virtio_info->reg_base) = feat;
    status |= VIRTIO_CONFIG_S_FEATURES_OK;
    *VIRTIO_MMIO_STATUS_REG(virtio_info->reg_base) = status;
    // double check the feat;
    status = *VIRTIO_MMIO_STATUS_REG(virtio_info->reg_base);
    if (!(status & VIRTIO_CONFIG_S_FEATURES_OK))
        goto init_failed;
    // init the data transportation;
    // - check the max num of queues;
    uint32 max_queue_supported = *VIRTIO_MMIO_QUEUE_MAX_NUM_REG(blk->virtio_internal->reg_base);
    if (max_queue_supported < VIRTIO_NUM_QUEUE)
        goto init_failed;
    // - set;
    *VIRTIO_MMIO_QUEUE_NUM_REG(blk->virtio_internal->reg_base) = VIRTIO_NUM_QUEUE;
    // - log2(VIRTIO_NUM_QUEUE * sizeof(struct virtio_virtq_desc_struct));
    //   here i'll just keep a space of the size of page;
    blk->virtq.desc = kmem_alloc_pages(0);
    blk->virtq.avail = kmem_alloc_pages(0);
    blk->virtq.used = kmem_alloc_pages(0);
    memset(blk->virtq.desc, 0, PAGE_SIZE);
    memset(blk->virtq.avail, 0, PAGE_SIZE);
    memset(blk->virtq.used, 0, PAGE_SIZE);
    // - give it to the device;
    *VIRTIO_MMIO_QUEUE_DESC_LOW_REG(blk->virtio_internal->reg_base) = (uint32)(PMA_VA2PA(blk->virtq.desc));
    *VIRTIO_MMIO_QUEUE_DESC_HIGH_REG(blk->virtio_internal->reg_base) = (uint32)(PMA_VA2PA(blk->virtq.desc) >> 32);
    *VIRTIO_MMIO_QUEUE_DRIVER_LOW_REG(blk->virtio_internal->reg_base) = (uint32)(PMA_VA2PA(blk->virtq.avail));
    *VIRTIO_MMIO_QUEUE_DRIVER_HIGH_REG(blk->virtio_internal->reg_base) = (uint32)(PMA_VA2PA(blk->virtq.avail) >> 32);
    *VIRTIO_MMIO_QUEUE_DEVICE_LOW_REG(blk->virtio_internal->reg_base) = (uint32)(PMA_VA2PA(blk->virtq.used));
    *VIRTIO_MMIO_QUEUE_DEVICE_HIGH_REG(blk->virtio_internal->reg_base) = (uint32)(PMA_VA2PA(blk->virtq.used) >> 32);
    // - queues are ready;
    *VIRTIO_MMIO_QUEUE_READY_REG(blk->virtio_internal->reg_base) = 0x1;
    for (int i = 0; i < VIRTIO_NUM_QUEUE; i++)
        blk->free_map[i] = false;
    // - all set;
    status |= VIRTIO_CONFIG_S_DRIVER_OK;
    *VIRTIO_MMIO_STATUS_REG(blk->virtio_internal->reg_base) = status;
    // get the blk config;
    struct virtio_blk_config cfg;
    uint32 cfg_gen_bef, cfg_gen_aft;
    do {
        cfg_gen_bef = *VIRTIO_MMIO_CONFIG_GENERATION_REG(blk->virtio_internal->reg_base);
        virtio_memcpy(&cfg, (void *)PMA_PA2VA(VIRTIO_MMIO_CONFIG_REG(blk->virtio_internal->reg_base)), sizeof(struct virtio_blk_config));
        cfg_gen_aft = *VIRTIO_MMIO_CONFIG_GENERATION_REG(blk->virtio_internal->reg_base);
    } while (cfg_gen_bef != cfg_gen_aft);
    // register to HAL;
    hal_interface->dev = dev;
    hal_interface->read_only = blk->read_only;
    hal_interface->block_size = cfg.blk_size;
    // - capacity in 512-bytes;
    if ((cfg.capacity << 9) % hal_interface->block_size != 0)
        panic("virtio_blk_driver_init - block size alignment");
    hal_interface->num_of_block = (cfg.capacity << 9) / hal_interface->block_size;
    hal_interface->read = ;
    hal_interface->write = ;
    return dev;

init_failed:
    status = VIRTIO_CONFIG_S_FAILED;
    if (dev == NULL)
        kmem_free(dev);
    if (virtio_info == NULL)
        kmem_free(virtio_info);
    if (blk == NULL)
        kmem_free(blk);
    if (hal_interface == NULL)
        kmem_free(hal_interface);
    return NULL;
}
