#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>

#define MAX_CLASSES 5 
#define MIN_BLOCK_SIZE 16 

typedef struct Block {
    struct Block *next;
} Block;

typedef struct Allocator {
    void *memory;
    size_t size;
    Block *free_lists[MAX_CLASSES];
    size_t allocated_memory;
    size_t freed_memory;
} Allocator;




Allocator* allocator_create(void *memory, size_t size) {
    Allocator *allocator = (Allocator *)memory;
    size_t remaining_size = size - sizeof(Allocator);

    allocator->memory = (void *)((char *)memory + sizeof(Allocator));
    allocator->size = remaining_size;
    allocator->allocated_memory = 0;
    allocator->freed_memory = 0;

    for (int i = 0; i < MAX_CLASSES; i++) {
        allocator->free_lists[i] = NULL;
    }

    printf("Allocator created with size: %zu bytes\n", remaining_size);
    return allocator;
}

size_t get_class_index(size_t size) {
    size = (size + MIN_BLOCK_SIZE - 1) & ~(MIN_BLOCK_SIZE - 1);
    size_t index = 0;
    while ((1 << (index + 4)) < size && index < MAX_CLASSES - 1) {
        index++;
    }
    return index;
}

void* allocator_malloc(Allocator *allocator, size_t size) {
    if (size == 0) {
        printf("Error: Cannot allocate 0 bytes.\n");
        return NULL;
    }

    size_t class_index = get_class_index(size);
    size_t block_size = (1 << (class_index + 4));

    if (!allocator->free_lists[class_index]) {
        printf("No free blocks in class %zu. Allocating new memory chunk.\n", class_index);

        size_t num_blocks = allocator->size / block_size;
        char *base = (char *)allocator->memory;
        for (size_t i = 0; i < num_blocks; i++) {
            Block *new_block = (Block *)(base + i * block_size);
            new_block->next = allocator->free_lists[class_index];
            allocator->free_lists[class_index] = new_block;
        }
    }

    Block *block = allocator->free_lists[class_index];
    if (!block) {
        printf("Error: Not enough memory to allocate %zu bytes.\n", size);
        return NULL;
    }

    allocator->free_lists[class_index] = block->next;
    allocator->allocated_memory += block_size;

    printf("Allocated block of size: %zu bytes from class %zu\n", block_size, class_index);
    return (void *)block;
}

void allocator_free(Allocator *allocator, void *ptr) {
    if (!ptr) {
        printf("Error: Attempt to free a null pointer.\n");
        return;
    }

    Block *block = (Block *)ptr;
    size_t offset = (char *)ptr - (char *)allocator->memory;
    size_t class_index = 0;
    while (offset >= (1 << (class_index + 4)) && class_index < MAX_CLASSES - 1) {
        class_index++;
    }

    block->next = allocator->free_lists[class_index];
    allocator->free_lists[class_index] = block;

    size_t block_size = (1 << (class_index + 4));
    allocator->freed_memory += block_size;
    printf("Freed block of size: %zu bytes from class %zu\n", block_size, class_index);
}

void allocator_destroy(Allocator *allocator) {
    printf("Allocator destroyed.\n");
}

int main() {
    char memory_pool[1024];
    Allocator *allocator = allocator_create(memory_pool, sizeof(memory_pool));

    void *block1 = allocator_malloc(allocator, 32);
    void *block2 = allocator_malloc(allocator, 64);
    void *block3 = allocator_malloc(allocator, 128);

    allocator_free(allocator, block1);
    allocator_free(allocator, block2);
    allocator_free(allocator, block3);

    allocator_destroy(allocator);

    return 0;
}
