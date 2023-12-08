#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

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

        #pragma omp parallel sections
        {
            #pragma omp section
            {
                quickSort(arr, low, pi - 1);
            }

            #pragma omp section
            {
                quickSort(arr, pi + 1, high);
            }
        }
    }
}

int main() {
    int n = 1000000;
    int arr[n];

    srand(42); 
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % 1000;
    double start_time = omp_get_wtime();

    quickSort(arr, 0, n - 1);

    double end_time = omp_get_wtime();
    printf("Execution Time: %f seconds\n", end_time - start_time);

    return 0;
}
