#ifndef MM_H
typedef struct MEM_USED{
    char * start;
    char * end;
	struct MEM_USED * next;
} USED;

typedef struct MEM{
    char * start;
    char * end;
    int buffer_overflows;
    int free_errors;
	USED * first_used;
} MEMORY;


MEMORY * memory;
int mm_init(unsigned long size);
void error(char * message);
int mm_end();
char * mm_alloc(unsigned long no_of_chars);
int mm_free(char *ptr);
int mm_assign(char * ptr, char val);

#endif

