/*
Report a C++ code snippet with a typical use case scenario for condition variables. In the code, two producers,
P1 and P2, randomly generate small alphabetic letters and pass them to a single consumer, C. The consumer
converts the letter to its corresponding uppercase and displays it on standard output before accepting the next
generated letter. Make sure that no generated letter is lost before it is displayed.
*/
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <random>
#include <cctype>
#include <chrono>

// Shared resources
std::mutex mtx;
std::condition_variable cv_producer;
std::condition_variable cv_consumer;

char shared_buffer;
bool is_empty = true; // Ensures the consumer reads before a producer overwrites

// Producer function
void producer(int id, int items_to_produce) {
    // Thread-local random number generator for lowercase letters
    std::mt19937 rng(std::random_device{}() + id);
    std::uniform_int_distribution<int> dist('a', 'z');

    for (int i = 0; i < items_to_produce; ++i) {
        char random_letter = dist(rng);

        // 1. Lock the mutex
        std::unique_lock<std::mutex> lock(mtx);

        // 2. Wait until the buffer is empty
        // The lambda prevents spurious wakeups
        cv_producer.wait(lock, [] { return is_empty; });

        // 3. Produce the item (write to buffer)
        shared_buffer = random_letter;
        is_empty = false;

        // 4. Notify the consumer that data is ready
        cv_consumer.notify_one();
        
        // Optional: Small sleep to simulate work and force thread interleaving
        lock.unlock(); 
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

// Consumer function
void consumer(int total_items_to_consume) {
    for (int i = 0; i < total_items_to_consume; ++i) {
        // 1. Lock the mutex
        std::unique_lock<std::mutex> lock(mtx);

        // 2. Wait until the buffer has data
        cv_consumer.wait(lock, [] { return !is_empty; });

        // 3. Consume the item
        char uppercase_letter = std::toupper(shared_buffer);
        std::cout << uppercase_letter << " " << std::flush;
        
        // 4. Mark buffer as empty
        is_empty = true;

        // 5. Notify a waiting producer that the buffer is free
        cv_producer.notify_one();
    }
}

int main() {
    const int ITEMS_PER_PRODUCER = 10;
    const int TOTAL_ITEMS = ITEMS_PER_PRODUCER * 2;

    std::cout << "Starting Producer-Consumer (2 Producers, 1 Consumer)...\n";
    std::cout << "Output: ";

    // Launch threads
    std::thread P1(producer, 1, ITEMS_PER_PRODUCER);
    std::thread P2(producer, 2, ITEMS_PER_PRODUCER);
    std::thread C(consumer, TOTAL_ITEMS);

    // Wait for all threads to finish
    P1.join();
    P2.join();
    C.join();

    std::cout << "\n\nAll items processed successfully.\n";

    return 0;
}