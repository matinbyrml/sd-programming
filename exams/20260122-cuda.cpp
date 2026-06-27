#include <iostream>
#include <vector>
#include <random>
#include <cuda_runtime.h>

__global__ void computeHistogramKernel(const unsigned char* d_vec, unsigned int* d_histogram, int size) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x; 

    if (idx < size) {
        unsigned char value = d_vec[idx];
        
        // Atomically increment the corresponding bin in global memory
        atomicAdd(&d_histogram[value], 1); 
    }
}

int main() {
    const int size = 512 
    const int num_bins = 256; 

    std::vector<unsigned char> h_vec(size);
    std::vector<unsigned int> h_histogram(num_bins, 0); 
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0, 255);
    
    for (int i = 0; i < size; ++i) {
        h_vec[i] = static_cast<unsigned char>(distr(gen)); 
    }

    // 3. Allocate Device Memory
    unsigned char* d_vec = nullptr;
    unsigned int* d_histogram = nullptr;
    
    cudaMalloc(&d_vec, size * sizeof(unsigned char)); 
    cudaMalloc(&d_histogram, num_bins * sizeof(unsigned int)); 
    cudaMemcpy(d_vec, h_vec.data(), size * sizeof(unsigned char), cudaMemcpyHostToDevice); 
    // This works, but is slow!
    cudaMemcpy(d_histogram, h_histogram.data(), num_bins * sizeof(unsigned int), cudaMemcpyHostToDevice);
    cudaMemset(d_histogram, 0, num_bins * sizeof(unsigned int)); 
    int threadsPerBlock = 256; 
    int blocksPerGrid = (size + threadsPerBlock - 1) / threadsPerBlock; 
    
    std::cout << "Launching Kernel with " << blocksPerGrid << " blocks and " 
              << threadsPerBlock << " threads per block...\n";

    computeHistogramKernel<<<blocksPerGrid, threadsPerBlock>>>(d_vec, d_histogram, size); 

    cudaDeviceSynchronize(); 
    cudaError_t err = cudaGetLastError();
    if (err != cudaSuccess) {
        std::cerr << "CUDA Error: " << cudaGetErrorString(err) << std::endl; 
        return -1;
    }

    cudaMemcpy(h_histogram.data(), d_histogram, num_bins * sizeof(unsigned int), cudaMemcpyDeviceToHost); 

    std::cout << "\nHistogram Sample Results: \n";
    unsigned int total_counts = 0;
    
    for (int i = 0; i < 10; ++i) {
        std::cout << "Bin [" << i << "]: " << h_histogram[i] << " occurrences\n"; 
    }
    for (int i = 0; i < num_bins; ++i) {
        total_counts += h_histogram[i]; 
    }
    std::cout << "...\nTotal items counted: " << total_counts << " (Expected: " << size << ") \n"; 

    // 9. Clean up allocated device memory
    cudaFree(d_vec); 
    cudaFree(d_histogram); 

    return 0;
}