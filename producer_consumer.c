#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define LOWER_NUM 1
#define UPPER_NUM 10000
#define BUFFER_SIZE 100
#define MAX_COUNT 10000

int buffer[BUFFER_SIZE];
int buffer_index = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int producer_finished = 0;

void *producer(void *arg) {
    FILE *file = fopen("all.txt", "w");
    for (int i = 0; i < MAX_COUNT; i++) {
        int number = rand() % (UPPER_NUM - LOWER_NUM + 1) + LOWER_NUM;
        while(buffer_index >= BUFFER_SIZE);

        pthread_mutex_lock(&lock);

        buffer[buffer_index++] = number;

        if (file != NULL) {
            fprintf(file, "%d\n", number);  
        }
        
        pthread_mutex_unlock(&lock);
    }

    fclose(file);
    producer_finished = 1;
}



void *customer(void *arg) {
    int parity = *((int *)arg);
    char filename[10];

    sprintf(filename, "%s.txt", parity ? "odd" : "even");
    FILE *file = fopen(filename, "w");

    while (1) {
        pthread_mutex_lock(&lock);

        if (!buffer_index && producer_finished) {
            pthread_mutex_unlock(&lock);
            break;
        }

        if (buffer_index > 0) {
            int number = buffer[buffer_index-1];
            if ((number&1) == parity) {
                buffer_index--;
                if (file != NULL) {
                    fprintf(file, "%d\n", number);
                }
            }
        }
        pthread_mutex_unlock(&lock);
    }

    fclose(file);
}



int main() {
    pthread_t prod_tid, cust1_tid, cust2_tid;
    int cust_odd_parity = 0;
    int cust_even_parity = 1;

    pthread_create(&prod_tid, NULL, producer, NULL);
    pthread_create(&cust1_tid, NULL, customer, &cust_odd_parity);
    pthread_create(&cust2_tid, NULL, customer, &cust_even_parity);

    pthread_join(prod_tid, NULL);
    pthread_join(cust1_tid, NULL);
    pthread_join(cust2_tid, NULL);

    printf("All threads have finished.\n");

    return 0;

}