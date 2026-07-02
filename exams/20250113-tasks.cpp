/*
    There are two producers, process P1 and process P2. P1 produces elements in the FIFO named FA. P2
    produces elements in the FIFO named FB. Suppose that P1 and P2 generate strings (they can read them from
    standard input or a file) at random intervals (ranging from 1 to 10 seconds).
    Consumer C consumes an element from FA, then one from FB, and then restarts the entire cycle from FA. It
    displays all strings it reads on standard output (or save in a file).
    The entire process ends once the consumer receives two consecutive strings equal to "end" (one from FA and
    one from FB).
*/#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>
#include <chrono>
#include <random>

using namespace std;

// --- Shared Resources (The FIFOs) ---
queue<string> FA;
queue<string> FB;

// --- Synchronization Primitives ---
// Separate mutexes and CVs so P1 and P2 do not block each other
mutex mtx_A;
condition_variable cv_A;

mutex mtx_B;
condition_variable cv_B;

// Mutex to protect standard input so threads don't read the same word
mutex mtx_cin; 

// Helper to simulate random intervals (1 to 10 seconds)
void simulate_random_delay(int seed_offset) {
    mt19937 rng(random_device{}() + seed_offset);
    uniform_int_distribution<int> dist(1, 10);
    // For testing purposes, you might want to change this to milliseconds
    this_thread::sleep_for(chrono::seconds(dist(rng)));
}

// --- Producer 1 ---
void producer_P1() {
    string input_str;
    while (true) {
        simulate_random_delay(1);
        
        // Safely read from standard input
        mtx_cin.lock();
        cout << "[P1] Enter a string for FA: ";
        cin >> input_str;
        mtx_cin.unlock();
        
        // Critical Section: Push to FA
        unique_lock<mutex> lock(mtx_A);
        FA.push(input_str);
        lock.unlock(); // Unlock before notifying for efficiency
        
        cv_A.notify_one();
        
        // Terminate thread if "end" is provided
        if (input_str == "end") {
            break;
        }
    }
}

// --- Producer 2 ---
void producer_P2() {
    string input_str;
    while (true) {
        simulate_random_delay(2);
        
        // Safely read from standard input
        mtx_cin.lock();
        cout << "[P2] Enter a string for FB: ";
        cin >> input_str;
        mtx_cin.unlock();
        
        // Critical Section: Push to FB
        unique_lock<mutex> lock(mtx_B);
        FB.push(input_str);
        lock.unlock(); // Unlock before notifying for efficiency
        
        cv_B.notify_one();
        
        // Terminate thread if "end" is provided
        if (input_str == "end") {
            break;
        }
    }
}

// --- Consumer ---
void consumer_C() {
    while (true) {
        string str_A, str_B;

        // 1. Consume from FA
        {
            unique_lock<mutex> lock(mtx_A);
            cv_A.wait(lock, []{ return !FA.empty(); });
            str_A = FA.front();
            FA.pop();
        } // mtx_A is automatically released here

        // 2. Consume from FB
        {
            unique_lock<mutex> lock(mtx_B);
            cv_B.wait(lock, []{ return !FB.empty(); });
            str_B = FB.front();
            FB.pop();
        } // mtx_B is automatically released here

        // 3. Display strings
        cout << "\n>>> Consumer read pair: [" << str_A << "] and [" << str_B << "]\n\n";

        // 4. Check Termination Condition
        if (str_A == "end" && str_B == "end") {
            cout << "Termination sequence received. Consumer exiting.\n";
            break;
        }
    }
}

int main() {
    cout << "Starting Producer/Consumer Simulation. Type 'end' to terminate a stream.\n\n";

    // Launch threads
    thread t1(producer_P1);
    thread t2(producer_P2);
    thread tc(consumer_C);

    // Join threads to ensure graceful exit
    t1.join();
    t2.join();
    tc.join();

    cout << "Program ended successfully.\n";
    return 0;
}