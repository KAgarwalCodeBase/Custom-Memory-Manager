/*
 * CS 551 Project "Memory manager".
 * You do not need to turn in this file.	
 */

#include "memory_manager.h"
#include "interposition.h"
#include <unistd.h>  // For sleep()


#include "memory_manager.h"

void test_memory_manager() {
    // Initialize the memory manager
    mem_mngr_init();

    // Test memory allocation
    void* ptr1 = malloc(1);
    void* ptr2 = malloc(2);
    void* ptr3 = malloc(8);
    void* ptr4 = malloc(7);
    void* ptr5 = malloc(3);
    void* ptr6 = malloc(2); 
    void* ptr7 = malloc(4); 
    void* ptr8 = malloc(3); 
    void* ptr9 = malloc(5); 
    void* ptr10 = malloc(6); 
    void* ptr11 = malloc(1);
    void* ptr12 = malloc(2);
    void* ptr13 = malloc(8);
    void* ptr14 = malloc(7);
    void* ptr15 = malloc(3);
    void* ptr16 = malloc(2); 
    void* ptr17 = malloc(4); 
    void* ptr18 = malloc(3);
    void* ptr19 = malloc(5); 
    void* ptr20 = malloc(2);
    void* ptr21 = malloc(3);
    void* ptr22 = malloc(4);
    void* ptr23 = malloc(8);
    void* ptr24 = malloc(7);
    void* ptr25 = malloc(3);

    void* ptr26 = malloc(10); 
    void* ptr27 = malloc(11); 
    void* ptr28 = malloc(13); 
    void* ptr29 = malloc(15); 

    void* ptr30 = malloc(26); 
    void* ptr31 = malloc(21);
    void* ptr32 = malloc(22);
    void* ptr33 = malloc(28);
    void* ptr34 = malloc(27);
    void* ptr35 = malloc(23);
    void* ptr36 = malloc(22); 
    void* ptr37 = malloc(24); 
    void* ptr38 = malloc(23); 
    void* ptr39 = malloc(25); 
    void* ptr40 = malloc(26); 
    void* ptr41 = malloc(21);
    void* ptr42 = malloc(22);
    void* ptr43 = malloc(28);
    void* ptr44 = malloc(27);
    void* ptr45 = malloc(23);
    void* ptr46 = malloc(22); 
    void* ptr47 = malloc(24); 
    void* ptr48 = malloc(23); 
    void* ptr49 = malloc(25); 
    void* ptr50 = malloc(26); 



    // Print current status of memory manager
    mem_mngr_print_snapshot();

    // Check if pointers are not NULL
    if (ptr1 == NULL || ptr2 == NULL || ptr3 == NULL || ptr4 == NULL || ptr5 == NULL || ptr6 == NULL) {
        printf("Allocation failed: One or more allocations returned NULL.\n");
        mem_mngr_leave();
        return;
    }

    // Test freeing memory
    free(ptr1); // Free second allocation
    free(ptr2); // Free first allocation
    free(ptr3); // Free third allocation
    free(ptr4); // Free fourth allocation
    free(ptr5); // Free fifth allocation
    free(ptr6); // Free sixth allocation
    free(ptr7); // Free sixth allocation
    free(ptr8); // Free sixth allocation
    free(ptr9); // Free sixth allocation
    free(ptr10); // Free sixth allocation
    free(ptr11); // Free second allocation
    free(ptr12); // Free first allocation
    free(ptr13); // Free third allocation
    free(ptr14); // Free fourth allocation
    free(ptr15); // Free fifth allocation
    free(ptr16); // Free sixth allocation
    free(ptr17); // Free sixth allocation
    free(ptr18); // Free sixth allocation
    free(ptr19); // Free sixth allocation
    
    free(ptr20); // Free sixth allocation
    free(ptr21); // Free second allocation
    free(ptr22); // Free first allocation
    free(ptr23); // Free third allocation
    free(ptr24); // Free fourth allocation
    free(ptr25); // Free fifth allocation
    free(ptr26); // Free sixth allocation
    free(ptr27); // Free sixth allocation
    free(ptr28); // Free sixth allocation
    free(ptr29); // Free sixth allocation

    free(ptr30); // Free second allocation
    free(ptr31); // Free second allocation
    free(ptr32); // Free first allocation
    free(ptr33); // Free third allocation
    free(ptr34); // Free fourth allocation
    free(ptr35); // Free fifth allocation
    free(ptr36); // Free sixth allocation
    free(ptr37); // Free sixth allocation
    free(ptr38); // Free sixth allocation
    free(ptr39); // Free sixth allocation
    free(ptr40); // Free sixth allocation
    free(ptr41); // Free second allocation
    free(ptr42); // Free first allocation
    free(ptr43); // Free third allocation
    free(ptr44); // Free fourth allocation
    free(ptr45); // Free fifth allocation
    free(ptr46); // Free sixth allocation
    free(ptr47); // Free sixth allocation
    free(ptr48); // Free sixth allocation
    free(ptr49); // Free sixth allocation
    free(ptr50); // Free sixth allocation

 
    // Print current status of memory manager after freeing
    mem_mngr_print_snapshot();

    // Test double free error
    printf("Testing double free...\n");
    free(ptr1); // This should report an error

    // Test freeing a pointer that wasn't allocated by the memory manager
    printf("Testing invalid free...\n");
    void* invalid_ptr = (void*)0xDEADBEEF; // Example of an invalid pointer
    free(invalid_ptr); // This should report an error

    // Cleanup
    mem_mngr_leave();
}

int main() {
    test_memory_manager();
    return 0;
}
