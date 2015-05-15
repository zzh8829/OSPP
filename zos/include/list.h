#ifndef _LIST_H_
#define _LIST_H_
 
#ifdef __compiler_offsetof
#define offset_of(T,member) __compiler_offsetof(T,member)
#else 
#define offset_of(T,member) ((size_t)&((T*)0)->member)
#endif

#define struct_of(ptr,T,member) ({ \
	const typeof( ((T*)0)->member)* __mptr = (ptr); \
	(T*)((char*)__mptr - offset_of(T,member));})

#define LIST_DATA(list,T,m) \
	struct_of(list,T,m)
	//(T *)((char *)(list) - (uint32_t)(&(((T*)0)->m)))

typedef struct _list
{
	struct _list* next;
	struct _list* prev;
} list_t;

#define LIST_FOR_EACH(it, list, m) \
	for (it = LIST_DATA((list).next, typeof(*it), m); \
		&it->m != &(list); \
		it = LIST_DATA(it->m.next, typeof(*it), m))

#define LIST_FOR_EACH_SAFE(it, n, list, m) \
	for (it = LIST_DATA((list).next, typeof(*it), m), \
		n = LIST_DATA(it->m.next, typeof(*it), m); \
		&it->m != &(list); \
		it = n, \
		n = LIST_DATA(n->m.next, typeof(*it), m))

#define LIST_INIT(list) list_init(&(list))
#define LIST_PUSH(list,node) list_push_back(&(list),&(node))
#define LIST_PUSH_FRONT(list,node) list_push_front(&(list),&(node))
#define LIST_DELETE(node) list_delete(&(node))
#define LIST_MOVE(list,node) \
	LIST_DELETE(node) \
	list_add_after(&(list),&(node))
#define LIST_MOVE_FRONT(list,node) \
	LIST_DELETE(node) \
	list_add_before(&(list),&(node))

static inline void list_init(list_t* list)
{
	list->next = list;
	list->prev = list;
}

static inline void list_delete(list_t* node)
{
	node->prev->next = node->next;
	node->next->prev = node->prev;
}

static inline void list_add(list_t* prev, list_t* node, list_t* next)
{
	next->prev = node;
	node->next = next;
	node->prev = prev;
	prev->next = node;
}

static inline void list_add_after(list_t* list, list_t* node)
{
	list_add(list,node,list->next);
}

static inline void list_add_before(list_t* list, list_t* node)
{
	list_add(list->prev,node,list);
}

static inline void list_push_back(list_t* head,list_t* node)
{
	list_add_before(head,node);	
}

static inline void list_push_front(list_t* head,list_t* node)
{
	list_add_after(head,node);
}

 
#endif