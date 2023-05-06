# kimage_defs.S
# In order to solve the limit brought by the memory model,
# we have the values of distant addresses here;

.section .rodata

.global pkernel_hsm_entry
pkernel_hsm_entry:
    .dword __hsm_entry

.global PA_BEGIN
PA_BEGIN:
    .dword __PA_BEGIN

.global KERNEL_IMG_PA_BEGIN
KERNEL_IMG_PA_BEGIN:
    .dword __KERNEL_IMG_PA_BEGIN
.global MEMORY_PA_BEGIN
MEMORY_PA_BEGIN:
    .dword __MEMORY_PA_BEGIN

.global KERNEL_USER_HANDLER_PA_BEGIN
KERNEL_USER_HANDLER_PA_BEGIN:
    .dword __KERNEL_USER_HANDLER_PA_BEGIN

.global KERNEL_TEXT_PA_BEGIN
KERNEL_TEXT_PA_BEGIN:
    .dword __KERNEL_TEXT_PA_BEGIN
.global KERNEL_RODATA_PA_BEGIN
KERNEL_RODATA_PA_BEGIN:
    .dword __KERNEL_RODATA_PA_BEGIN
.global KERNEL_DATA_PA_BEGIN
KERNEL_DATA_PA_BEGIN:
    .dword __KERNEL_DATA_PA_BEGIN
.global KERNEL_BSS_PA_BEGIN
KERNEL_BSS_PA_BEGIN:
    .dword __KERNEL_BSS_PA_BEGIN

.global KERNEL_IMG_PA_END
KERNEL_IMG_PA_END:
    .dword __KERNEL_IMG_PA_END

.global KERNEL_IMG_VA_BEGIN
KERNEL_IMG_VA_BEGIN:
    .dword __KERNEL_IMG_VA_BEGIN
.global KERNEL_IMG_VA_END
KERNEL_IMG_VA_END:
    .dword __KERNEL_IMG_VA_END
