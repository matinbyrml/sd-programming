#include <iostream>
#include <vector>
#include <cuda_runtime.h>
using namespace std;


__global__ void filteredVectorAdd(const int* a, const int* b, int* c, int* d_count, int threshold, int n) {
    // get the global thread id
    int idx = blockIdx.x * blockDim.x + threadIdx.x;

    // Boundary check
    if (idx < n) {
        int sum = a[idx] + b[idx];
        
        // Filter condition
        if (sum > threshold) {
            /*
             * THE CORE TRICK: atomicAdd returns the OLD value of the variable.
             * If d_count is 0, the first thread to arrive gets '0' and increments the counter to 1.
             * The second thread gets '1' and increments to 2, and so on.
             * This guarantees every thread gets a unique, gapless index to write to!
             */
            int compact_index = atomicAdd(d_count, 1);
            c[compact_index] = sum;
        }
    }
}

int main(){
    // define the data in the host memory, automatically managed the lifecycle
    vector<int> h_a = {1, 4, 7, 2, 9};
    vector<int> h_b  = {3, 5, 1, 6, 4};

    int treshold = 8;
    int n = h_a.size();

    vector<int> h_c(n);
    int h_count = 0;

    size_t size = n * sizeof(int);

    // defined data vectors above cannot be passed to gpu kernel, we use pointers
    int *d_a, *d_b, *d_c, *d_count;
    // allocate space for them
    cudaMalloc(&d_a, size);
    cudaMalloc(&d_b, size);
    cudaMalloc(&d_c, size);
    cudaMalloc(&d_count, sizeof(int));
    
    // now we can transfer data in binary format 
    cudaMemcpy(d_a, h_a.data(), size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_b, h_b.data(), size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_count(), &h_count, sizeof(int),cudaMemcpyHostToDevice);

    // choose a multiple of 32 
    int threadsPerBlock = 256; 
    // correctly define num of blocks
    int numBlocks = (n + threadsPerBlock - 1) / threadsPerBlock;
    filteredVectorAdd<<<numBlocks, threadsPerBlock>>>(d_a, d_b, d_c, d_count, threshold, n);

    cudaMemcpy(&h_count, d_count, sizeof(int), cudaMemcpyDeviceToHost);

    
    // 2. Copy ONLY the compacted valid elements of the array
    cudaMemcpy(h_c.data(), d_c, h_count * sizeof(int), cudaMemcpyDeviceToHost);


    // Print the compacted vector
    std::cout << "Valid sums (> " << threshold << "): { ";
    for (int i = 0; i < h_count; ++i) {
        std::cout << h_c[i];
        if (i < h_count - 1) std::cout << ", ";
    }
    std::cout << " }\n";
    std::cout << "Total valid elements: " << h_count << "\n";

    // Free device memory
    cudaFree(d_a);
    cudaFree(d_b);
    cudaFree(d_c);
    cudaFree(d_count);

    return 0;
}