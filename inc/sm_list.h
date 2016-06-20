#ifndef INC_SM_LIST_H_
#define INC_SM_LIST_H_


typedef struct sm_list_item sm_list_item_t;

/* Doubly-linked list. */

struct sm_list_item {
	sm_list_item_t *next;
	sm_list_item_t *prev;
};

typedef struct {
	sm_list_item_t *first;
	sm_list_item_t *last;
} sm_list_t;

void sm_list_init(sm_list_t *self);
#define sm_list_empty(self) (!((self)->first))
#define sm_list_begin(self) ((self)->first)
#define sm_list_next(it) ((it)->next)

void sm_list_insert(sm_list_t *self, sm_list_item_t *item,
		sm_list_item_t *it);

sm_list_item_t *sm_list_erase(sm_list_t *self,
		sm_list_item_t *item);

#endif /* INC_SM_LIST_H_ */
