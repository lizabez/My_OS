#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>
#include <ctype.h>

#define SHM_NAME "/shared_memory"
#define SEM_EMPTY "/sem_empty"
#define SEM_FULL "/sem_full"
#define BUFFER_SIZE 256

struct SharedData {
    char message[BUFFER_SIZE];
    int flag;
};

int main() {
    sem_t *sem_empty = sem_open(SEM_EMPTY, 0);
    sem_t *sem_full = sem_open(SEM_FULL, 0);

    if (sem_empty == SEM_FAILED || sem_full == SEM_FAILED) {
        perror("sem_open failed");
        exit(EXIT_FAILURE);
    }

    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open failed");
        exit(EXIT_FAILURE);
    }

    struct SharedData *shmaddr = mmap(NULL, sizeof(struct SharedData),
                                      PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shmaddr == MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }

    sem_wait(sem_empty);

    for (int i = 0; shmaddr->message[i] != '\0'; i++) {
        shmaddr->message[i] = toupper(shmaddr->message[i]);
    }

    shmaddr->flag = 1;
    sem_post(sem_full);
    munmap(shmaddr, sizeof(struct SharedData));

    return 0;
}
