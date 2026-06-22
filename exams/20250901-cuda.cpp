#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <cuda_runtime.h>

// ==========================================
// DUMMY CURAND FUNCTION (As requested by the exam prompt)
// ==========================================
// The prompt states to "suppose there is a function named my_curand()". 
// We define a simple mock version here so the code is compilable.

__global__ void simulatePedestrians(const int* d_in, int* d_out, int width, int height) {
    // 1. Calculate absolute grid coordinates
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    int row = blockIdx.y * blockDim.y + threadIdx.y;

    // Boundary check
    if (col >= width || row >= height) return;

    int idx = row * width + col;
    int agentID = d_in[idx];

    // If cell is empty, the thread does nothing
    if (agentID != 0) {
        
        // 2. Decide random direction (8 contiguous cells + current cell = 9 options)
        // my_curand() returns [0, 1]. Multiplying by 9 gives an int from 0 to 8.
        int rand_dir = (int)(my_curand(idx + agentID) * 9.0f);
        if (rand_dir == 9) rand_dir = 8; // Safety clamp
        
        // Decode direction into X and Y offsets (-1, 0, or 1)
        int dx = (rand_dir % 3) - 1;
        int dy = (rand_dir / 3) - 1;
        
        int dest_col = col + dx;
        int dest_row = row + dy;

        // 3. Prevent moving out of bounds (clamp to matrix edges)
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

// ==========================================
// 2. THE HOST CODE (Main)
// ==========================================
int main(int argc, char** argv) {
    // 1. Parse Command Line Arguments
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <FNAME> <N>\n";
        return 1;
    }
    std::string fname = argv[1];_
    int N = std::atoi(argv[2]);

    // 2. Read the ASCII File
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

    // 3. GPU Memory Allocation (Double Buffering)
    int *d_grid_in, *d_grid_out;
    cudaMalloc(&d_grid_in, bytes);
    cudaMalloc(&d_grid_out, bytes);
    // Copy initial state to device
    cudaMemcpy(d_grid_in, h_matrix.data(), bytes, cudaMemcpyHostToDevice);

    // 4. Grid and Block Configuration
    dim3 threadsPerBlock(16, 16);
    dim3 numBlocks((width + threadsPerBlock.x - 1) / threadsPerBlock.x,
                   (height + threadsPerBlock.y - 1) / threadsPerBlock.y);

    // 5. Simulation Loop
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

    // 6. Retrieve Final Results
    // Because we swapped at the end of the loop, d_grid_in holds the final state
    cudaMemcpy(h_matrix.data(), d_grid_in, bytes, cudaMemcpyDeviceToHost);
    std::cout << "Simulation complete.\n";

    // Clean up
    cudaFree(d_grid_in);
    cudaFree(d_grid_out);

    return 0;
}