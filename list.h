#ifndef __LIST_H__
#define __LIST_H__

#include <stdlib.h>
#include <string.h>

#define	EMPTY_LIST				{NULL, NULL, NULL, 0}
#define NEW_PTR(type, sz)		calloc(sz, sizeof(type))
#define DEFAULT_LIST_SZ			1	//0x400

inline static void* NEW_PHEAD_PTR(int item_sz)
{
	void** p_head = calloc(1, sizeof(void*));
	*p_head = calloc(DEFAULT_LIST_SZ, item_sz);
	return p_head;
}

inline static void* ENSURE_CAPACITY(void* head, int item_sz, int old_size, int new_size, int* p_capacity)
{
	if(head && p_capacity)
	{
		while(*p_capacity < new_size){
			*p_capacity = 3 * (*p_capacity) / 2 + 1;
		}
		head = realloc(head, *p_capacity * item_sz);	
		memset((char*)head + old_size * item_sz, 0, item_sz*(*p_capacity - old_size));\
	}
	return head;			
}

inline static int* NEW_PCAPACITY()
{
	int* p_capacity = NEW_PTR(int, 1);
	*p_capacity = DEFAULT_LIST_SZ;
	return p_capacity;
}

inline static void CLEAR_LIST(void* head, int* p_capacity, int* p_size, int item_sz)
{
	if(head && p_capacity && p_size)
	{
		*p_size = 0;
		memset(head, 0, item_sz * (*p_capacity));
	}
}

#define list(type) 				struct {\
									type** p_head;\
									int* p_size;\
									int* p_capacity;\
									int item_sz;\
								}

#define new_list(type)			{NEW_PHEAD_PTR(sizeof(type)), NEW_PTR(int, 1), NEW_PCAPACITY(), sizeof(type)}

#define list_size(lst)			(*((lst).p_size))
#define list_head(lst) 			(*((lst).p_head))

#define ensure_cap(lst,sz) 	 	if(*(lst).p_capacity < sz){\
									*(lst).p_head = ENSURE_CAPACITY(*(lst).p_head, (lst).item_sz, *(lst).p_size, sz, (lst).p_capacity);\
								}

#define at_list_q(lst,i) 		(list_head(lst)[i])
#define at_list_s(lst,i) 		(*(((unsigned)(i)<list_size(lst))?(list_head(lst)+(i)) : NULL))

#ifdef __DEBUG__								
#define at_list(lst,i) 			at_list_s(lst,i)
#else
#define at_list(lst,i)			at_list_q(lst,i)
#endif


#define is_empty_list(lst)		((lst).p_size == NULL || list_size(lst) == 0)	
#define clear_list(lst)			CLEAR_LIST(*(lst).p_head, (lst).p_capacity, (lst).p_size, (lst).item_sz)
#define add_to_list(lst,item)	{\
									ensure_cap(lst, list_size(lst)+1);\
									at_list_q(lst, list_size(lst)) = item;\
									list_size(lst)++;\
								}
							  

#define plist_size(plst)		list_size(*(plst))
#define at_plist(plst,i)		at_list(*(plst), i)
#define is_empty_plist(plst)	is_empty_list(*(plst))
#define clear_plist(plst)		clear_list(*(plst))
#define add_to_plist(plst,item) add_to_list(*(plst), item)

#define free_and_clear(ptr)		{free(ptr); ptr = NULL;}

#define free_list(lst)			{\
									free(list_head(lst));\
									free_and_clear(lst.p_size);\
									free_and_clear(lst.p_capacity);\
									free_and_clear(lst.p_head);\
								}
							
#define free_list_obj(lst)	  	{\
									for(int i = 0; i < list_size(lst); i++){\
										void* tmp = at_list(lst, i);\
										if(tmp) free(tmp);\
									}\
									free_list(lst);\
								}
								
#define free_list_list(lst)	  	{\
									for(int i = 0; i < list_size(lst); i++){\
										free_list(at_list(lst, i));\
									}\
									free_list(lst);\
								}
								
#define free_list_list_obj(lst)	{\
									for(int i = 0; i < list_size(lst); i++){\
										free_list_obj(at_list(lst, i));\
									}\
									free_list(lst);\
								}								
								
typedef list(int) 				list_int;
typedef list(char) 				list_char;
typedef list(long) 				list_long;
typedef list(short) 			list_short;
typedef list(double) 			list_double;
typedef list(float) 			list_float;

#endif
