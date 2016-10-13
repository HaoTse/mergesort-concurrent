#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mergesort.h"

llist_t *merge_list(llist_t *a, llist_t *b)
{
    llist_t *_list = list_new();
    node_t *current = NULL;
    while (a->size && b->size) {
        llist_t *small = (llist_t *)
                         ((intptr_t) a * (strncmp(a->head->data ,b->head->data, sizeof(val_t)) <= 0) +
                          (intptr_t) b * (strncmp(a->head->data ,b->head->data, sizeof(val_t)) > 0));
        if (current) {
            current->next = small->head;
            current = current->next;
        } else {
            _list->head = small->head;
            current = _list->head;
        }
        small->head = small->head->next;
        --small->size;
        ++_list->size;
        current->next = NULL;
    }

    llist_t *remaining = (llist_t *) ((intptr_t) a * (a->size > 0) +
                                      (intptr_t) b * (b->size > 0));
    if (current) current->next = remaining->head;
    _list->size += remaining->size;
    free(a);
    free(b);
    return _list;
}
