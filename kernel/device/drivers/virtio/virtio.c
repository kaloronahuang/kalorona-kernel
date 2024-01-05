// virtio.c
#include "virtio.h"
#include <signal.h>

void virtio_memcpy(void *dst, const void *src, size_t siz)
{
    if ((siz & ((1 << 2) - 1)) != 0)
        panic("virtio_memcpy - size not aligned in 32-bits");
    volatile uint32 *dst_ptr = (volatile uint32 *)dst;
    const volatile uint32 *src_ptr = (volatile uint32 *)src;
    while (siz > 0)
        *(dst_ptr++) = *(src_ptr++), siz -= 4;
    return dst;
}

struct driver_struct virtio_driver = {
    .developer = "Kalorona Huang",
    .name = "VirtIO Driver",
    .version = 0,
    .init = virtio_driver_init,
    .recognize_device = virtio_driver_recognize_device};

bool virtio_driver_recognize_device(struct fdt_header *fdt, int node_offset)
{
    const struct fdt_property *p = fdt_get_property(fdt, node_offset, "compatible", NULL);
    if (p == NULL)
        return false;
    return strcmp(p->data, "virtio,mmio") == 0;
}

struct device_struct *virtio_driver_init(int devId, struct fdt_header *fdt, int node_offset)
{
    struct virtio_internal_struct *virtio_info = kmem_alloc(sizeof(struct virtio_internal_struct));
    if (virtio_info == NULL)
        return NULL;

    virtio_info->devId = devId;
    // reg;
    const struct fdt_property *prop = fdt_get_property(fdt, node_offset, "reg", NULL);
    if (prop == NULL)
    {
        kmem_free(virtio_info);
        return NULL;
    }
    virtio_info->reg_base = fdt64_to_cpu(*((fdt64_t *)(prop->data)));
    virtio_info->reg_size = fdt64_to_cpu(*(((fdt64_t *)(prop->data)) + 1));
    // interrupt_src_id;
    prop = fdt_get_property(fdt, node_offset, "interrupts", NULL);
    if (prop == NULL)
    {
        kmem_free(virtio_info);
        return NULL;
    }
    virtio_info->interrupt_src_id = fdt32_to_cpu(*((fdt32_t *)(prop->data)));
    // verification;
    if (*VIRTIO_MMIO_MAGIC_VALUE_REG(virtio_info->reg_base) != VIRTIO_MAGIC_VALUE || *VIRTIO_MMIO_DEVICE_TYPE_REG(virtio_info->reg_base) == 0x0)
    {
        kmem_free(virtio_info);
        return NULL;
    }
    // read info;
    uint32 dev_typ = *VIRTIO_MMIO_DEVICE_TYPE_REG(virtio_info->reg_base);
    virtio_info->virtio_device_id = dev_typ;
    switch (dev_typ)
    {
    case VIRTIO_NETWORK_CARD:
        // TODO;
        break;
    case VIRTIO_BLOCK_DEVICE:
        return virtio_blk_driver_init(virtio_info);
        break;
    default:
        kmem_free(virtio_info);
        return NULL;
    }
}
