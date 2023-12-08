%%writefile cuda_code.cu
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CUDA_CHECK(call) \
do { \
    cudaError_t result = call; \
    if (result != cudaSuccess) { \
        fprintf(stderr, "%s:%d error code %d (%s)\n", \
                _FILE, __LINE_, result, cudaGetErrorString(result)); \
        exit(EXIT_FAILURE); \
    } \
} while (0)

_global_ void bucketSortCUDA(int *arr, int *buckets, int n) {
    int tid = blockIdx.x * blockDim.x + threadIdx.x;

    if (tid < n) {
        atomicAdd(&buckets[arr[tid]], 1);
    }

    __syncthreads();

    int index = 0;
    for (int i = 0; i <= tid; i++) {
        while (buckets[i] > 0) {
            arr[index++] = i;
            buckets[i]--;
        }
    }
}

void printArray(int arr[], int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main() {
    cudaEvent_t start, stop;
    CUDA_CHECK(cudaEventCreate(&start));
    CUDA_CHECK(cudaEventCreate(&stop));

    int n_cuda = 10000;

    srand(time(NULL));

    int *arr_cuda = (int *)malloc(n_cuda * sizeof(int));
    for (int i = 0; i < n_cuda; i++) {
        arr_cuda[i] = rand() % 100;
    }

    printf("Original array (CUDA): ");
    printArray(arr_cuda, n_cuda);

    int *arr_d, *buckets_d;
    CUDA_CHECK(cudaMalloc((void**)&arr_d, n_cuda * sizeof(int)));
    CUDA_CHECK(cudaMalloc((void**)&buckets_d, (n_cuda + 1) * sizeof(int)));

    CUDA_CHECK(cudaMemcpy(arr_d, arr_cuda, n_cuda * sizeof(int), cudaMemcpyHostToDevice));
    CUDA_CHECK(cudaMemset(buckets_d, 0, (n_cuda + 1) * sizeof(int)));

    CUDA_CHECK(cudaEventRecord(start, 0));

    int threadsPerBlock = 256;
    int blocksPerGrid = (n_cuda + threadsPerBlock - 1) / threadsPerBlock;

    bucketSortCUDA<<<blocksPerGrid, threadsPerBlock>>>(arr_d, buckets_d, n_cuda);
    CUDA_CHECK(cudaDeviceSynchronize());

    CUDA_CHECK(cudaEventRecord(stop, 0));
    CUDA_CHECK(cudaEventSynchronize(stop));

    CUDA_CHECK(cudaMemcpy(arr_cuda, arr_d, n_cuda * sizeof(int), cudaMemcpyDeviceToHost));

    printf("Sorted array (CUDA): ");
    printArray(arr_cuda, n_cuda);

    float gpu_time_used;
    CUDA_CHECK(cudaEventElapsedTime(&gpu_time_used, start, stop));
    printf("Execution Time (CUDA): %f milliseconds\n", gpu_time_used);

    CUDA_CHECK(cudaFree(arr_d));
    CUDA_CHECK(cudaFree(buckets_d));

    free(arr_cuda);

    return 0;
}