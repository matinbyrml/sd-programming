#include <iostream>
#include <vector>
#include <thread>
#include <future>
#include <algorithm>
#include <random>

using std::cout;
using std::endl;
using std::vector;
using std::thread;
using std::promise;
using std::future;

// ==========================================
// THREAD A & B: The Generators
// ==========================================
// Reusable function for both threads
void generator_thread(future<void> start_signal, promise<vector<int>> p_array) {
    // 1. Wait for Main to wake us up!
    start_signal.get(); 

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> sizeDist(5, 15); // Smaller size for readable terminal output
    std::uniform_int_distribution<> valueDist(0, 100);

    int size = sizeDist(gen);
    vector<int> v(size);
    for (int& x : v) {
        x = valueDist(gen);
    }
    
    // 2. Sort the array
    std::sort(v.begin(), v.end());
    
    // 3. Wake up Thread C by passing the sorted vector
    p_array.set_value(std::move(v));
}

// ==========================================
// THREAD C: The Merger
// ==========================================
void merger_thread(future<vector<int>> f_arrA, future<vector<int>> f_arrB, promise<void> wake_main) {
    // 1. Wait for Threads A and B
    vector<int> vA = f_arrA.get();
    vector<int> vB = f_arrB.get();

    // 2. Merge the two sorted arrays (O(N) efficiency)
    vector<int> merged(vA.size() + vB.size());
    std::merge(vA.begin(), vA.end(), vB.begin(), vB.end(), merged.begin());

    // 3. Display it
    cout << "Merged and Sorted Array: ";
    for (int value : merged) {
        cout << value << " ";
    }
    cout << endl;

    // 4. Wake up the main thread
    wake_main.set_value(); 
}

// ==========================================
// MAIN: The Orchestrator
// ==========================================
int main() {
    // Stage 1: Promises
    promise<void> p_startA, p_startB;         // Main -> Threads A & B
    promise<vector<int>> p_arrA, p_arrB;      // Threads A & B -> Thread C
    promise<void> p_done;                     // Thread C -> Main

    // Stage 2: Futures
    future<void> f_startA = p_startA.get_future();
    future<void> f_startB = p_startB.get_future();
    future<vector<int>> f_arrA = p_arrA.get_future();
    future<vector<int>> f_arrB = p_arrB.get_future();
    future<void> f_done = p_done.get_future();

    // Stage 3: Launch Threads
    thread tA(generator_thread, std::move(f_startA), std::move(p_arrA));
    thread tB(generator_thread, std::move(f_startB), std::move(p_arrB));
    thread tC(merger_thread, std::move(f_arrA), std::move(f_arrB), std::move(p_done));

    // Stage 4: Trigger the simulation
    cout << "[Main] Waking up Threads A and B...\n";
    p_startA.set_value();
    p_startB.set_value();

    // Stage 5: Wait for Thread C to wake us up before exiting
    f_done.get();
    cout << "[Main] Thread C woke me up. Shutting down.\n";

    // Standard Cleanup
    tA.join();
    tB.join();
    tC.join();

    return 0;
}