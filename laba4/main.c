#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <sys/time.h>

typedef struct Allocator {
    void *memory;
    size_t size;
    void* (*alloc)(struct Allocator *allocator, size_t size);
    void (*free)(struct Allocator *allocator, void *ptr);
    void (*destroy)(struct Allocator *allocator);
    size_t allocated_memory;
    size_t freed_memory;
} Allocator;

double get_time();

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <allocator_library_path>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *lib_path = argv[1];
    void *handle = dlopen(lib_path, RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "Error loading library: %s\n", dlerror());
        return EXIT_FAILURE;
    }

    Allocator* (*create_allocator)(void *memory, size_t size) = dlsym(handle, "create_allocator");
    void (*destroy_allocator)(Allocator *allocator) = dlsym(handle, "destroy_allocator");
    void* (*alloc_memory)(Allocator *allocator, size_t size) = dlsym(handle, "alloc_memory");
    void (*free_memory)(Allocator *allocator, void *ptr) = dlsym(handle, "free_memory");

    if (!create_allocator || !destroy_allocator || !alloc_memory || !free_memory) {
        fprintf(stderr, "Error loading functions: %s\n", dlerror());
        dlclose(handle);
        return EXIT_FAILURE;
    }

    size_t memory_size = 1024 * 1024;
    void *memory = malloc(memory_size);
    Allocator *allocator = create_allocator(memory, memory_size);

    double start, end;

    start = get_time();
    void *block1 = alloc_memory(allocator, 128);
    end = get_time();
    printf("Alloc time for 128 bytes: %.6f seconds\n", end - start);

    start = get_time();
    void *block2 = alloc_memory(allocator, 256);
    end = get_time();
    printf("Alloc time for 256 bytes: %.6f seconds\n", end - start);

    start = get_time();
    free_memory(allocator, block1);
    end = get_time();
    printf("Free time for 128 bytes: %.6f seconds\n", end - start);

    start = get_time();
    free_memory(allocator, block2);
    end = get_time();
    printf("Free time for 256 bytes: %.6f seconds\n", end - start);

    double usage_factor = (double)allocator->allocated_memory / (double)memory_size;
    printf("Memory usage factor: %.2f%%\n", usage_factor * 100);

    destroy_allocator(allocator);
    free(memory);

    dlclose(handle);

    return EXIT_SUCCESS;
}

double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}
