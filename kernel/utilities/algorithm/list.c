// list.c
#include <utilities/algorithm.h>

void list_insert(struct list_node *head, struct list_node *node)
{
    if (head == NULL || node == NULL)
        panic("list_insert - null parameters");
    node->prv = head;
    node->nxt = head->nxt;
    if (node->nxt != NULL)
        node->nxt->prv = node;
    head->nxt = node;
}

void list_detach(struct list_node *node)
{
    if (node == NULL)
        panic("list_insert - null parameters");
    if (node->prv != NULL)
        node->prv->nxt = node->nxt;
    if (node->nxt != NULL)
        node->nxt->prv = node->prv;
    node->prv = node->nxt = NULL;
}
