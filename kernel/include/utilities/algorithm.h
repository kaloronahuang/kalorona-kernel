// algorithm.h
#ifndef UTILITIES_ALGORITHM

#define UTILITIES_ALGORITHM

#include <types.h>

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

// Linked List;

struct list_node
{
    struct list_node *prv;
    struct list_node *nxt;
    void *val;
};

#define list_access(node, typ) (typ *)((node).val)

void list_insert(struct list_node *head, struct list_node *node);
void list_detach(struct list_node *node);

#endif