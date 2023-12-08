#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int partition(int arr[], int low, int high) {
    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j <= high - 1; j++) {
        if (arr[j] < pivot) {
            i++;
            int temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }

    int temp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = temp;

    return i + 1;
}

void quickSort(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

int main() {
    int n = 100000;
    int arr[n];

    srand(42); 
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % 1000;
    }

    clock_t start_time = clock();

    quickSort(arr, 0, n - 1);

   clock_t end_time = clock();

    printf("Sequential Execution Time: %f seconds\n", ((double) (end_time - start_time)) / CLOCKS_PER_SEC);

    return 0;
}
