#ifndef MM_H

typedef struct MEM_USED{
    char * start;
    char * end;
    int barrier_level;
	struct MEM_USED * next;
} USED;

typedef struct MEM_BARRIER{
    int buffer_overflows_init;
    int free_errors_init;
    struct MEM_BARRIER* next;
} BARRIER;

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

