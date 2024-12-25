#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>

#define SHM_NAME "/shared_memory"
#define BUFFER_SIZE 256

struct SharedData {
    char message[BUFFER_SIZE];
    int flag;
};

int main(int argc, char *argv[]) {
    if (argc != 3) {
        write(2, "Usage: child2 <sem_empty> <sem_full>\n", 37);
        exit(EXIT_FAILURE);
    }

    sem_t *sem_empty = sem_open(argv[1], 0);
    sem_t *sem_full = sem_open(argv[2], 0);

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

    sem_wait(sem_full);

    for (int i = 0; shmaddr->message[i] != '\0'; i++) {
        if (shmaddr->message[i] == ' ') {
            shmaddr->message[i] = '_';
        }
    }

    write(1, "Child 2 processed message: ", 26);
    write(1, shmaddr->message, strlen(shmaddr->message));
    write(1, "\n", 1);

    munmap(shmaddr, sizeof(struct SharedData));
    close(shm_fd);
    return 0;
}
