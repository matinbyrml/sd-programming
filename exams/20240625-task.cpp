#include <iostream>
#include <vector>
#include <thread>
#include <future>
#include <algorithm> // For std::inplace_merge
#include <random>

// ==========================================
// SEQUENTIAL FALLBACK: Prevents Thread Exhaustion
// ==========================================
void sequential_mergesort(std::vector<int>& arr, int left, int right) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    sequential_mergesort(arr, left, mid);
    sequential_mergesort(arr, mid + 1, right);
    
    // Using std::inplace_merge to simulate the "merge" function
    // we don't have to care about, ensuring the code compiles and runs.
    std::inplace_merge(arr.begin() + left, arr.begin() + mid + 1, arr.begin() + right + 1);
}

// ==========================================
// THE PARALLEL WORKER
// ==========================================
void parallel_mergesort(std::vector<int>& arr, int left, int right, 
                        std::promise<void> p_done, int depth) {
    
    if (left >= right) {
        p_done.set_value();
        return;
    }

    // [EXAM TRICK] The Thread Exhaustion Check
    // Limit thread creation to a specific depth (e.g., 3 levels deep = 8 threads)
    // If we exceed this, fall back to standard sequential execution.
    if (depth > 3) {
        sequential_mergesort(arr, left, right);
        p_done.set_value();
        return;
    }

    int mid = left + (right - left) / 2;

    // 1. Create promises/futures to synchronize the left and right splits
    std::promise<void> p_left, p_right;
    std::future<void> f_left = p_left.get_future();
    std::future<void> f_right = p_right.get_future();

    // 2. Spawn threads for the recursive calls
    std::thread t1(parallel_mergesort, std::ref(arr), left, mid, std::move(p_left), depth + 1);
    std::thread t2(parallel_mergesort, std::ref(arr), mid + 1, right, std::move(p_right), depth + 1);

    // 3. Synchronization Barrier: Wait for both halves to finish sorting
    f_left.get();
    f_right.get();

    // 4. Memory cleanup
    t1.join();
    t2.join();

    // 5. Merge the two sorted halves
    std::inplace_merge(arr.begin() + left, arr.begin() + mid + 1, arr.begin() + right + 1);

    // 6. Signal the parent thread that this chunk is completely sorted
    p_done.set_value();
}

// ==========================================
// THE COORDINATOR TASK: Fulfills the "Wake Up" Requirement
// ==========================================
void sort_coordinator(std::vector<int>& arr, std::future<void> start_signal, std::promise<void> all_done) {
    // 1. BLOCKED: Wait here until 'main' populates the array and sends the wake-up signal
    start_signal.get(); 

    // 2. Now that data is ready, kick off the parallel sort
    std::promise<void> root_promise;
    std::future<void> root_future = root_promise.get_future();

    parallel_mergesort(arr, 0, arr.size() - 1, std::move(root_promise), 0);

    // 3. Wait for the entire sort to finish
    root_future.get();

    // 4. Signal main that the sorted array is ready to print
    all_done.set_value();
}

// ==========================================
// MAIN: The Orchestrator
// ==========================================
int main() {
    const int size = 12; // Test size
    std::vector<int> array;
    array.reserve(size);

    // 1. Setup the "Wake Up" and "Finished" channels
    std::promise<void> p_start; 
    std::future<void> f_start = p_start.get_future();

    std::promise<void> p_done;
    std::future<void> f_done = p_done.get_future();

    // 2. REQUIREMENT: "The main has to run a task..."
    // We launch the coordinator thread BEFORE we populate the array!
    std::thread coordinator(sort_coordinator, std::ref(array), std::move(f_start), std::move(p_done));

    // 3. REQUIREMENT: "...set the values into the array..."
    std::cout << "Main thread is populating the array...\n";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 10000);
    
    for (int i = 0; i < size; i++) {
        array.push_back(dist(gen));
    }

    // 4. REQUIREMENT: "...and wake up the task to start the sorting process."
    std::cout << "Main thread is sending the wake-up signal!\n";
    p_start.set_value(); // This unblocks the coordinator thread

    // Wait for the sorting to finish before printing
    f_done.get();
    coordinator.join();

    // Print Results
    std::cout << "Sorted array: ";
    for (int val : array) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    return 0;
}