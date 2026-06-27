#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <random>

void computeHistogramPartition(const std::vector<unsigned short>& vec, 
                               size_t start_idx, 
                               size_t end_idx, 
                               std::atomic<unsigned int>* histogram) {
    
    // Each thread only loops over its assigned slice of the array
    for (size_t i = start_idx; i < end_idx; ++i) {
        unsigned short value = vec[i];
        
        // [EXAM TRICK] Appropriately protected increment!
        // std::atomic guarantees that this read-modify-write is thread-safe,
        // exactly like atomicAdd() in CUDA, but for the CPU.
        // memory_order_relaxed is used for maximum performance.
        histogram[value].fetch_add(1, std::memory_order_relaxed); 
    }
}

int main() {
    // 1. Setup Mock Data
    const size_t size = 20'000'005; // 20 Million + 5 pixels to test the remainder
    const int num_bins = 256;
    
    std::vector<unsigned short> vec(size);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<unsigned short> distr(0, 255);
    
    for (size_t i = 0; i < size; ++i) {
        vec[i] = distr(gen);
    }

    // 2. The Shared, Protected State
    // We use an array of atomic integers. This is the modern C++ way 
    // to protect individual bins without the massive overhead of std::mutex.
    std::atomic<unsigned int> histogram[num_bins];
    for (int i = 0; i < num_bins; ++i) {
        histogram[i] = 0; // Initialize all bins to 0
    }

    // 3. Execution Configuration (Partitioning)
    int num_threads = 8; // Or use std::thread::hardware_concurrency()
    std::vector<std::thread> threads;
    
    size_t chunk_size = size / num_threads;

    std::cout << "Launching " << num_threads << " CPU threads...\n";

    // 4. Launch Threads with precise boundaries
    for (int i = 0; i < num_threads; ++i) {
        size_t start_idx = i * chunk_size;
        
        // [EXAM TRICK] The Remainder Check!
        // The last thread MUST go all the way to 'size' to catch any leftover elements.
        size_t end_idx = (i == num_threads - 1) ? size : start_idx + chunk_size;
        
        // std::ref is required to pass the vector by reference to the thread
        threads.emplace_back(computeHistogramPartition, 
                             std::ref(vec), 
                             start_idx, 
                             end_idx, 
                             histogram);
    }

    // 5. Synchronize (Barrier)
    for (auto& t : threads) {
        t.join();
    }

    // 6. Verify Results
    unsigned int total_counts = 0;
    std::cout << "\nHistogram Sample Results: \n";
    for (int i = 0; i < 10; ++i) {
        std::cout << "Bin [" << i << "]: " << histogram[i].load() << " occurrences\n";
    }
    
    // Check if the total sum matches the original array size
    for (int i = 0; i < num_bins; ++i) {
        total_counts += histogram[i].load();
    }
    
    std::cout << "...\nTotal items counted: " << total_counts 
              << " (Expected: " << size << ")\n";

    return 0;
}