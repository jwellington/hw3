#include <stdio.h>
#include <stdlib.h>
#include "mm.h"
#include <string.h>

//Initializes mm with the specified amount of available storage.
int mm_init(unsigned long size)
{
	memory = (MEMORY*)malloc(sizeof(MEMORY));
	memory->barrier = NULL;
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

//Subroutine of mm_alloc
//Recursively search through the USED linked list for free space and create 
//new USED item there
char * search_for_free(USED * current_memory, unsigned long no_of_chars)
{
	USED * next = current_memory->next;
	if(next == NULL) //We're on the last in-use block
	{
	    //Is there between the last block and the end of the memory?
		if(memory->end - current_memory->end > no_of_chars)
		{
		    USED * new_used = (USED*)malloc(sizeof(USED));
		    new_used->start = current_memory->end;
		    new_used->end = new_used->start + no_of_chars;
		    current_memory->next = new_used;
		    new_used->next = NULL;
		    new_used->barrier_level = num_barriers();
			return new_used->start;
		}
		else
		{
			error("mm_alloc: Not enough space");
			return NULL;
		}
	}
	//Is there space between two blocks
	else if((next->start - current_memory->end) >= no_of_chars)
	{
		USED * new_used = (USED*)malloc(sizeof(USED));
		new_used->start = current_memory->end;
		new_used->end = new_used->start + no_of_chars;
		current_memory->next = new_used;
		new_used->next = next;
		new_used->barrier_level = num_barriers();
		return new_used->start;
	}
	else
	{
		return search_for_free(next, no_of_chars);
	}
}

//Allocate space for no_of_chars and return pointer to it
char * mm_alloc(unsigned long no_of_chars)
{
	//If no memory is currently allocated
	if(memory->first_used == NULL)
	{
	    //If there is enough space in the memory block
		if(memory->end - memory->start >= no_of_chars)
		{
			USED * new_used = (USED*)malloc(sizeof(USED));
			new_used->start = memory->start;
			new_used->end = new_used->start + no_of_chars;
			new_used->next = NULL;
			new_used->barrier_level = num_barriers();
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
	    //Is there space at the beginning of the memory block?
		if(memory->first_used->start - memory->start >= no_of_chars)
		{
			
			USED * new_used = (USED*)malloc(sizeof(USED));
			new_used->start = memory->start;
			new_used->end = new_used->start + no_of_chars;
			new_used->next = memory->first_used;
			new_used->barrier_level = num_barriers();
			memory->first_used = new_used;
			return new_used->start;
		}
		else //Have to search for space
		{
			return search_for_free(memory->first_used, no_of_chars);
		}
	}
	
}

//Subroutine of mm_free
//Search through USED linked list to find a start pointer
//Returns address of the block BEFORE the one to be freed
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
	//Can't free if nothing is allocated
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
		    //If ptr is the start of the first block
			memory->first_used = first_used->next;
			free(first_used);
			return 0;
		}
		else
		{
		    //Have to search for the correct block
			USED * before_free = search_for_used(memory->first_used, ptr);
			if(before_free != NULL)
			{
			    USED* to_be_freed = before_free->next;
				before_free->next = before_free->next->next;
				free(to_be_freed);
			}
			return 0;
		}
	}
	
}

//Subroutine of mm_assign
//Search to see if a pointer is in an allocated buffer
USED * search_for_buffered(USED * current_used, char * ptr)
{
    //If ptr is in current_used
	if(ptr - current_used->start >= 0 && current_used->end - ptr > 0)
	{
		return current_used;
	}
    //Not found
	else if(current_used->next == NULL)
	{
		return NULL;
	}
	//Else, check next used block
	else
	{
		return search_for_buffered(current_used->next, ptr);
	}
}

//Assign a pointer to a value if it has been properly allocated
int mm_assign(char * ptr, char val)
{
	USED * first_used = memory->first_used;
	//ptr is before all allocated blocks
	if(first_used == NULL || ptr - first_used->start < 0)
	{
		error("mm_assign: Buffer overflow");
		memory->buffer_overflows++;
		return -1;
	}
	//ptr is in first block
	else if(ptr - first_used->start >= 0 && first_used->end - ptr > 0)
	{
		*ptr = val;
		return 0;
	}
	else //Have to search for the correct block
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

//Subroutine of mm_end
//Counts the number of memory blocks remaining (memory leaks) and frees them
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

//Called by process using mm_end when it is finished
//Frees all remaining space alocated by mm and counts the number of memory
//errors
int mm_end()
{
	if(memory != NULL)
	{
		int leak_count = 0;
		if(memory->first_used != NULL)
		{
			leak_count = count_memory_leaks_and_free(1, memory->first_used);
		}
		printf("%d Memory Leaks\n", leak_count);
		printf("%d Buffer Overflows\n", memory->buffer_overflows);
		printf("%d Free Errors\n", memory->free_errors);
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

//Creates a barrier that is useful for debugging memory errors. Barriers keep
//track of memory errors that occur between the corresponding start and end
//statements similar to a limited version of mm_init and mm_end.
//Barriers are stored in a stack pointed to by the variable memory
void mm_barrier_start()
{
    BARRIER* new_barrier = (BARRIER *)malloc(sizeof(BARRIER));
    new_barrier->buffer_overflows_init = memory->buffer_overflows;
    new_barrier->free_errors_init = memory->free_errors;
    if (memory->barrier == NULL)
    {
        new_barrier->next = NULL;
        memory->barrier = new_barrier;
    }
    else
    {
        new_barrier->next = memory->barrier;
        memory->barrier = new_barrier;
    }
}

//Ends the last created barrier and prints out the memory errors that occurred
//since it was created
int mm_barrier_end()
{
    if (memory->barrier == NULL)
    {
        error("mm_barrier_end: Called before mm_barrier_start()");
        return -1;
    }
    else
    {
        //Remove the last created barrier from the stack and get its
        //information
        int barrier_level = num_barriers();
        BARRIER* cur_barrier = memory->barrier;
        memory->barrier = cur_barrier->next;
        cur_barrier->next = NULL;
        int buffer_overflows = memory->buffer_overflows - cur_barrier->buffer_overflows_init;
        int free_errors =  memory->free_errors - cur_barrier->free_errors_init;
        int memory_leaks = 0;
        USED* cur_used = memory->first_used;
        //Find remaining blocks of memory created within this barrier
        while (cur_used != NULL)
        {
            if (cur_used->barrier_level == barrier_level)
            {
                memory_leaks++;
                cur_used->barrier_level--;
            }
            cur_used = cur_used->next;
        }
        //Get rid of the old barrier
        free(cur_barrier);
        char level_str[10];
        sprintf(level_str, "%d", barrier_level-1);
        int indentlen = strlen("Barrier : ") + strlen(level_str) + 1;
        char indent[indentlen];
        int i;
        for (i = 0; i < indentlen - 1; i++)
        {
            indent[i] = ' ';
        }
        indent[indentlen-1] = '\0';
        printf("Barrier %d: %d Memory Leaks\n%s%d Free Errors\n%s%d Buffer Overflows\n",
               barrier_level-1, memory_leaks, indent, free_errors, indent,
               buffer_overflows);
        return buffer_overflows + free_errors + memory_leaks;
    }
}

//Return the current number of memory barriers in place
int num_barriers()
{
    if (memory->barrier == NULL)
    {
        return 0;
    }
    else
    {
        BARRIER* cur_barrier = memory->barrier;
        int count = 0;
        while (cur_barrier != NULL)
        {
            cur_barrier = cur_barrier->next;
            count++;
        }
        return count;
    }
}

//Print error message message
void error(char * message)
{
	fprintf(stderr, "%s", message);
	fprintf(stderr, "\n");
}
