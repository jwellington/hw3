#include "mm.h"

int main()
{
     char *ptr1, *ptr2, *ptr3;
     int i;

     // initialize the memory library
     mm_init(4000); // memory pool size = 4000 bytes

     ptr1=mm_alloc(10);
     //Start barrier 0
     mm_barrier_start(); //The first start
     for (i=0; i<11;i++) {
        // Should return buffer overflow error on 11th iteration
        mm_assign(ptr1+i,(char)i);
        // It is similar to ptr1[i] = (char)i
    }

     //Start barrier 1
     mm_barrier_start();//The second start
     ptr2=mm_alloc(1);
     mm_assign(ptr2+1, (char)2); //Buffer overflow error
     mm_free(ptr2);

     //End barrier 1: should catch mem bugs from start of barrier 1
     //Should find one buffer overflow
     mm_barrier_end();//The first end should match the second start
     mm_free (ptr2); // Double free
     ptr3=mm_alloc(1);
     mm_assign(ptr3, (char)3);

     // End barrier 0: should catch mem bugs from start of barrier 0
     //Should find find 2 buffer overflows, 1 memory leak, 1 free error
     mm_barrier_end();//The second end should match the first start
     mm_end(); //ptr1 is a memory leak
}
