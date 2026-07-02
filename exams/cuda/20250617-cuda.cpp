/*
The question asks us to calculate the dot product of two large vectors, v1 and v2, of size n. 
The exam specifically restricts the design to two stages:
Stage One (Device/GPU): A simple kernel runs n threads to calculate the element-wise product 
mult[i] = v1[i] * v2[i] and stores it in an intermediate array in global memory. 
Stage Two (Host/CPU): A sequential iteration performs the sum of all elements in the mult array.  
*/
#include <iostream>
#include <vector>
#include <cuda_runtime.h>

__global__ void multiplyVecotrs(cont float* v1, const float* v2, float* mult, int n){
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if(i<n){
        mult[i] =  v1[i] * v2[i];
    }
}

int main(){
    int n = 1000000;
    size_t size =  n *  sizeof(float);

    std::vector<float> h_v1(n, 1.5f);
    std::vector<float> h_v2(n, 2.0f);
    std::vector<float> h_mult(n, 0.0f);

    // Device pointers
    float *d_v1 = nullptr, *d_v2 = nullptr, *d_mult = nullptr;

    // Allocate memory on the GPU
    cudaMalloc(&d_v1, size);
    cudaMalloc(&d_v2, size);
    cudaMalloc(&d_mult, size);

    cudaMemcpy(d_v1, h_v1.data(), size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_v2, h_v2.data(), size, cudaMemcpyHostToDevice);

    // Define grid and block dimensions
    int threadsPerBlock = 256;
    int blocksPerGrid = (n + threadsPerBlock - 1) / threadsPerBlock;

    multiplyVectors<<<blocksPerGrid, threadsPerBlock>>>(d_v1, d_v2, d_mult, n);

    // Synchronize to ensure kernel finishes before copying back (cudaMemcpy handles this implicitly, but good practice to note)
    cudaDeviceSynchronize();

    cudaMemcpy(h_mult.data(), d_mult, size, cudaMemcpyDeviceToHost);
    float dot_product = 0.0f;
    for (int i = 0; i < n; i++) {
        dot_product += h_mult[i];
    }

    std::cout << "Dot product result: " << dot_product << std::endl;

    // Free device memory
    cudaFree(d_v1);
    cudaFree(d_v2);
    cudaFree(d_mult);

    return 0;

}
