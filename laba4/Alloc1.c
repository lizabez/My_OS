#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>

#define ALIGNMENT 8

typedef struct Header {
    size_t size;
    bool is_free;
    struct Header *next;
} Header;

typedef struct Allocator {
    void *memory;
    size_t size;
    Header *free_list;
    size_t allocated_memory;
    size_t freed_memory;
} Allocator;



Allocator* allocator_create(void *memory, size_t size) {
    if (size < sizeof(Header)) {
        printf("Error: Not enough memory to create the allocator.\n");
        return NULL;
    }

    Header *initial_block = (Header *)memory;
    initial_block->size = size - sizeof(Header);
    initial_block->is_free = true;
    initial_block->next = NULL;

    Allocator *allocator = (Allocator *)memory;
    allocator->memory = memory;
    allocator->size = size;
    allocator->free_list = initial_block;
    allocator->allocated_memory = 0;
    allocator->freed_memory = 0;

    printf("Allocator created with size: %zu bytes\n", size);
    return allocator;
}

void* allocator_malloc(Allocator *allocator, size_t size) {
    if (size == 0) {
        printf("Error: Cannot allocate 0 bytes.\n");
        return NULL;
    }

    size = (size + ALIGNMENT - 1) & ~(ALIGNMENT - 1);

    Header *current = allocator->free_list;
    Header *prev = NULL;

    while (current) {
        if (current->is_free && current->size >= size) {
            if (current->size > size + sizeof(Header)) {
                Header *new_block = (Header *)((char *)current + sizeof(Header) + size);
                new_block->size = current->size - size - sizeof(Header);
                new_block->is_free = true;
                new_block->next = current->next;

                current->size = size;
                current->next = new_block;
            }

            current->is_free = false;
            allocator->allocated_memory += size;
            printf("Allocated block of size: %zu bytes\n", size);
            return (void *)((char *)current + sizeof(Header));
        }

        prev = current;
        current = current->next;
    }

    printf("Error: Not enough memory to allocate %zu bytes.\n", size);
    return NULL;
}

void allocator_free(Allocator *allocator, void *ptr) {
    if (!ptr) {
        printf("Error: Attempt to free a null pointer.\n");
        return;
    }

    Header *block = (Header *)((char *)ptr - sizeof(Header));

    if (block->is_free) {
        printf("Error: Double free detected.\n");
        return;
    }

    block->is_free = true;
    allocator->freed_memory += block->size;
    printf("Freed block of size: %zu bytes\n", block->size);

    Header *current = allocator->free_list;
    while (current) {
        if (current->is_free && current->next && current->next->is_free) {
            current->size += sizeof(Header) + current->next->size;
            current->next = current->next->next;
            printf("Merged free blocks. New size: %zu bytes\n", current->size);
        } else {
            current = current->next;
        }
    }
}

void allocator_destroy(Allocator *allocator) {
    if (allocator) {
        printf("Allocator destroyed.\n");
    } else {
        printf("Error: Invalid allocator.\n");
    }
}

int main() {
    char memory_pool[1024];
    Allocator *allocator = allocator_create(memory_pool, sizeof(memory_pool));

    if (!allocator) {
        return 1;
    }

    void *block1 = allocator_malloc(allocator, 32);
    void *block2 = allocator_malloc(allocator, 64);
    void *block3 = allocator_malloc(allocator, 128);

    allocator_free(allocator, block1);
    allocator_free(allocator, block2);
    allocator_free(allocator, block3);

    allocator_destroy(allocator);

    return 0;
}
