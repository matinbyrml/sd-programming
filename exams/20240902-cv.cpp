#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <random>
#include <chrono>

using namespace std;

// --- Shared State ---
const int N = 5; // Predefined constant for number of threads
vector<int> buffer;
int current_sum = 0;
bool terminate_flag = false;

// --- Synchronization Primitives ---
mutex mtx;
condition_variable cv_main;    // Used by workers to wake up the main thread

// --- Worker Thread (Producer) ---
void worker(int id) {
    // Thread-local random number generators
    mt19937 rng(random_device{}() + id);
    uniform_int_distribution<int> val_dist(1, 10);
    uniform_int_distribution<int> time_dist(1, 10);

    while (true) {
        int sleep_time = time_dist(rng);
        int random_val = val_dist(rng);

        // Standard sleep (Make sure you sleep BEFORE locking the mutex!)
        this_thread::sleep_for(chrono::seconds(sleep_time));

        unique_lock<mutex> lock(mtx);
        
        // Check if main thread signaled to stop while we were sleeping
        if (terminate_flag) {
            break; 
        }

        // --- Append to the array ---
        buffer.push_back(random_val);
        current_sum += random_val;
        
        cout << "[Thread " << id << "] Appended " << random_val << " (Sum is now " << current_sum << ")\n";

        // Unlock BEFORE notifying to optimize performance
        lock.unlock();
        
        // Wake up the main thread so it can check the sum
        cv_main.notify_one();
    }
}

// --- Main Thread (Consumer) ---
int main() {
    cout << "Starting " << N << " threads. Target sum is > 1000...\n\n";

    // Initialize an empty array of integers (already done globally)
    // Run N threads
    vector<thread> threads;
    for (int i = 0; i < N; ++i) {
        threads.emplace_back(worker, i + 1);
    }

    // --- Main Thread Waiting Logic ---
    unique_lock<mutex> lock(mtx);
    
    // The main thread waits here. It will wake up every time a thread appends a value.
    // It checks if the sum > 1000. If not, it goes right back to sleep.
    cv_main.wait(lock, [] { 
        return current_sum > 1000; 
    });

    // --- Termination Phase ---
    cout << "\n>>> Target sum reached! (Total: " << current_sum << ") <<<\n";
    cout << "Terminating all threads...\n";
    
    // Set the flag to true. Threads will check this when they wake up.
    terminate_flag = true;
    
    lock.unlock(); // Always unlock before joining threads

    // Wait for all threads to cleanly exit (might take a few seconds for sleeping threads)
    for (auto& t : threads) {
        t.join();
    }

    // --- Display the content of the array ---
    cout << "\nFinal Array Contents (" << buffer.size() << " elements):\n[ ";
    for (size_t i = 0; i < buffer.size(); ++i) {
        cout << buffer[i] << (i < buffer.size() - 1 ? ", " : "");
    }
    cout << " ]\n\nProgram ended successfully.\n";

    return 0;
}