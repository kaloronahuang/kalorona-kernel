// sbi.c
#include <asm/sbi.h>
#include <types.h>

// Borrowed from Linux;
struct sbiret sbi_ecall(
    int eid,
    int fid,
    unsigned long arg0,
    unsigned long arg1,
    unsigned long arg2,
    unsigned long arg3,
    unsigned long arg4,
    unsigned long arg5)
{
    struct sbiret ret;

    register ulong a0 asm("a0") = (ulong)(arg0);
    register ulong a1 asm("a1") = (ulong)(arg1);
    register ulong a2 asm("a2") = (ulong)(arg2);
    register ulong a3 asm("a3") = (ulong)(arg3);
    register ulong a4 asm("a4") = (ulong)(arg4);
    register ulong a5 asm("a5") = (ulong)(arg5);
    register ulong a6 asm("a6") = (ulong)(fid);
    register ulong a7 asm("a7") = (ulong)(eid);
    asm volatile("ecall"
                 : "+r"(a0), "+r"(a1)
                 : "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a6), "r"(a7)
                 : "memory");
    ret.error = a0;
    ret.value = a1;

    return ret;
}

// System Reset Extension
struct sbiret sbi_srst_system_reset(uint32 reset_type, uint32 reset_reason)
{
    return sbi_ecall(SBI_SRST_EXT, SBI_SRST_SYSTEM_RESET_FID, reset_type, reset_reason, 0, 0, 0, 0);
}

long sbi_legacy_console_putchar(int ch)
{
    struct sbiret ret = sbi_ecall(SBI_LEGACY_EXT_PUTCHAR, 0, ch, 0, 0, 0, 0, 0);
    return ret.error;
}

long sbi_legacy_console_getchar(void)
{
    struct sbiret ret = sbi_ecall(SBI_LEGACY_EXT_GETCHAR, 0, 0, 0, 0, 0, 0, 0);
    return ret.error;
}

// Definitions are on riscv-non-isa/riscv-sbi-doc;

struct sbiret sbi_debug_console_write(ulong num_bytes,
                                      ulong base_addr_lo,
                                      ulong base_addr_hi)
{
    return sbi_ecall(SBI_DBCN_EXT, SBI_DBCN_CONSOLE_WRITE, num_bytes, base_addr_lo, base_addr_hi, 0, 0, 0);
}

struct sbiret sbi_debug_console_read(ulong num_bytes,
                                     ulong base_addr_lo,
                                     ulong base_addr_hi)
{
    return sbi_ecall(SBI_DBCN_EXT, SBI_DBCN_CONSOLE_READ, num_bytes, base_addr_lo, base_addr_hi, 0, 0, 0);
}

struct sbiret sbi_debug_console_write_byte(uchar byte)
{
    return sbi_ecall(SBI_DBCN_EXT, SBI_DBCN_CONSOLE_WRITE_BYTE, byte, 0, 0, 0, 0, 0);
}
