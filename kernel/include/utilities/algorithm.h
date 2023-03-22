// algorithm.h
#include <types.h>

#ifndef ALGORITHM

#define ALGORITHM

// Quick Sort;

#define qsort_def(typ) \
    void qsort_##typ(typ arr[], int len)

qsort_def(uint8);
qsort_def(uint16);
qsort_def(uint32);
qsort_def(uint64);

qsort_def(int8);
qsort_def(int16);
qsort_def(int32);
qsort_def(int64);

#endif