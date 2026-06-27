#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <atomic>
#include <random>
#include <memory>
#include <algorithm>

void simulate_chunk(int start_row, int end_row, int width, int height,
                    std::atomic<int>* current_grid, 
                    std::atomic<int>* next_grid) {
    
    // [EXAM TRICK] thread_local ensures each thread gets its own random engine!
    // Sharing one engine across threads causes severe race conditions and blocking.
    thread_local std::random_device rd;
    thread_local std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(-1, 1);

    for (int r = start_row; r < end_row; ++r) {
        for (int c = 0; c < width; ++c) {
            int idx = r * width + c;
            
            // Read current cell. memory_order_relaxed is fast and safe here.
            int agentID = current_grid[idx].load(std::memory_order_relaxed);

            if (agentID != 0) { // We found an agent
                // 1. Randomly decide the next move (-1, 0, or 1 for x and y)
                int dx = dist(gen);
                int dy = dist(gen);

                // 2. Boundary Checking (Agents must never get out of the matrix)
                int dest_r = std::max(0, std::min(height - 1, r + dy));
                int dest_c = std::max(0, std::min(width - 1, c + dx));
                int dest_idx = dest_r * width + dest_c;

                // 3. THE C++ ATOMIC COMPARE AND SWAP (CAS)
                // This is the C++ equivalent of CUDA's atomicCAS.
                int expected = 0; // We expect the destination to be empty (0)
                
                // compare_exchange_strong checks if next_grid[dest_idx] == expected (0).
                // If TRUE: it writes agentID and returns true.
                // If FALSE: it writes nothing, updates 'expected' to whoever is there, and returns false.
                bool moved = next_grid[dest_idx].compare_exchange_strong(expected, agentID);

                // 4. Fallback: If move failed (cell taken by another agent), we MUST claim our original spot
                if (!moved && dest_idx != idx) {
                    int expected_orig = 0;
                    next_grid[idx].compare_exchange_strong(expected_orig, agentID);
                }
            }
        }
    }
}

// ==========================================
// MAIN: The Orchestrator
// ==========================================
int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <FNAME> <N>\n";
        return 1;
    }

    std::string fname = argv[1];
    int N = std::atoi(argv[2]);

    // 1. File I/O
    std::ifstream file(fname);
    if (!file.is_open()) {
        std::cerr << "Failed to open file.\n";
        return 1;
    }

    int width, height;
    file >> width >> height; // First line is size
    
    int num_elements = width * height;

    // [EXAM TRICK] Using unique_ptr for arrays of atomics. 
    // std::vector cannot store std::atomic because atomics are non-copyable.
    auto grid_A = std::make_unique<std::atomic<int>[]>(num_elements);
    auto grid_B = std::make_unique<std::atomic<int>[]>(num_elements);

    // Read the matrix state into grid_A
    for (int i = 0; i < num_elements; ++i) {
        int val;
        file >> val;
        grid_A[i].store(val, std::memory_order_relaxed);
        grid_B[i].store(0, std::memory_order_relaxed);
    }
    file.close();

    // Pointers for Double-Buffering
    std::atomic<int>* current_grid = grid_A.get();
    std::atomic<int>* next_grid = grid_B.get();

    // Partitioning: Calculate how many rows each thread will handle
    int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) num_threads = 4; // Fallback
    int rows_per_thread = height / num_threads;

    std::cout << "Simulating " << N << " cycles with " << num_threads << " threads...\n";

    // 2. The N-Iteration Cycle
    for (int cycle = 0; cycle < N; ++cycle) {
        
        std::vector<std::thread> threads;

        // Step A: Clean the 'next_grid' buffer for the upcoming cycle
        for(int i = 0; i < num_elements; ++i) {
            next_grid[i].store(0, std::memory_order_relaxed);
        }

        // Step B: Launch partitioned threads
        for (int i = 0; i < num_threads; ++i) {
            int start_row = i * rows_per_thread;
            // The last thread cleans up any remaining rows
            int end_row = (i == num_threads - 1) ? height : start_row + rows_per_thread;

            threads.emplace_back(simulate_chunk, start_row, end_row, width, height,
                                 current_grid, next_grid);
        }

        // Step C: Synchronization Barrier
        // The main thread waits for all workers to finish the current iteration
        for (auto& t : threads) {
            t.join();
        }

        // Step D: Double Buffering Swap
        // The "next" state becomes the "current" state for the next iteration
        std::swap(current_grid, next_grid);
    }

    std::cout << "Simulation complete!\n";
    return 0;
}