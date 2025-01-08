#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <sys/time.h>

typedef struct FreeBlock {
    size_t size;
    struct FreeBlock *next;
} FreeBlock;

typedef struct Allocator {
    void *memory;
    size_t size;
    void* (*alloc)(struct Allocator *allocator, size_t size);
    void (*free)(struct Allocator *allocator, void *ptr);
    void (*destroy)(struct Allocator *allocator);
    size_t allocated_memory;  
    size_t freed_memory; 
} Allocator;

Allocator* allocator_create(void *memory, size_t size);
void* allocator_alloc(Allocator *allocator, size_t size);
void allocator_free(Allocator *allocator, void *ptr);
void allocator_destroy(Allocator *allocator);
double get_time();

Allocator* allocator_create(void *memory, size_t size) {
    FreeBlock *free_block = (FreeBlock*)memory;
    free_block->size = size;
    free_block->next = NULL;

    Allocator *allocator = malloc(sizeof(Allocator));
    allocator->memory = memory;
    allocator->size = size;
    allocator->allocated_memory = 0;
    allocator->freed_memory = 0;
    allocator->alloc = allocator_alloc;
    allocator->free = allocator_free;
    allocator->destroy = allocator_destroy;

    return allocator;
}

void* allocator_alloc(Allocator *allocator, size_t size) {
    FreeBlock *prev = NULL;
    FreeBlock *current = (FreeBlock*)allocator->memory;

    while (current) {
        if (current->size >= size) {
            if (prev) {
                prev->next = current->next;
            } else {
                allocator->memory = current->next;
            }
            allocator->allocated_memory += size; 
            return (void*)current;
        }
        prev = current;
        current = current->next;
    }
    return NULL;
}

void allocator_free(Allocator *allocator, void *ptr) {
    FreeBlock *block = (FreeBlock*)ptr;
    allocator->freed_memory += block->size;
    block->next = (FreeBlock*)allocator->memory;
    allocator->memory = block;
}

void allocator_destroy(Allocator *allocator) {
    free(allocator);
}

double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

__attribute__((visibility("default"))) Allocator* create_allocator(void *memory, size_t size) {
    return allocator_create(memory, size);
}

__attribute__((visibility("default"))) void destroy_allocator(Allocator *allocator) {
    allocator_destroy(allocator);
}

__attribute__((visibility("default"))) void* alloc_memory(Allocator *allocator, size_t size) {
    return allocator_alloc(allocator, size);
}

__attribute__((visibility("default"))) void free_memory(Allocator *allocator, void *ptr) {
    allocator_free(allocator, ptr);
}
