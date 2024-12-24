#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define MAX_THREADS 4  

typedef struct {
    int K;              
    int round;           
    int score1;          
    int score2;          
} GameData;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int roll_die() {
    return rand() % 6 + 1; 
}

void* experiment(void* arg) {
    GameData* gameData = (GameData*)arg;

    int player1_score = 0, player2_score = 0;
   
    for (int i = 0; i < gameData->K; i++) {
        player1_score += roll_die() + roll_die();
    }

    for (int i = 0; i < gameData->K; i++) {
        player2_score += roll_die() + roll_die();  
    }

    pthread_mutex_lock(&mutex);

 
    gameData->score1 += player1_score;
    gameData->score2 += player2_score;

    if (player1_score > player2_score) {
        gameData->round++;
    }

    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

int main() {
    int K, round, score1, score2, max_threads;

    
    printf("Enter the number of rolls per player (K): ");
    scanf("%d", &K);

    printf("Enter the current round number: ");
    scanf("%d", &round);

    printf("Enter the initial score for player 1: ");
    scanf("%d", &score1);

    printf("Enter the initial score for player 2: ");
    scanf("%d", &score2);

    printf("Enter the maximum number of threads (experiments) to run simultaneously: ");
    scanf("%d", &max_threads);

    srand(time(NULL));

    pthread_t threads[max_threads];
    GameData gameData = {K, round, score1, score2};

    for (int i = 0; i < max_threads; i++) {
        if (pthread_create(&threads[i], NULL, experiment, (void*)&gameData) != 0) {
            perror("Error creating thread");
            return 1;
        }
    }

  
    for (int i = 0; i < max_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\nTotal score of player 1: %d\n", gameData.score1);
    printf("Total score of player 2: %d\n", gameData.score2);
    if (gameData.score1 > gameData.score2) {
        printf("Player 1 wins!\n");
    } else if (gameData.score1 < gameData.score2) {
        printf("Player 2 wins!\n");
    } else {
        printf("It's a draw!\n");
    }

    return 0;
}
