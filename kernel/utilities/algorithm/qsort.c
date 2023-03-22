// qsort.c
#include <utilities/algorithm.h>

#define __qsort_impl(typ)                                   \
    static void __qsort_##typ(typ arr[], int low, int high) \
    {                                                       \
        if (low < high)                                     \
        {                                                   \
            typ pivot = arr[high];                          \
            int ptr = low - 1;                              \
            for (int j = low; j < high; j++)                \
            {                                               \
                if (arr[j] < pivot)                         \
                {                                           \
                    ptr++;                                  \
                    typ tmp = arr[ptr];                     \
                    arr[ptr] = arr[j];                      \
                    arr[j] = tmp;                           \
                }                                           \
            }                                               \
            ptr++;                                          \
            typ tmp = arr[ptr];                             \
            arr[ptr] = arr[high];                           \
            arr[high] = tmp;                                \
            __qsort_##typ(arr, low, ptr - 1);               \
            __qsort_##typ(arr, ptr + 1, high);              \
        }                                                   \
    }

__qsort_impl(uint8);
__qsort_impl(uint16);
__qsort_impl(uint32);
__qsort_impl(uint64);

__qsort_impl(int8);
__qsort_impl(int16);
__qsort_impl(int32);
__qsort_impl(int64);

#define qsort_impl(typ)                  \
    void qsort_##typ(typ arr[], int len) \
    {                                    \
        __qsort_##typ(arr, 0, len - 1);  \
    }

qsort_impl(uint8);
qsort_impl(uint16);
qsort_impl(uint32);
qsort_impl(uint64);

qsort_impl(int8);
qsort_impl(int16);
qsort_impl(int32);
qsort_impl(int64);
