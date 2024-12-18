#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

typedef struct {
    int K;
    int currentTurn;
    int player1_total;
    int player2_total;
    int experiments;
    int *results;
} GameParams;

int roll_dice(unsigned int *seed) {
    return (rand_r(seed) % 6 + 1) + (rand_r(seed) % 6 + 1);
}

int simulate_game(int K, unsigned int *seed) {
    int player1_score = 0;
    int player2_score = 0;

    for (int i = 0; i < K; i++) {
        player1_score += roll_dice(seed);
        player2_score += roll_dice(seed);
    }

    return player1_score > player2_score ? 1 : (player1_score < player2_score ? 2 : 0);
}

void *simulate_games(void *arg) {
    GameParams *params = (GameParams *)arg;
    unsigned int seed = (unsigned int)time(NULL) ^ pthread_self();

    for (int i = 0; i < params->experiments; i++) {
        params->results[i] = simulate_game(params->K, &seed);
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 7) {
        printf("Usage: %s <K> <current turn> <player1 total score> <player2 total score> <number of expe riments> <number of threads>\n", argv[0]);
        return 1;
    }

    int K = atoi(argv[1]);
    int currentTurn = atoi(argv[2]);
    int player1_total = atoi(argv[3]);
    int player2_total = atoi(argv[4]);
    int experiments = atoi(argv[5]);
    int num_threads = atoi(argv[6]);

    if (K <= 0 || currentTurn < 0 || player1_total < 0 || player2_total < 0 || experiments <= 0 || num_threads <= 0) {
        printf("Invalid input data. All parameters must be positive integers.\n");
        return 1;
    }

    int experiments_per_thread = experiments / num_threads;
    int remaining_experiments = experiments % num_threads;

    pthread_t threads[num_threads];
    GameParams params[num_threads];
    int *results = (int *)malloc(experiments * sizeof(int));

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < num_threads; i++) {
        params[i].K = K;
        params[i].currentTurn = currentTurn;
        params[i].player1_total = player1_total;
        params[i].player2_total = player2_total;
        params[i].experiments = experiments_per_thread + (i < remaining_experiments ? 1 : 0);
        params[i].results = results + (i * experiments_per_thread + (i < remaining_experiments ? i : remaining_experiments));
        pthread_create(&threads[i], NULL, simulate_games, &params[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    int player1_wins = 0;
    int player2_wins = 0;
    int draws = 0;

    for (int i = 0; i < experiments; i++) {
        if (results[i] == 1) {
            player1_wins++;
        } else if (results[i] == 2) {
            player2_wins++;
        } else {
            draws++;
        }
    }

    free(results);

    printf("Chances of player 1 winning: %.2f%%\n", (double)player1_wins / experiments * 100);
    printf("Chances of player 2 winning: %.2f%%\n", (double)player2_wins / experiments * 100);
    printf("Chances of a draw: %.2f%%\n", (double)draws / experiments * 100);
    printf("Time taken: %.6f seconds\n", time_taken);

    return 0;
}