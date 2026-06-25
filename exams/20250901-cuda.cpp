#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <cuda_runtime.h>


__global__ void simulatePedestrians(const int* d_in, int* d_out, int width, int height) {
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    int row = blockIdx.y * blockDim.y + threadIdx.y;

    if (col >= width || row >= height) return;

    int idx = row * width + col;
    int agentID = d_in[idx];

    if (agentID != 0) {
        
        // 2. Decide random direction (8 contiguous cells + current cell = 9 options)
        // my_curand() returns [0, 1]. Multiplying by 9 gives an int from 0 to 8.
        int rand_dir = (int)(my_curand(idx + agentID) * 9.0f);
        if (rand_dir == 9) rand_dir = 8; // Safety clamp
        
        int dx = (rand_dir % 3) - 1;
        int dy = (rand_dir / 3) - 1;
        
        int dest_col = col + dx;
        int dest_row = row + dy;

        if (dest_col < 0) dest_col = 0;
        if (dest_col >= width) dest_col = width - 1;
        if (dest_row < 0) dest_row = 0;
        if (dest_row >= height) dest_row = height - 1;

        int dest_idx = dest_row * width + dest_col;

        // 4. ATOMIC COMPARE AND SWAP (The Core Logic)
        // atomicCAS checks if d_out[dest_idx] == 0. 
        // If TRUE: It writes agentID and returns 0 (Move Successful!).
        // If FALSE: It writes nothing and returns the occupying ID (Move Failed!).
        int old_val = atomicCAS(&d_out[dest_idx], 0, agentID);

        // 5. Fallback: If the destination was taken, the agent MUST stay in its old cell
        if (old_val != 0 && dest_idx != idx) {
            // Attempt to claim original position in the new buffer
            atomicCAS(&d_out[idx], 0, agentID);
        }
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <FNAME> <N>\n";
        return 1;
    }
    std::string fname = argv[1];_
    int N = std::atoi(argv[2]);

    std::ifstream file(fname);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << fname << "\n";
        return 1;
    }

    int width, height;
    file >> width >> height; // First line: size

    int numElements = width * height;
    size_t bytes = numElements * sizeof(int);

    std::vector<int> h_matrix(numElements);
    for (int i = 0; i < numElements; ++i) {
        file >> h_matrix[i];
    }
    file.close();

    int *d_grid_in, *d_grid_out;
    cudaMalloc(&d_grid_in, bytes);
    cudaMalloc(&d_grid_out, bytes);
    cudaMemcpy(d_grid_in, h_matrix.data(), bytes, cudaMemcpyHostToDevice);

    dim3 threadsPerBlock(16, 16);
    dim3 numBlocks((width + threadsPerBlock.x - 1) / threadsPerBlock.x,
                   (height + threadsPerBlock.y - 1) / threadsPerBlock.y);

    std::cout << "Starting simulation for " << N << " cycles...\n";
    for (int cycle = 0; cycle < N; ++cycle) {
        
        // Step A: Clear the output buffer for the new cycle
        cudaMemset(d_grid_out, 0, bytes);

        // Step B: Launch Kernel
        simulatePedestrians<<<numBlocks, threadsPerBlock>>>(d_grid_in, d_grid_out, width, height);

        // Step C: Global Synchronization
        // cudaDeviceSynchronize() guarantees all threads have finished the current cycle
        cudaDeviceSynchronize(); 

        // Step D: Swap pointers for the next iteration (d_grid_out becomes d_grid_in)
        std::swap(d_grid_in, d_grid_out);
    }
    cudaMemcpy(h_matrix.data(), d_grid_in, bytes, cudaMemcpyDeviceToHost);
    std::cout << "Simulation complete.\n";
    cudaFree(d_grid_in);
    cudaFree(d_grid_out);

    return 0;
}