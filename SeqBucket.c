#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void bucketSort(int arr[], int n) {
    int max = arr[0];
    for (int i = 1; i < n; i++) {
        if (arr[i] > max) {
            max = arr[i];
        }
    }

    int *buckets = (int *)malloc((max + 1) * sizeof(int));
    if (buckets == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }


    for (int i = 0; i <= max; i++) {
        buckets[i] = 0;
    }

    for (int i = 0; i < n; i++) {
        buckets[arr[i]]++;
    }

    int index = 0;
    for (int i = 0; i <= max; i++) {
        while (buckets[i] > 0) {
            arr[index++] = i;
            buckets[i]--;
        }
    }

    free(buckets);
}

int main() {

    int n = 1000000;

    srand(time(NULL));

    int arr[n];
    for (int i = 0; i < n; i++) { 
        arr[i] = rand() % 100;
    }

    clock_t start = clock();

    bucketSort(arr, n);

    clock_t end = clock();
    double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Execution time: %f seconds\n", cpu_time_used);

    return 0;
}