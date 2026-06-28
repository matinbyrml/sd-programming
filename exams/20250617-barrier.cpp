#include <iostream>
#include <vector>
#include <thread>
#include <barrier> // C++20 barrier
#include <random>
#include <algorithm>
#include <syncstream> // C++20 thread-safe output

// The worker function executed by each thread
void worker(int id, std::barrier<>& sync_point) {
    // Thread-safe wrapper for std::cout to prevent scrambled text
    std::osyncstream bout(std::cout); 
    
    // Set up random number generation
    std::mt19937 rng(std::random_device{}() + id);
    std::uniform_int_distribution<int> size_dist(10, 50); 
    std::uniform_int_distribution<int> val_dist(1, 1000); 
    std::uniform_int_distribution<int> bool_dist(0, 1);   

    // ==========================================
    // STAGE 1: Initialize random vector
    // ==========================================
    int size = size_dist(rng);
    std::vector<int> data(size);
    for (int& x : data) {
        x = val_dist(rng);
    }
    
    bout << "Thread " << id << " finished Stage 1 (initialized " << size << " elements).\n";
    
    // SYNCHRONIZATION POINT 1
    // Decrement the barrier and block until all threads arrive.
    sync_point.arrive_and_wait();

    // ==========================================
    // STAGE 2: Sort randomly (asc/desc)
    // ==========================================
    bool sort_ascending = bool_dist(rng);
    if (sort_ascending) {
        std::sort(data.begin(), data.end());
    } else {
        std::sort(data.rbegin(), data.rend()); 
    }
    
    bout << "Thread " << id << " finished Stage 2 (sorted " 
         << (sort_ascending ? "ascending" : "descending") << ").\n";
         
    // SYNCHRONIZATION POINT 2
    // REUSING THE SAME BARRIER! The counter automatically reset after Phase 1.
    sync_point.arrive_and_wait();

    // ==========================================
    // STAGE 3: Verify the sorting
    // ==========================================
    bool is_valid = false;
    if (sort_ascending) {
        is_valid = std::is_sorted(data.begin(), data.end());
    } else {
        is_valid = std::is_sorted(data.begin(), data.end(), std::greater<int>());
    }
    
    bout << "Thread " << id << " finished Stage 3. Verification: " 
         << (is_valid ? "SUCCESS" : "FAILED") << "\n";
}

int main() {
    constexpr int N = 5; // Number of threads

    // Initialize ONE barrier for N threads. 
    // It defaults to std::barrier<> meaning it has no special completion function.
    std::barrier<> sync_point(N);

    std::vector<std::thread> threads;

    std::cout << "Starting scientific computation with " << N << " threads...\n\n";

    // Spawn N threads, passing a reference to the single barrier
    for (int i = 0; i < N; ++i) {
        threads.emplace_back(worker, i, std::ref(sync_point));
    }

    // Wait for all threads to complete execution
    for (auto& t : threads) {
        t.join();
    }

    std::cout << "\nAll threads have successfully completed the computation.\n";
    
    return 0;
}