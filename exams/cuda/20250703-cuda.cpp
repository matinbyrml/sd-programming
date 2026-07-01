#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cuda_runtime.h>

using namespace std;
__global__ void applyGaussianBlur(const float* d_in, float* d_out, int width, int height, int filter_size) {
    // 1. Calculate the 2D grid coordinates for this thread
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    int row = blockIdx.y * blockDim.y + threadIdx.y;

    // Boundary check: ensure thread is within the image dimensions
    if (col >= width || row >= height) return;
    
    int d = filter_size / 2;
    
    float sum = 0.0f;
    int count = 0;

    // 3. Iterate through the neighborhood window
    for (int r = row - d; r <= row + d; ++r) {
        for (int c = col - d; c <= col + d; ++c) {
            
            // BOUNDARY CHECK: Only include pixels that actually exist in the image
            if (r >= 0 && r < height && c >= 0 && c < width) {
                // Flatten the 2D coordinate into a 1D index
                int neighbor_idx = r * width + c;
                sum += d_in[neighbor_idx];
                count++;
            }
        }
    }

    int target_idx = row * width + col;
    d_out[target_idx] = sum / (float)count;

}


int main(int argc, char ** argv){
    int filter_size = 3; // Default
    if (argc > 1) {
        filter_size = std::atoi(argv[1]);
        if (filter_size != 3 && filter_size != 5 && filter_size != 7) {
            std::cerr << "Invalid filter size. Must be 3, 5, or 7.\n";
            return 1;
        }
    }

    int width = 1024;
    int height = 1024;
    int numElements = width * height;
    size_t bytes = sizeof(int) *  numElements;

    vector<float> h_in(numElements);
    vector<float> h_out(numElements);

    std::srand(std::time(nullptr));
    for (int i = 0; i < numElements; ++i) {
        h_in[i] = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    }

    float *d_in, *d_out;
    cudaMalloc(&d_in, bytes);
    cudaMalloc(&d_out, bytes);

    cudaMemcpy(d_in, h_in.data(), bytes, cudaMemcpyHostToDevice);

    dim3 threadsPerBlock(16, 16); 
    dim3 numBlocks((width + threadsPerBlock.x - 1) / threadsPerBlock.x,
                   (height + threadsPerBlock.y - 1) / threadsPerBlock.y);

    applyGaussianBlur<<<numBlocks, threadsPerBlock>>>(d_in, d_out, width, height, filter_size);
    cudaDeviceSynchronize();

    // 6. Retrieve Final Results
    cudaMemcpy(h_out.data(), d_out, bytes, cudaMemcpyDeviceToHost);

    std::cout << "Blur applied successfully.\n";

    // 7. Clean up Device Memory
    cudaFree(d_in);
    cudaFree(d_out);


}