// vfs.h

#ifndef VFS

#define VFS

#include <types.h>
#include <device/block_device.h>

// VFS: Universal Definitions;

enum inode_type
{
    INODE_FILE,
    INODE_DIRECTORY
};

struct inode_struct
{
    enum inode_type type;
    size_t size;
    timestamp_t access_time;
    timestamp_t modification_time;
    struct block_device_struct *bdev;
    int ref_count;
    void *fs_internal;
};

struct dentry_struct
{
};

struct superblock_struct
{
};

// VFS: Filesystem Interface;

struct inode_operations_struct
{
};

struct fs_operations_struct
{
};

struct fs_struct
{
    char *fs_name;
};

// VFS: VFS API;

struct vfs_struct
{
      
};

void register_fs(struct fs_struct *fs);
void unregister_fs(struct fs_struct *fs);

#endif
