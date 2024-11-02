#include "memory_manager.h"

static STRU_MEM_LIST *mem_pool = NULL; // Global memory list

/*
 * Print out the current status of the memory manager.
 * Reading this function may help you understand how the memory manager organizes the memory.
 * Do not change the implementation of this function. It will be used to help the grading.
 */
void mem_mngr_print_snapshot(void) {
    STRU_MEM_LIST *mem_list = mem_pool; // Get the first memory list

    printf("============== Memory snapshot ===============\n");

    while (mem_list != NULL) {
        STRU_MEM_BATCH *mem_batch = mem_list->first_batch; // Get the first mem batch from the list 

        printf("mem_list %p slot_size %d batch_count %d free_slot_bitmap %p\n", 
               mem_list, mem_list->slot_size, mem_list->batch_count, mem_list->free_slots_bitmap);
        bitmap_print_bitmap(mem_list->free_slots_bitmap, (mem_list->bitmap_size));

        while (mem_batch != NULL) {
            printf("\t mem_batch %p batch_mem %p\n", mem_batch, mem_batch->batch_mem);
            mem_batch = mem_batch->next_batch; // Get next mem batch
        }

        mem_list = mem_list->next_list; // Move to the next memory list
    }

    printf("==============================================\n");
}

/*
 * Initialize the memory manager with 16 bytes(defined by the macro MEM_ALIGNMENT_BOUNDARY) slot size mem_list.
 * Initialize this list with 1 batch of slots.
 */
void mem_mngr_init(void) {
    mem_pool = (STRU_MEM_LIST *)malloc(sizeof(STRU_MEM_LIST));
    if (!mem_pool) {
        printf("ERROR: Memory initialization failed\n");
        exit(1);
    }

    mem_pool->slot_size = MEM_ALIGNMENT_BOUNDARY; // Set the size of each slot
    mem_pool->batch_count = 1; // Initialize batch count
    mem_pool->first_batch = (STRU_MEM_BATCH *)malloc(sizeof(STRU_MEM_BATCH));

    if (!mem_pool->first_batch) {
        perror("ERROR: Memory allocation for first batch failed\n");
        exit(1);
    }

    // Allocate memory for the batch
    mem_pool->first_batch->batch_mem = malloc(MEM_ALIGNMENT_BOUNDARY * MEM_BATCH_SLOT_COUNT);
    if (!mem_pool->first_batch->batch_mem) {
        printf("ERROR: Memory allocation for batch failed\n");
        exit(1);
    }

    // Calculate the bitmap size
    size_t bitmap_size_bytes = (MEM_BATCH_SLOT_COUNT + 7) / 8; // Round up to the nearest byte
    mem_pool->free_slots_bitmap = (unsigned char *)malloc(bitmap_size_bytes);
    if (!mem_pool->free_slots_bitmap) {
        printf("ERROR: Memory allocation for bitmap failed\n");
        exit(1);
    }

    // Set all bits to 1 (all slots are free).
    memset(mem_pool->free_slots_bitmap, 0xFF, bitmap_size_bytes);
    mem_pool->bitmap_size = bitmap_size_bytes;
    mem_pool->first_batch->next_batch = NULL; // Set the next batch to NULL
    mem_pool->next_list = NULL; // Initialize the next memory list to NULL
}

/*
 * Allocate a chunk of memory 	
 * @param size: size in bytes to be allocated
 * @return: the pointer to the allocated memory slot
 */
void *mem_mngr_alloc(size_t size) {
    if (size > MEM_ALIGNMENT_BOUNDARY * 5) {
        printf("ERROR: Requested size exceeds the maximum limit\n");
        return NULL;
    }

    STRU_MEM_LIST *mem_list = mem_pool;
    // Traverse the list to find a suitable slot.
    while (mem_list) {
        if (size <= (size_t)mem_list->slot_size) {
            // Search for a free slot.
            int slot_pos = bitmap_find_first_bit(mem_list->free_slots_bitmap, mem_list->bitmap_size, 1);
            if (slot_pos != BITMAP_OP_NOT_FOUND) {
                bitmap_clear_bit(mem_list->free_slots_bitmap, mem_list->bitmap_size, slot_pos);
                int batch_index = slot_pos / MEM_BATCH_SLOT_COUNT;
                STRU_MEM_BATCH *batch_iter = mem_list->first_batch;
                int batch_count = 0;
                while(batch_iter->next_batch && batch_count<batch_index){
                    batch_count +=1;
                    batch_iter = batch_iter->next_batch;
                }
                int slot_rel_index = slot_pos -  batch_index * MEM_BATCH_SLOT_COUNT;
                return batch_iter->batch_mem + slot_rel_index * mem_list->slot_size;
            }else {
                STRU_MEM_BATCH *new_batch = (STRU_MEM_BATCH *)malloc(sizeof(STRU_MEM_BATCH));
                if (!new_batch) {
                    printf("ERROR: Memory allocation for new batch failed\n");
                    return NULL;
                }
                new_batch->batch_mem = malloc(mem_list->slot_size * MEM_BATCH_SLOT_COUNT);
                if (!new_batch->batch_mem) {
                    printf("ERROR: Memory allocation for batch memory failed\n");
                    free(new_batch);
                    return NULL;
                }

                // Find the last batch in the list and link the new batch.
                STRU_MEM_BATCH *temp_mem_batch = mem_list->first_batch;
                while (temp_mem_batch->next_batch != NULL) {
                    temp_mem_batch = temp_mem_batch->next_batch;
                }
                temp_mem_batch->next_batch = new_batch;
                new_batch->next_batch = NULL;  // Ensure the new batch is the last one.
                mem_list->batch_count++;
                // Reallocate the bitmap to accommodate the new batch.
                size_t new_bitmap_size = (MEM_BATCH_SLOT_COUNT * mem_list->batch_count + 7) / 8;
                unsigned char *new_bitmap = (unsigned char *)malloc(new_bitmap_size);
                if (!new_bitmap) {
                    printf("ERROR: Bitmap reallocation failed\n");
                    free(new_batch->batch_mem);
                    free(new_batch);
                    return NULL;
                }

                // **Copy existing bitmap entries to the front**, keeping the new batch's bits at the back.
                size_t old_bitmap_size = mem_list->bitmap_size;
                memcpy(new_bitmap, mem_list->free_slots_bitmap, old_bitmap_size);
                memset(new_bitmap + old_bitmap_size, 0xFF, new_bitmap_size - old_bitmap_size);  // Mark new slots as free.

                // Free the old bitmap and update the memory list.
                free(mem_list->free_slots_bitmap);
                mem_list->free_slots_bitmap = new_bitmap;
                mem_list->bitmap_size = new_bitmap_size;

                // Mark the first slot of the new batch as used.
                bitmap_clear_bit(mem_list->free_slots_bitmap, mem_list->bitmap_size, 
                                (mem_list->batch_count - 1) * MEM_BATCH_SLOT_COUNT);

                // Return the allocated memory address from the new batch.
                return new_batch->batch_mem;
            }
        }
        mem_list = mem_list->next_list; // Move to the next memory list
    }

    // Calculate the new slot size aligned to MEM_ALIGNMENT_BOUNDARY.
    size_t new_slot_size = ((size + MEM_ALIGNMENT_BOUNDARY - 1) / MEM_ALIGNMENT_BOUNDARY) * MEM_ALIGNMENT_BOUNDARY;

    // Create a new batch.
    STRU_MEM_BATCH *new_batch = (STRU_MEM_BATCH *)malloc(sizeof(STRU_MEM_BATCH));
    if (!new_batch) {
        printf("ERROR: Memory allocation for new batch failed\n");
        return NULL;
    }

    // Allocate memory for the batch
    new_batch->batch_mem = malloc(new_slot_size * MEM_BATCH_SLOT_COUNT);
    if (!new_batch->batch_mem) {
        printf("ERROR: Memory allocation for batch memory failed\n");
        free(new_batch);
        return NULL;
    }

    // Attach the new batch to the end of the batch list.
    STRU_MEM_LIST *last_mem_list = mem_pool;
    while (last_mem_list->next_list) {
        last_mem_list = last_mem_list->next_list; // Traverse to the end.
    }

    last_mem_list->next_list = (STRU_MEM_LIST *)malloc(sizeof(STRU_MEM_LIST));
    last_mem_list = last_mem_list->next_list;
    
    last_mem_list->first_batch = new_batch;
    last_mem_list->batch_count = 1;
    last_mem_list->slot_size = new_slot_size;
    last_mem_list->next_list = NULL;
    size_t new_bitmap_size = (MEM_BATCH_SLOT_COUNT + 7) / 8;
    last_mem_list->free_slots_bitmap = (unsigned char *)malloc(new_bitmap_size);
    if (!last_mem_list->free_slots_bitmap) {
        printf("ERROR: Memory allocation for bitmap failed\n");
        free(last_mem_list->first_batch->batch_mem);
        free(new_batch);
        return NULL;
    }
    last_mem_list->bitmap_size = new_bitmap_size;
    // Initialize the bitmap: mark all slots as free.
    memset(last_mem_list->free_slots_bitmap, 0xFF, new_bitmap_size);

    // Now, mark the first slot of the new batch as used.
    bitmap_clear_bit(last_mem_list->free_slots_bitmap, new_bitmap_size, 0);

    // Return the allocated memory address.
    return last_mem_list->first_batch->batch_mem;
}

/*
 * Free a chunk of memory pointed by ptr
 * Print out any error messages
 * @param: the pointer to the allocated memory slot
 */
void mem_mngr_free(void *ptr) {
    if (!ptr) {
        printf("ERROR: Cannot free a NULL pointer\n");
        return;
    }

    STRU_MEM_LIST *mem_list = mem_pool;
    while (mem_list) {
        STRU_MEM_BATCH *batch = mem_list->first_batch;
        int batch_count = 0;
        while (batch) {
            // Check if ptr is within the batch's memory range
            if ((unsigned char *)ptr >= (unsigned char *)batch->batch_mem &&
                (unsigned char *)ptr < (unsigned char *)batch->batch_mem + mem_list->slot_size * MEM_BATCH_SLOT_COUNT) {

                // Calculate the slot index
                size_t slot_index = (((unsigned char *)ptr - (unsigned char *)batch->batch_mem) / mem_list->slot_size) ;

                int abs_slot_pos = slot_index+ batch_count * MEM_BATCH_SLOT_COUNT;
                if (bitmap_bit_is_set(mem_list->free_slots_bitmap, mem_list->bitmap_size, abs_slot_pos)) {
                    printf("ERROR: Double free detected\n");
                    return;
                }

                bitmap_set_bit(mem_list->free_slots_bitmap, mem_list->bitmap_size, abs_slot_pos);
                return;
            }
            batch_count += 1;
            batch = batch->next_batch;  // Move to the next batch
        }
        mem_list = mem_list->next_list;  // Move to the next memory list
    }
    printf("ERROR: Pointer does not belong to any managed memory\n");
}

/*
 * Clean up the memory manager (e.g., release all the memory allocated)
*/
void mem_mngr_leave(void) {
    STRU_MEM_LIST *mem_list = mem_pool;
    while (mem_list) {
        STRU_MEM_BATCH *batch = mem_list->first_batch;
        while (batch) {
            free(batch->batch_mem); // Free the batch memory
            STRU_MEM_BATCH *next_batch = batch->next_batch;
            free(batch); // Free the batch structure
            batch = next_batch;
        }
        STRU_MEM_LIST *next_list = mem_list->next_list;
        free(mem_list->free_slots_bitmap); // Free the bitmap
        free(mem_list); // Free the memory list structure
        mem_list = next_list;
    }
    mem_pool = NULL; // Clear the global memory pool
}
