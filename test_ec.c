#include "mm.h"

int main()
{
     char *ptr1, *ptr2, *ptr3, *ptr4, *ptr5, *ptr6;
     int i;

     // initialize the memory library
     mm_init(4000); // memory pool size = 4000 bytes

     ptr1=mm_alloc(10);
     
     //Start barrier 0
     mm_barrier_start();
     for (i=0; i<11;i++) {
        // Should return buffer overflow error on 11th iteration
        mm_assign(ptr1+i,(char)i);
    }

     //Start barrier 1
     mm_barrier_start();
     ptr2=mm_alloc(1);
     mm_assign(ptr2+1, (char)2); //Buffer overflow
     mm_free(ptr2);
     mm_free(ptr2); // Double free
     ptr6 = mm_alloc(1); //Will be a memory leak

     //End barrier 1: should catch mem bugs from start of barrier 1
     //Should find 1 buffer overflow, 1 free error and 1 memory leak
     mm_barrier_end();
     
     ptr3=mm_alloc(1); //Will be a memory leak
     mm_assign(ptr3, (char)3);

     // End barrier 0: should catch mem bugs from start of barrier 0
     //Should find find 2 buffer overflows, 2 memory leaks, 1 free error
     mm_barrier_end();
     
     mm_barrier_start();
     
     mm_assign(ptr1 - 1, (char)4); //Buffer overflow
     ptr4 = mm_alloc(1); //Will be a memory leak
     mm_free(ptr5); //Free error
     ptr5 = mm_alloc(10);
     mm_assign(ptr4+5, (char)5); //Should not detect this buffer overflow
     mm_free(ptr5);
     
     mm_barrier_end(); //Should catch 1 buffer overflow, 1 memory leak,
                       //1 free error
     mm_end(); //ptr1, ptr3, ptr4, and ptr6 are memory leaks
     //Three buffer overflows, two free errors
}
