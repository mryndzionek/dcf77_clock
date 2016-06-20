#include <stddef.h>

#include "sm_list.h"

void sm_list_init(sm_list_t *self)
{
    self->first = NULL;
    self->last = NULL;
}

void sm_list_insert(sm_list_t *self, sm_list_item_t *item,
		sm_list_item_t *it)
{
    item->prev = it ? it->prev : self->last;
    item->next = it;
    if(item->prev)
        item->prev->next = item;
    if(item->next)
        item->next->prev = item;
    if(!self->first || self->first == it)
        self->first = item;
    if(!it)
        self->last = item;
}

sm_list_item_t *sm_list_erase(sm_list_t *self,
		sm_list_item_t *item)
{
   sm_list_item_t *next;

    if(item->prev)
        item->prev->next = item->next;
    else
        self->first = item->next;
    if(item->next)
        item->next->prev = item->prev;
    else
        self->last = item->prev;

    next = item->next;

    item->prev = NULL;
    item->next = NULL;

    return next;
}
