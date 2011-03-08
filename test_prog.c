#include "mm.h"
#include <stdio.h>
#include <stdlib.h>
int main(){
        char *ptr1, *ptr2;
        int i, num_leaks;

        // initialize the memory library
        mm_init(4000); // memory pool size = 4000 bytes

        // Allocate 10 bytes
        ptr1=mm_alloc(10);

        for (i=0; i<11;i++) {
                // Should return buffer overflow error on 11th iteration
                mm_assign(ptr1+i,(char)i); // It is similar to ptr1[i] = (char)i
        }
        ptr2=ptr1+5;

        // Should return error since ptr2 is not the original ptr
        // returned by earlier mm_alloc.
        
        mm_free(ptr2);
        
        mm_free(ptr2); // free error again
		
        //End allocation barrier: should catch memory leak since
        //ptr1 was allocated but not freed.
        mm_end();
}

