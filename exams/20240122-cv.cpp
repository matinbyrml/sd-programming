#include <iostream>
#include <thread>
#include <vector>
#include <semaphore>
#include <mutex>
#include <chrono>

const int N = 5; // Number of worker threads (T_1 to T_n)

// The required synchronization primitives
std::counting_semaphore<N> sem_start(0); // Blocks workers until T signals them
std::counting_semaphore<1> sem_end(0);   // Blocks T until all workers finish

std::mutex mtx;          // Mutex to protect the counter
int finished_count = 0;  // Counter to track how many workers have finished their phase

// Master Thread T
void Thread_T() {
    while (true) {
        std::cout << "Master T: Starting new cycle. Waking up workers...\n";
        
        // 1. Top of Graph: Master T releases N tokens to wake up all T_i threads
        sem_start.release(N); 
        
        // 2. Bottom of Graph: Master T goes to sleep waiting for workers to finish
        sem_end.acquire(); 
        
        std::cout << "Master T: All workers finished. Cycle complete.\n\n";
        
        // Sleep briefly to make console output readable
        std::this_thread::sleep_for(std::chrono::milliseconds(800));
    }
}

// Worker Threads T_i
void Thread_Ti(int id) {
    while (true) {
        // 1. Wait for Master T to give the start signal
        sem_start.acquire(); 
        
        // --- CONCURRENT WORK PHASE ---
        std::cout << "  Worker T_" << id << " is executing...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(200)); 
        
        // 2. Barrier Implementation (Mutex + Counter + Semaphore)
        // Safely increment the counter to indicate this thread has finished
        std::unique_lock<std::mutex> lock(mtx);
        finished_count++;
        
        // 3. The LAST thread to arrive at the barrier triggers the wake-up
        if (finished_count == N) {
            finished_count = 0; // Reset the counter for the next cycle
            sem_end.release();  // Wake up Master T
        }
        lock.unlock(); // Always unlock after updating shared state
    }
}

int main() {
    std::cout << "Starting Precedence Graph Simulation...\n\n";

    // Spawn the Master Thread T
    std::thread master(Thread_T);

    // Spawn the N Worker Threads T_i
    std::vector<std::thread> workers;
    for (int i = 1; i <= N; ++i) {
        workers.emplace_back(Thread_Ti, i);
    }

    // Join threads (in this infinite loop simulation, they run forever)
    master.join();
    for (auto& w : workers) {
        w.join();
    }

    return 0;
}