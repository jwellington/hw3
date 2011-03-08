#include <stdio.h>
#include <stdlib.h>
#include "mm.h"


int mm_init(unsigned long size)
{
	memory = (MEMORY*)malloc(sizeof(MEMORY));
    char * start = (char*)malloc(size);
	if(start == NULL)
	{
		error("Not enough memory left");
		return -1;
	}
	else
	{
		memory->start = start;
		memory->end = start +size;
		memory->buffer_overflows = 0;
		memory->free_errors = 0;
		memory->first_used = NULL;
		return 0;
	}
}

//Recursively search through the USED linked list for free space and create new USED item there
char * search_for_free(USED * current_memory, unsigned long no_of_chars)
{
	USED * next = current_memory->next;
	if(next == NULL)
	{
		if(memory->end - current_memory->end > no_of_chars)
		{
			return current_memory->end;
		}
		else
		{
			error("mm_alloc: Not enough space");
			
			return NULL;
		}
	}
	else if((next->start - current_memory->end) >= no_of_chars)
	{
		USED * new_used = (USED*)malloc(sizeof(USED));
		new_used->start = current_memory->end;
		new_used->end = new_used->start + no_of_chars;
		current_memory->next = new_used;
		new_used->next = next;
		return new_used->start;
	}
	else
	{
		return search_for_free(next, no_of_chars);
	}
}

//Allocate space for no_of_chars and return pointer
char * mm_alloc(unsigned long no_of_chars)
{
	
	if(memory->first_used == NULL)
	{
		if(memory->end - memory->start >= no_of_chars)
		{
			
			USED * new_used = (USED*)malloc(sizeof(USED));
			new_used->start = memory->start;
			new_used->end = new_used->start + no_of_chars;
			new_used->next = NULL;
			memory->first_used = new_used;
			return new_used->start;
		}
		else
		{
			error("mm_alloc: Not enough space");
			
			return NULL;
		}
	}
	else
	{
		if(memory->first_used->start - memory->end >= no_of_chars)
		{
			
			USED * new_used = (USED*)malloc(sizeof(USED));
			new_used->start = memory->start;
			new_used->end = new_used->start + no_of_chars;
			new_used->next = memory->first_used;
			memory->first_used = new_used;
			return new_used->start;
		}
		else
		{
			return search_for_free(memory->first_used, no_of_chars);
		}
	}
	
}

//Search through USED linked list to find a start pointer
USED * search_for_used(USED * current_used, char * ptr)
{
	USED * next_used = current_used->next;
	if(next_used == NULL)
	{
	
		error("mm_free: Memory not freeable");
		memory->free_errors++;
		return NULL;
	}
	else if(next_used->start == ptr)
	{
		return current_used;
	}
	else
	{
		return search_for_used(next_used, ptr);
	}
}

//Free USED item that starts at pointer
int mm_free(char *ptr)
{
	USED * first_used = memory->first_used;
	if(first_used == NULL)
	{
		error("mm_free: Memory not freeable");
		memory->free_errors++;
		return -1;
	}
	else
	{
		if(first_used->start == ptr)
		{
		
			memory->first_used = first_used->next;
			free(first_used);
			return 0;
		}
		else
		{
			USED * before_free = search_for_used(memory->first_used, ptr);
			if(before_free != NULL)
			{
				before_free->next = before_free->next->next;
				free(before_free->next);
			}
			return 0;
		}
	}
	
}

//Search to see if a pointer is in an allocated buffer
USED * search_for_buffered(USED * current_used, char * ptr)
{
	if(ptr - current_used->start >= 0 && current_used->end - ptr > 0)
	{
		return current_used;
	}
	else if(current_used->next == NULL)
	{
		return NULL;
	}
	else
	{
		return search_for_buffered(current_used->next, ptr);
	}
}

//Assign a pointer to a value is it has been properly allocated
int mm_assign(char * ptr, char val)
{
	USED * first_used = memory->first_used;
	if(first_used == NULL || ptr - first_used->start < 0)
	{
		error("mm_assign: Buffer overflow");
		memory->buffer_overflows++;
		return -1;
	}
	else if(ptr - first_used->start >= 0 && first_used->end - ptr > 0)
	{
		*ptr = val;
		return 0;
	}
	else
	{
		if(first_used->next == NULL || search_for_buffered(first_used->next, ptr) == NULL)
		{
			error("mm_assign: Buffer overflow");
			memory->buffer_overflows++;
			return -1;
		}
		else
		{
			*ptr = val;
			return 0;
		}
	}
}
int count_memory_leaks_and_free(int count, USED * current_used)
{
	if(current_used->next != NULL)
	{
		free(current_used);
		return count_memory_leaks_and_free(count + 1, current_used->next);
	}
	else
	{
		free(current_used);
		return count;
	}
}
int mm_end()
{
	if(memory != NULL)
	{
		int leak_count = 0;
		if(memory->first_used != NULL)
		{
			leak_count = count_memory_leaks_and_free(1, memory->first_used);
		}
		printf("Memory Leaks: %d\n", leak_count);
		printf("Buffer Overflows: %d\n", memory->buffer_overflows);
		printf("Free Errors: %d\n", memory->free_errors);
		free(memory);
		memory = NULL;
		return 0;
	}
	else
	{
		error("mm_end: Called before mm_init()");
		return -1;
	}
}

void error(char * message)
{
	fprintf(stderr, message);
	fprintf(stderr, "\n");
}
