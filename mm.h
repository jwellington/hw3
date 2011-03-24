/* mm.c: A custom memory-management system. See README for more.
 */

#ifndef MM_H

//Data structure representing an allocated block of memory. Keeps pointers to
//its start and endpoints, the next allocated block, and the barrier level it
//was created in.
//Used blocks are stored in a linked list in order of starting address.
typedef struct MEM_USED{
    char * start;
    char * end;
    int barrier_level;
	struct MEM_USED * next;
} USED;

//Data structure representing a memory barrier. Records the number of buffer
//overflows and free errors when it was initialized (so they can be compared
//with the values when it ends) and a pointer to the next barrier.
//Barriers are stored in a stack (first barrier is the last created).
typedef struct MEM_BARRIER{
    int buffer_overflows_init;
    int free_errors_init;
    struct MEM_BARRIER* next;
} BARRIER;

//Data structure representing a block of memory requested by mm_init.
//Contains pointers to its start and endpoints, the first allocated block,
//and a list of memory barriers currently in place. Also tracks buffer
//overflows and free errors.
typedef struct MEM{
    char * start;
    char * end;
    int buffer_overflows;
    int free_errors;
	USED * first_used;
	BARRIER* barrier;
} MEMORY;


MEMORY * memory;
int mm_init(unsigned long size);
void error(char * message);
int mm_end();
char * mm_alloc(unsigned long no_of_chars);
int mm_free(char *ptr);
int mm_assign(char * ptr, char val);
void mm_barrier_start();
int mm_barrier_end();
int num_barriers();

#endif

