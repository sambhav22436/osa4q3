#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdbool.h>

#define MAX_CARS 5

sem_t mutex, LSem, RSem;
int Lc = 0, Rc = 0;
bool Lcross[MAX_CARS] = {false};
bool Rcross[MAX_CARS] = {false};

void passing(int dir, int id) {
    printf("Car %d from %s is crossing the bridge.\n", id, (dir == 0) ? "Left" : "Right");
    sleep(1); 
    printf("Car %d from %s has crossed the bridge.\n", id, (dir == 0) ? "Left" : "Right");
}

void* left(void* args) {
    int id = *((int*)args);
    while (1) {
        sem_wait(&LSem);
        sem_wait(&mutex);

        if (Rc == 0 && Lc < MAX_CARS && !Lcross[id - 1]) {
            Lc++;
            passing(0, id);
            Lc--;
            Lcross[id - 1] = true;
        }

        sem_post(&mutex);
        sem_post(&LSem);
        sleep(1); 

        if (Lcross[id - 1]) {
            break;
        }
    }

    return NULL;
}

void* right(void* args) {
    int id = *((int*)args);
    while (1) {
        sem_wait(&RSem);
        sem_wait(&mutex);

        if (Lc == 0 && Rc < MAX_CARS && !Rcross[id - 1]) {
            Rc++;
            passing(1, id);
            Rc--;
            Rcross[id - 1] = true;
        }

        sem_post(&mutex);
        sem_post(&RSem);
        sleep(1); 

        if (Rcross[id - 1]) {
            break;
        }
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
    int Lid[numLeft], Rid[numRight];

    sem_init(&mutex, 0, 1);
    sem_init(&LSem, 0, MAX_CARS);
    sem_init(&RSem, 0, MAX_CARS);

    for (int i = 0; i < numLeft; ++i) {
        Lid[i] = i + 1;
        pthread_create(&leftThreads[i], NULL, left, &Lid[i]);
    }

    for (int i = 0; i < numRight; ++i) {
        Rid[i] = i + 1;
        pthread_create(&rightThreads[i], NULL, right, &Rid[i]);
    }

    for (int i = 0; i < numLeft; ++i) {
        pthread_join(leftThreads[i], NULL);
    }

    for (int i = 0; i < numRight; ++i) {
        pthread_join(rightThreads[i], NULL);
    }

    sem_destroy(&mutex);
    sem_destroy(&LSem);
    sem_destroy(&RSem);

    return 0;
}

