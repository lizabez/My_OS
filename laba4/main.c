#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <sys/time.h>


typedef struct FreeBlock {
    size_t size;           
    struct FreeBlock *next; 
} FreeBlock;


typedef struct Allocator {
    void *memory;
    size_t size;
    FreeBlock *free_list; 
    size_t allocated_memory;
    size_t freed_memory;
} Allocator;





Allocator* allocator_create(void *memory, size_t size) {
    if (size < sizeof(FreeBlock)) {
        fprintf(stderr, "Error: Not enough memory to create allocator.\n");
        return NULL;
    }

    FreeBlock *initial_block = (FreeBlock*)memory;
    initial_block->size = size - sizeof(FreeBlock);
    initial_block->next = NULL;

    Allocator *allocator = (Allocator*)malloc(sizeof(Allocator));
    if (!allocator) {
        fprintf(stderr, "Error: Failed to allocate memory for allocator structure.\n");
        return NULL;
    }

    allocator->memory = memory;
    allocator->size = size;
    allocator->free_list = initial_block;
    allocator->allocated_memory = 0;
    allocator->freed_memory = 0;

    printf("Allocator created with size: %zu bytes\n", size);
    return allocator;
}


void* allocator_alloc(Allocator *allocator, size_t size) {
    if (size == 0) {
        fprintf(stderr, "Error: Cannot allocate 0 bytes.\n");
        return NULL;
    }
    size = (size + sizeof(void*) - 1) & ~(sizeof(void*) - 1);

    FreeBlock *prev = NULL;
    FreeBlock *current = allocator->free_list;

    while (current) {
        if (current->size >= size) {
            if (current->size > size + sizeof(FreeBlock)) {
                FreeBlock *new_block = (FreeBlock*)((char*)current + sizeof(FreeBlock) + size);
                new_block->size = current->size - size - sizeof(FreeBlock);
                new_block->next = current->next;

                current->size = size;
                if (prev) {
                    prev->next = new_block;
                } else {
                    allocator->free_list = new_block;
                }
            } else {
                if (prev) {
                    prev->next = current->next;
                } else {
                    allocator->free_list = current->next;
                }
            }

            allocator->allocated_memory += size;
            printf("Allocated block of size: %zu bytes\n", size);
            return (void*)((char*)current + sizeof(FreeBlock));
        }

        prev = current;
        current = current->next;
    }

    fprintf(stderr, "Error: Not enough memory to allocate %zu bytes.\n", size);
    return NULL;
}


void allocator_free(Allocator *allocator, void *ptr) {
    if (!ptr) {
        fprintf(stderr, "Error: Cannot free a NULL pointer.\n");
        return;
    }

    FreeBlock *block = (FreeBlock*)((char*)ptr - sizeof(FreeBlock));
    allocator->freed_memory += block->size;

    block->next = allocator->free_list;
    allocator->free_list = block;

    printf("Freed block of size: %zu bytes\n", block->size);
}


void allocator_destroy(Allocator *allocator) {
    if (allocator) {
        free(allocator);
        printf("Allocator destroyed.\n");
    } else {
        fprintf(stderr, "Error: Invalid allocator.\n");
    }
}

double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}


int main() {
    char memory_pool[1024];
    Allocator *allocator = allocator_create(memory_pool, sizeof(memory_pool));

    if (!allocator) {
        return EXIT_FAILURE;
    }

    double start_time, end_time;

    start_time = get_time();
    void *block1 = allocator_alloc(allocator, 128);
    void *block2 = allocator_alloc(allocator, 256);
    end_time = get_time();
    printf("Allocation time: %.6f seconds\n", end_time - start_time);

    start_time = get_time();
    allocator_free(allocator, block1);
    allocator_free(allocator, block2);
    end_time = get_time();
    printf("Free time: %.6f seconds\n", end_time - start_time);

    double usage_factor = (double)allocator->allocated_memory / allocator->size * 100;
    printf("Memory usage factor: %.2f%%\n", usage_factor);

    allocator_destroy(allocator);

    return EXIT_SUCCESS;
}
