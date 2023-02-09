// sbi.h
#include <types.h>

#ifndef ARCH_RISCV_SBI

#define ARCH_RISCV_SBI

#define SBI_FDT_ADDR 0x82200000

#define SBI_SUCCESS 0
#define SBI_ERR_FAILURE -1
#define SBI_ERR_NOT_SUPPORTED -2
#define SBI_ERR_INVALID_PARAM -3
#define SBI_ERR_DENIED -4
#define SBI_ERR_INVALID_ADDRESS -5
#define SBI_ERR_ALREADY_AVAILABLE -6
#define SBI_ERR_ALREADY_STARTED -7
#define SBI_ERR_ALREADY_STOPPED -8

struct sbiret
{
    long error;
    long value;
};

struct sbiret sbi_ecall(
    int eid,
    int fid,
    unsigned long arg0,
    unsigned long arg1,
    unsigned long arg2,
    unsigned long arg3,
    unsigned long arg4,
    unsigned long arg5);

enum sbi_eid
{
    SBI_BASE_EXT = 0x10,

    SBI_LEGACY_EXT_SET_TIMER = 0x00,
    SBI_LEGACY_EXT_PUTCHAR = 0x01,
    SBI_LEGACY_EXT_GETCHAR = 0x02,
    SBI_LEGACY_EXT_CLEAR_IPI = 0x03,
    SBI_LEGACY_EXT_SEND_IPI = 0x04,
    SBI_LEGACY_EXT_REMOTE_FENCE_I = 0x05,
    SBI_LEGACY_EXT_REMOTE_SFENCE_VMA = 0x06,
    SBI_LEGACY_EXT_REMOTE_SFENCE_VMA_W_ASID = 0x07,
    SBI_LEGACY_EXT_SHUTDOWN = 0x08,

    SBI_TIMER_EXT = 0x54494D45,
    SBI_IPI_EXT = 0x735049,
    SBI_RFENCE_EXT = 0x52464E43,
    SBI_HSM_EXT = 0x48534D,
    SBI_SRST_EXT = 0x53525354,
    SBI_PMU_EXT = 0x504D55,
    SBI_DBCN_EXT = 0x4442434E
};

// Base Extension (EID #0x10)

enum sbi_base_ext_fid
{
    SBI_BASE_GET_SPEC_VERSION_FID = 0,
    SBI_BASE_GET_IMPL_ID_FID = 1,
    SBI_BASE_GET_IMPL_VERSION_FID = 2,
    SBI_BASE_PROBE_EXTENSION_FID = 3,
    SBI_BASE_GET_MAVENDORID_FID = 4,
    SBI_BASE_GET_MARCHID_FID = 5,
    SBI_BASE_GET_MIMPID_FID = 6
};

enum sbi_impl_id
{
    SBI_IMPL_ID_BBL = 0,
    SBI_IMPL_ID_OPENSBI = 1,
    SBI_IMPL_ID_XVISOR = 2,
    SBI_IMPL_ID_KVM = 3,
    SBI_IMPL_ID_RUSTSBI = 4,
    SBI_IMPL_ID_DIOSIX = 5,
    SBI_IMPL_ID_COFFER = 6
};

struct sbiret sbi_get_spec_version(void);

struct sbiret sbi_get_impl_id(void);

struct sbiret sbi_get_impl_version(void);

struct sbiret sbi_probe_extension(long extension_id);

struct sbiret sbi_get_mvendorid(void);

struct sbiret sbi_get_marchid(void);

struct sbiret sbi_get_mimpid(void);

// Legacy Extension

long sbi_legacy_console_putchar(int ch);

long sbi_legacy_console_getchar(void);

// System Reset Extension

enum sbi_srst_fid
{
    SBI_SRST_SYSTEM_RESET_FID= 0
};

struct sbiret sbi_srst_system_reset(uint32 reset_type, uint32 reset_reason);

// Debug Console Extension
// Not Supported Yet!

enum sbi_dbcn_fid
{
    SBI_DBCN_CONSOLE_WRITE = 0,
    SBI_DBCN_CONSOLE_READ = 1,
    SBI_DBCN_CONSOLE_WRITE_BYTE = 2,
};

struct sbiret sbi_debug_console_write(ulong num_bytes,
                                      ulong base_addr_lo,
                                      ulong base_addr_hi);

struct sbiret sbi_debug_console_read(ulong num_bytes,
                                     ulong base_addr_lo,
                                     ulong base_addr_hi);

struct sbiret sbi_debug_console_write_byte(uchar byte);

#endif