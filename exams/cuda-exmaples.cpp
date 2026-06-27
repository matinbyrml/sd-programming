#include <iostream>
#include <vector>
#include <fstream>
#include <random>
#include <cuda_runtime.h>

// =========================================================================
// KERNEL 1: 2D GRID NEIGHBORHOOD PATTERN
// [EXAM ADAPTATION]: Use this exact nested-loop pattern if the exam asks for:
// - Gaussian Blurs or Image Filters
// - Conway's Game of Life (Cellular Automata)
// - Heat diffusion / Matrix smoothing
// =========================================================================
__global__ void blurKernel2D(const float* d_in, float* d_out, int width, int height) {
    // 1. THE 2D INDEXING FORMULA (MEMORIZE THIS)
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    int row = blockIdx.y * blockDim.y + threadIdx.y;

    // 2. THE GLOBAL BOUNDARY CHECK
    // Prevents threads mapped outside the image from crashing the GPU.
    if (col >= width || row >= height) return;

    // Flatten to 1D memory address because VRAM is physically 1D
    int idx = row * width + col;
    
    float sum = 0.0f;
    int count = 0;

    // 3. THE NEIGHBORHOOD WINDOW
    // Change '1' to 'filter_size / 2' if the exam asks for a dynamic window size.
    for (int r = row - 1; r <= row + 1; ++r) {
        for (int c = col - 1; c <= col + 1; ++c) {
            
            // 4. THE LOCAL BOUNDARY CHECK (CRITICAL FOR EXAMS)
            // If we are at pixel (0,0), row - 1 is -1. This check prevents reading out of bounds!
            if (r >= 0 && r < height && c >= 0 && c < width) {
                sum += d_in[r * width + c];
                count++;
            }
        }
    }
    // [HOW TO ADAPT]: If Game of Life, replace the division with "if (count == 3) d_out[idx] = 1;"
    d_out[idx] = sum / (float)count;
}


// =========================================================================
// KERNEL 2: 1D DATA FILTERING & "TICKET DISPENSER" PATTERN
// [EXAM ADAPTATION]: Use this exact atomicAdd trick if the exam asks you to:
// - "Compact" an array (remove zeros/invalid data)
// - Filter values above a threshold into a new list
// - Create a queue of tasks
// =========================================================================
__global__ void compactKernel1D(const float* d_in, float* d_compacted, int* d_global_counter, float threshold, int size) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;

    if (idx < size) {
        float val = d_in[idx];
        
        if (val > threshold) {
            // [THE TICKET DISPENSER TRICK]
            // atomicAdd returns the OLD value in memory before the addition.
            // Thread A arrives, sees 0, increments to 1, and gets '0' to use as its index.
            // Thread B arrives, sees 1, increments to 2, and gets '1' to use as its index.
            // This guarantees every passing thread gets a gapless, unique slot in the new array!
            int my_unique_slot = atomicAdd(d_global_counter, 1);
            
            d_compacted[my_unique_slot] = val;
        }
    }
}


// =========================================================================
// KERNEL 3: 2D CONFLICT RESOLUTION (atomicCAS)
// [EXAM ADAPTATION]: Use this pattern if the exam asks you to:
// - Move agents/pedestrians without them overlapping.
// - Implement a custom lock (Mutex) on the GPU.
// - Update a global Minimum/Maximum value safely.
// =========================================================================
__global__ void agentSimKernel(const int* d_grid_in, int* d_grid_out, int width, int height) {
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    if (col >= width || row >= height) return;

    int idx = row * width + col;
    
    // 'agentID' is just our state. It could be a particle type, a pedestrian, etc.
    int my_state = d_grid_in[idx];

    if (my_state != 0) { // 0 means empty cell. If not empty, we need to move it!
        
        // [HOW TO HANDLE MOVEMENT]
        // If the exam provides 'my_curand()', use it here. 
        // Otherwise, use a math trick like this to generate a pseudo-random direction (-1, 0, or 1).
        int dx = (my_state % 3) - 1; 
        int dy = ((my_state / 3) % 3) - 1;

        // Clamp the destination so the agent doesn't walk off the edge of the matrix
        int dest_col = max(0, min(width - 1, col + dx));
        int dest_row = max(0, min(height - 1, row + dy));
        int dest_idx = dest_row * width + dest_col;

        // [THE ATOMIC BOOKING SYSTEM]
        // atomicCAS(address, compare, new_val) means "Compare And Swap".
        // It asks the hardware: "Is the cell at dest_idx currently equal to 0 (empty)?"
        // IF YES: Write 'my_state' into it, and return 0. (Move Successful!)
        // IF NO:  Touch nothing, and return the ID of whoever is already there. (Move Failed!)
        int cell_status = atomicCAS(&d_grid_out[dest_idx], 0, my_state);

        // [THE FALLBACK MECHANISM]
        // If cell_status != 0, it means someone else beat us to the cell!
        // Also ensure we actually tried to move (dest_idx != idx).
        if (cell_status != 0 && dest_idx != idx) {
            // Since we failed to move, we MUST write ourselves back into our original starting cell
            // so we don't disappear from the simulation.
            atomicCAS(&d_grid_out[idx], 0, my_state);
        }
    }
}


// =========================================================================
// HOST: THE MASTER ORCHESTRATOR
// =========================================================================
int main() {
    int width = 1024, height = 1024;
    int total_pixels = width * height;
    size_t float_bytes = total_pixels * sizeof(float);
    size_t int_bytes = total_pixels * sizeof(int);

    // ---------------------------------------------------------
    // 1. HOST INIT 
    // ---------------------------------------------------------
    std::vector<float> h_in(total_pixels);
    for (int i = 0; i < total_pixels; i++) {
        h_in[i] = static_cast<float>(std::rand()) / RAND_MAX; 
    }

    // ---------------------------------------------------------
    // 2. DEVICE ALLOCATIONS (Standard Boilerplate)
    // ---------------------------------------------------------
    float *d_in, *d_blurred, *d_compacted;
    int *d_global_counter, *d_sim_in, *d_sim_out;

    cudaMalloc(&d_in, float_bytes);
    cudaMalloc(&d_blurred, float_bytes);
    cudaMalloc(&d_compacted, float_bytes);
    
    cudaMalloc(&d_global_counter, sizeof(int)); // Single integer for the ticket dispenser
    
    cudaMalloc(&d_sim_in, int_bytes);
    cudaMalloc(&d_sim_out, int_bytes);

    // ---------------------------------------------------------
    // 3. MEMORY INITIALIZATION (Crucial for Atomics)
    // ---------------------------------------------------------
    cudaMemcpy(d_in, h_in.data(), float_bytes, cudaMemcpyHostToDevice);
    
    // [EXAM TRICK: PREVENTING GARBAGE DATA]
    // Any memory address used by an 'atomic' function MUST be zeroed out first!
    // If you forget this, your counter will start at whatever random garbage was left in VRAM.
    cudaMemset(d_global_counter, 0, sizeof(int));
    cudaMemset(d_sim_in, 0, int_bytes); 

    // ---------------------------------------------------------
    // 4. EXECUTION CONFIGURATIONS (The Math)
    // ---------------------------------------------------------
    // 1D Config: 256 is the universally accepted standard for 1D arrays.
    int threads1D = 256;
    // CEILING DIVISION: (total + threads - 1) / threads. Never use standard division!
    int blocks1D = (total_pixels + threads1D - 1) / threads1D;

    // 2D Config: 16x16 gives exactly 256 threads per block, perfect for matrices.
    dim3 threads2D(16, 16);
    dim3 blocks2D((width + threads2D.x - 1) / threads2D.x,
                  (height + threads2D.y - 1) / threads2D.y);

    // ---------------------------------------------------------
    // 5. KERNEL LAUNCHES & SYNCHRONIZATION
    // ---------------------------------------------------------
    
    // Launch 1D Compaction (Filters array, places passing values in d_compacted)
    compactKernel1D<<<blocks1D, threads1D>>>(d_in, d_compacted, d_global_counter, 0.8f, total_pixels);
    
    // [EXAM TRICK: WHEN TO SYNC]
    // You MUST synchronize if the CPU needs to read a result, OR if the next kernel 
    // relies on the output of the previous kernel.
    cudaDeviceSynchronize();

    // Fetch the final atomic count back to CPU so we know how many items passed the filter!
    int h_count = 0;
    cudaMemcpy(&h_count, d_global_counter, sizeof(int), cudaMemcpyDeviceToHost); 
    std::cout << "Compacted array contains " << h_count << " valid elements.\n";

    // ---------------------------------------------------------
    // 6. THE DOUBLE-BUFFERING SIMULATION LOOP
    // [EXAM ADAPTATION]: Use this loop for any problem that has "cycles", 
    // "iterations", or "time steps" where state updates continuously.
    // ---------------------------------------------------------
    int cycles = 5;
    for (int i = 0; i < cycles; i++) {
        // STEP A: Wipe the "future" board clean so we have a blank slate to move agents into.
        cudaMemset(d_sim_out, 0, int_bytes);
        
        // STEP B: Run the simulation. Threads read from 'in', and write to 'out'.
        agentSimKernel<<<blocks2D, threads2D>>>(d_sim_in, d_sim_out, width, height);
        
        // STEP C: Wait for EVERY thread to finish moving its agent.
        cudaDeviceSynchronize(); 
        
        // STEP D: Swap the pointers! The "future" board (out) now becomes the "current" board (in)
        // for the next iteration of the loop.
        std::swap(d_sim_in, d_sim_out); 
    }

    // ---------------------------------------------------------
    // 7. MEMORY HYGIENE
    // ---------------------------------------------------------
    // Forgetting these is an easy way to lose 0.5 points. Match every cudaMalloc with a cudaFree.
    cudaFree(d_in);
    cudaFree(d_blurred);
    cudaFree(d_compacted);
    cudaFree(d_global_counter);
    cudaFree(d_sim_in);
    cudaFree(d_sim_out);

    return 0;
}e