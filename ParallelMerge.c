#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void merge(int arr[], int left[], int left_size, int right[], int right_size) {
    int i = 0, j = 0, k = 0;

    while (i < left_size && j < right_size) {
        if (left[i] < right[j]) {
            arr[k++] = left[i++];
        } else {
            arr[k++] = right[j++];
        }
    }

    while (i < left_size) {
        arr[k++] = left[i++];
    }

    while (j < right_size) {
        arr[k++] = right[j++];
    }
}

void merge_sort(int arr[], int size) {
    if (size > 1) {
        int mid = size / 2;
        int *left = malloc(mid * sizeof(int));
        int *right = malloc((size - mid) * sizeof(int));

        for (int i = 0; i < mid; i++) {
            left[i] = arr[i];
        }

        for (int i = mid; i < size; i++) {
            right[i - mid] = arr[i];
        }

        merge_sort(left, mid);
        merge_sort(right, size - mid);

        merge(arr, left, mid, right, size - mid);

        free(left);
        free(right);
    }
}

void parallel_merge_sort(int arr[], int size, int rank, int size_mpi) {
    if (size > 1) {
        int mid = size / 2;
        int *left = malloc(mid * sizeof(int));
        int *right = malloc((size - mid) * sizeof(int));

        if (rank == 0) {
            // Divide the array into chunks for each process
            MPI_Scatter(arr, mid, MPI_INT, left, mid, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Scatter(arr + mid, size - mid, MPI_INT, right, size - mid, MPI_INT, 0, MPI_COMM_WORLD);
        }

        parallel_merge_sort(left, mid, rank, size_mpi);
        parallel_merge_sort(right, size - mid, rank, size_mpi);

        if (rank == 0) {
            // Gather the sorted chunks from each process
            MPI_Gather(left, mid, MPI_INT, arr, mid, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Gather(right, size - mid, MPI_INT, arr + mid, size - mid, MPI_INT, 0, MPI_COMM_WORLD);

            // Perform the final merge on the root process
            merge(arr, left, mid, right, size - mid);
        }

        free(left);
        free(right);
    }
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    printf("Started...\n");
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    printf("Defined...\n");
    if (rank == 0) {
        // Generate a random array with values from 1 to 10000
        int array_size;
        scanf("%d", &array_size);
        int *input_array = malloc(array_size * sizeof(int));
        for (int i = 0; i < array_size; i++) {
            input_array[i] = rand() % 10000 + 1;
        }

        // Record the start time
        double start_time = MPI_Wtime();

        // Perform parallel merge sort
        parallel_merge_sort(input_array, array_size, rank, size);

        // Record the end time
        double end_time = MPI_Wtime();

        printf("\n");

        printf("Execution Time: %f seconds\n", end_time - start_time);

        free(input_array);
    } else {
        parallel_merge_sort(NULL, 0, rank, size);
    }

    MPI_Finalize();

    return 0;
}
