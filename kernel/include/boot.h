// boot.h

// Shared by the virtual kernel and physical kernel entrypoint;

#ifndef BOOT

#define BOOT

#define BOOT_COMMAND_MAX_LEN 1024

struct boot_command
{
    int agrc;
    char agrv[BOOT_COMMAND_MAX_LEN];
};

extern struct boot_command vboot_cmd;

#endif