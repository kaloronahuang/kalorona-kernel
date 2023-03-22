// bytes.c
#include <utilities/bytes.h>

void flip_bytes_inplace(void *x, size_t numOfBytes)
{
    uint8 *lft = x;
    uint8 *rig = lft + (numOfBytes - 1);
    while (lft < rig)
    {
        uint8 tmp;
        tmp = *lft;
        *lft = *rig;
        *rig = tmp;
        lft++, rig--;
    }
}

uint16 flip_bytes_16(uint16 x)
{
    flip_bytes_inplace(&x, 2);
    return x;
}

uint32 flip_bytes_32(uint32 x)
{
    flip_bytes_inplace(&x, 4);
    return x;
}

uint64 flip_bytes_64(uint64 x)
{
    flip_bytes_inplace(&x, 8);
    return x;
}