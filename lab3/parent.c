#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/wait.h>

#define SHM_NAME "/shared_memory"
#define SEM_EMPTY "/sem_empty"
#define SEM_FULL "/sem_full"
#define BUFFER_SIZE 256

struct SharedData {
    char message[BUFFER_SIZE];
    int flag;
};

int main() {
    sem_t *sem_empty = sem_open(SEM_EMPTY, O_CREAT, 0666, 1);
    sem_t *sem_full = sem_open(SEM_FULL, O_CREAT, 0666, 0); 

    if (sem_empty == SEM_FAILED || sem_full == SEM_FAILED) {
        perror("sem_open failed");
        exit(EXIT_FAILURE);
    }

    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open failed");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(shm_fd, sizeof(struct SharedData)) == -1) {
        perror("ftruncate failed");
        exit(EXIT_FAILURE);
    }

    struct SharedData *shmaddr = mmap(NULL, sizeof(struct SharedData),
                                      PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shmaddr == MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }

    shmaddr->flag = 0;

    write(1, "Enter a message: ", 17); 
    ssize_t bytes_read = read(0, shmaddr->message, BUFFER_SIZE);
    if (bytes_read == -1) {
        perror("read failed");
        exit(EXIT_FAILURE);
    }

   
    if (shmaddr->message[bytes_read - 1] == '\n') {
        shmaddr->message[bytes_read - 1] = '\0';
    }

    pid_t child1 = fork();
    if (child1 == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (child1 == 0) {
        execl("./child1", "child1", SEM_EMPTY, SEM_FULL, NULL);
        perror("execl failed");
        exit(EXIT_FAILURE);
    }

    pid_t child2 = fork();
    if (child2 == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (child2 == 0) {
        execl("./child2", "child2", SEM_EMPTY, SEM_FULL, NULL);
        perror("execl failed");
        exit(EXIT_FAILURE);
    }

    sem_post(sem_empty);

    wait(NULL);
    wait(NULL);

    write(1, "Final message: ", 15);
    write(1, shmaddr->message, strlen(shmaddr->message));
    write(1, "\n", 1); 

    sem_close(sem_empty);
    sem_close(sem_full);
    sem_unlink(SEM_EMPTY);
    sem_unlink(SEM_FULL);

    munmap(shmaddr, sizeof(struct SharedData));
    shm_unlink(SHM_NAME);

    return 0;
}
