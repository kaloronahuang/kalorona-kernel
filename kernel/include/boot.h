// boot.h

// Shared by the virtual kernel and physical kernel entrypoint;

#ifndef BOOT

#define BOOT

#define BOOT_COMMAND_MAX_LEN 1024

struct boot_command
{
    int argc;
    char argv[BOOT_COMMAND_MAX_LEN];
};

extern struct boot_command vboot_cmd;

void kernel_main(int argc, char *const argv[]);

#endif