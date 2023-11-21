#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_CARS 5

sem_t mutex, leftSem, rightSem;
int leftCount = 0, rightCount = 0;
int leftCrossed = 0, rightCrossed = 0;

void passing(int dir, int id) {
    printf("Car %d from %s is crossing the bridge.\n", id, (dir == 0) ? "Left" : "Right");
    sleep(1); // Simulating the time it takes to cross the bridge
    printf("Car %d from %s has crossed the bridge.\n", id, (dir == 0) ? "Left" : "Right");
}

void* left(void* args) {
    int id = *((int*)args);
    while (1) {
        sem_wait(&leftSem);
        sem_wait(&mutex);

        if (rightCount == 0 && leftCount < MAX_CARS) {
            leftCount++;
            passing(0, id);
            leftCount--;
            leftCrossed++;

            if (leftCrossed == MAX_CARS) {
                sem_post(&mutex);
                sem_post(&leftSem);
                break;
            }
        }

        sem_post(&mutex);
        sem_post(&leftSem);
        sleep(1); // Sleep to avoid immediate re-entry
    }

    return NULL;
}

void* right(void* args) {
    int id = *((int*)args);
    while (1) {
        sem_wait(&rightSem);
        sem_wait(&mutex);

        if (leftCount == 0 && rightCount < MAX_CARS) {
            rightCount++;
            passing(1, id);
            rightCount--;
            rightCrossed++;

            if (rightCrossed == MAX_CARS) {
                sem_post(&mutex);
                sem_post(&rightSem);
                break;
            }
        }

        sem_post(&mutex);
        sem_post(&rightSem);
        sleep(1); // Sleep to avoid immediate re-entry
    }

    return NULL;
}

int main() {
    int numLeft, numRight;

    printf("Enter the number of cars on the left side: ");
    scanf("%d", &numLeft);

    printf("Enter the number of cars on the right side: ");
    scanf("%d", &numRight);

    pthread_t leftThreads[numLeft], rightThreads[numRight];
    int leftIds[numLeft], rightIds[numRight];

    sem_init(&mutex, 0, 1);
    sem_init(&leftSem, 0, MAX_CARS);
    sem_init(&rightSem, 0, MAX_CARS);

    for (int i = 0; i < numLeft; ++i) {
        leftIds[i] = i + 1;
        pthread_create(&leftThreads[i], NULL, left, &leftIds[i]);
    }

    for (int i = 0; i < numRight; ++i) {
        rightIds[i] = i + 1;
        pthread_create(&rightThreads[i], NULL, right, &rightIds[i]);
    }

    for (int i = 0; i < numLeft; ++i) {
        pthread_join(leftThreads[i], NULL);
    }

    for (int i = 0; i < numRight; ++i) {
        pthread_join(rightThreads[i], NULL);
    }

    sem_destroy(&mutex);
    sem_destroy(&leftSem);
    sem_destroy(&rightSem);

    return 0;
}
