// virtio.h

#ifndef KDRIVER_VIRTIO

#include <device.h>
#include <vmem.h>

#define KDRIVER_VIRTIO

// VirtIO General Definitions;

// VirtIO registers over MMIO;

#define VIRTIO_MMIO_REG(base, reg_id) (volatile uint32 *)(PMA_PA2VA(((ulong)base) + ((ulong)reg_id)))
// read-only;
#define VIRTIO_MMIO_MAGIC_VALUE_REG(base) VIRTIO_MMIO_REG(base, 0x0)
// read-only;
#define VIRTIO_MMIO_VERSION_REG(base) VIRTIO_MMIO_REG(base, 0x4)
// read-only;
#define VIRTIO_MMIO_DEVICE_TYPE_REG(base) VIRTIO_MMIO_REG(base, 0x8)
// read-only;
#define VIRTIO_MMIO_VENDOR_ID_REG(base) VIRTIO_MMIO_REG(base, 0xc)
// write-only;
#define VIRTIO_MMIO_DEVICE_FEATURES_REG(base) VIRTIO_MMIO_REG(base, 0x10)
// write-only;
#define VIRTIO_MMIO_DRIVER_FEATURES_REG(base) VIRTIO_MMIO_REG(base, 0x20)
// write-only;
#define VIRTIO_MMIO_QUEUE_SELECT_REG(base) VIRTIO_MMIO_REG(base, 0x30)
// read-only;
#define VIRTIO_MMIO_QUEUE_MAX_NUM_REG(base) VIRTIO_MMIO_REG(base, 0x34)
// write-only;
#define VIRTIO_MMIO_QUEUE_NUM_REG(base) VIRTIO_MMIO_REG(base, 0x38)
#define VIRTIO_MMIO_QUEUE_READY_REG(base) VIRTIO_MMIO_REG(base, 0x44)
// write-only;
#define VIRTIO_MMIO_QUEUE_NOTIFY_REG(base) VIRTIO_MMIO_REG(base, 0x50)
// read-only;
#define VIRTIO_MMIO_INTERRUPT_STATUS_REG(base) VIRTIO_MMIO_REG(base, 0x60)
// write-only;
#define VIRTIO_MMIO_INTERRUPT_ACK_REG(base) VIRTIO_MMIO_REG(base, 0x64)
#define VIRTIO_MMIO_STATUS_REG(base) VIRTIO_MMIO_REG(base, 0x70)
// write-only;
#define VIRTIO_MMIO_QUEUE_DESC_LOW_REG(base) VIRTIO_MMIO_REG(base, 0x80)
// write-only;
#define VIRTIO_MMIO_QUEUE_DESC_HIGH_REG(base) VIRTIO_MMIO_REG(base, 0x84)
// write-only;
#define VIRTIO_MMIO_QUEUE_DRIVER_LOW_REG(base) VIRTIO_MMIO_REG(base, 0x90)
// write-only;
#define VIRTIO_MMIO_QUEUE_DRIVER_HIGH_REG(base) VIRTIO_MMIO_REG(base, 0x94)
// write-only;
#define VIRTIO_MMIO_QUEUE_DEVICE_LOW_REG(base) VIRTIO_MMIO_REG(base, 0xa0)
// write-only;
#define VIRTIO_MMIO_QUEUE_DEVICE_HIGH_REG(base) VIRTIO_MMIO_REG(base, 0xa4)
// read-only;
#define VIRTIO_MMIO_CONFIG_GENERATION_REG(base) VIRTIO_MMIO_REG(base, 0xfc)

// VirtIO Constants;

// Magic Value;
#define VIRTIO_MAGIC_VALUE 0x74726976
// Device Types;
#define VIRTIO_NETWORK_CARD 1
#define VIRTIO_BLOCK_DEVICE 2
#define VIRTIO_CONSOLE 3

#define VIRTIO_CONFIG_S_ACKNOWLEDGE 1
#define VIRTIO_CONFIG_S_DRIVER 2
#define VIRTIO_CONFIG_S_DRIVER_OK 4
#define VIRTIO_CONFIG_S_FEATURES_OK 8

struct virtio_internal_struct
{
    dev_t devId;
    uint64 reg_base;
    uint64 reg_size;
    uint interrupt_src_id;
    uint virtio_device_id;
};

bool virtio_driver_recognize_device(struct fdt_header *fdt, int node_offset);
struct device_struct *virtio_driver_init(int devId, struct fdt_header *fdt, int node_offset);

extern struct driver_struct virtio_driver;

// Block Device Definitions;

#define VIRTIO_BLK_F_SIZE_MAX 1
#define VIRTIO_BLK_F_SEG_MAX 2
#define VIRTIO_BLK_F_GEOMETRY 4
#define VIRTIO_BLK_F_RO 5
#define VIRTIO_BLK_F_BLK_SIZE 6
#define VIRTIO_BLK_F_FLUSH 9
#define VIRTIO_BLK_F_TOPOLOGY 10
#define VIRTIO_BLK_F_CONFIG_WCE 11
#define VIRTIO_BLK_F_MQ 12
#define VIRTIO_BLK_F_DISCARD 13
#define VIRTIO_BLK_F_WRITE_ZEROES 14
#define VIRTIO_BLK_F_LIFETIME 15
#define VIRTIO_BLK_F_SECURE_ERASE 16
#define VIRTIO_BLK_F_BARRIER 0
#define VIRTIO_BLK_F_SCSI 7

struct virtio_blk_config
{
    uint64 capacity;
    uint32 size_max;
    uint32 seg_max;
    struct virtio_blk_geometry
    {
        uint16 cylinders;
        uint8 heads;
        uint8 sectors;
    } geometry;
    uint32 blk_size;
    struct virtio_blk_topology
    {
        // # of logical blocks per physical block (log2)
        uint8 physical_block_exp;
        // offset of first aligned logical block
        uint8 alignment_offset;
        // suggested minimum I/O size in blocks
        uint16 min_io_size;
        // optimal (suggested maximum) I/O size in blocks
        uint32 opt_io_size;
    } topology;
    uint8 writeback;
    uint8 unused0;
    uint16 num_queues;
    uint32 max_discard_sectors;
    uint32 max_discard_seg;
    uint32 discard_sector_alignment;
    uint32 max_write_zeroes_sectors;
    uint32 max_write_zeroes_seg;
    uint8 write_zeroes_may_unmap;
    uint8 unused1[3];
    uint32 max_secure_erase_sectors;
    uint32 max_secure_erase_seg;
    uint32 secure_erase_sector_alignment;
};

#define VIRTIO_BLK_T_IN 0
#define VIRTIO_BLK_T_OUT 1
#define VIRTIO_BLK_T_FLUSH 4
#define VIRTIO_BLK_T_GET_ID 8
#define VIRTIO_BLK_T_GET_LIFETIME 10
#define VIRTIO_BLK_T_DISCARD 11
#define VIRTIO_BLK_T_WRITE_ZEROES 13
#define VIRTIO_BLK_T_SECURE_ERASE 14

struct virtio_blk_req
{
    uint32 type;
    uint32 reserved;
    uint64 sector;
};

#define VIRTIO_BLK_S_OK 0
#define VIRTIO_BLK_S_IOERR 1
#define VIRTIO_BLK_S_UNSUPP 2

struct virtio_blk_internal_struct
{
    struct virtio_internal_struct virtio_info;
    bool read_only;
};

struct device_struct *virtio_blk_driver_init(struct virtio_internal_struct *virtio_info);

// NIC Device Definitions;

struct virtio_nic_internal_struct
{
    struct virtio_internal_struct virtio_info;
};

#endif
