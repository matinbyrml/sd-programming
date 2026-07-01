#include <iostream>
#include <vector>
#include <thread>
#include <future>
#include <cstdlib>
 /*
 
 void mergesort(std::vector<int>& arr, int left, int right) {
    if (left>=right)
        return;
        int mid = left + (right-left) / 2;
        mergesort (arr, left, mid);
        mergesort (arr, mid + 1, right);
        merge(arr, left, mid, right);
        return;
    }
}
int main() {
    const int size = 12;
    std::vector<int> array;
    for (int i=0; i<size; i++){
        array.push
        back(rand()%10000);
        mergesort(array, 0, array.size() - 1);
        std::cout << "Sorted array: ";
        for (int val: array) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    return 0;
}
 */
// (Assumed provided by the exercise)
void merge(std::vector<int>& arr, int left, int mid, int right) { /* ... */ }

// --- 1. PARALLEL RECURSIVE SORT ---
void parallel_mergesort(std::vector<int>& arr, int left, int right) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;

    // Create a promise/future pair to synchronize the left half
    std::promise<void> p_left;
    std::future<void> f_left = p_left.get_future();

    // Spawn ONE thread to handle the left half. 
    // We move the promise into the thread so it can signal when done.
    std::thread t_left([&arr, left, mid](std::promise<void> p) {
        parallel_mergesort(arr, left, mid);
        p.set_value(); // Signal that the left half is sorted
    }, std::move(p_left));

    // Sort the right half synchronously in the CURRENT thread (saves resources!)
    parallel_mergesort(arr, mid + 1, right);

    // Wait for the left half thread to signal completion before merging
    f_left.get();
    t_left.join();

    // Merge the two halves
    merge(arr, left, mid, right);
}

// --- 2. WRAPPER TASK ---
void root_task(std::vector<int>& arr, std::future<void> start_signal, std::promise<void> done_signal) {
    // Block and wait for main to wake us up
    start_signal.get(); 

    // Start the parallel sorting
    parallel_mergesort(arr, 0, arr.size() - 1);

    // Signal main that sorting is entirely finished
    done_signal.set_value();
}

// --- 3. MAIN FUNCTION ---
int main() {
    const int size = 12;
    std::vector<int> array;

    // Setup promises and futures for Main <-> Task communication
    std::promise<void> start_promise;
    std::future<void> start_future = start_promise.get_future();

    std::promise<void> done_promise;
    std::future<void> done_future = done_promise.get_future();

    // Run the task (it will immediately go to sleep waiting for start_future)
    std::thread sorter(root_task, std::ref(array), std::move(start_future), std::move(done_promise));

    // Main sets the values into the array
    for (int i = 0; i < size; i++) {
        array.push_back(rand() % 10000);
    }

    // Wake up the sorting task!
    start_promise.set_value();

    // Wait for the sorting task to signal that it is finished
    done_future.get();
    sorter.join();

    // Print results
    std::cout << "Sorted array: ";
    for (int val : array) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    return 0;
}