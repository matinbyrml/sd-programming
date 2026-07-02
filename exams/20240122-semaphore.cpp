#include <iostream>
#include <thread>
#include <semaphore>
#include <random>
#include <chrono>
#include <mutex>

using namespace std;

// --- Synchronization Primitives ---

// "Space" semaphores start at 1. This allows the generators to run their very first cycle.
binary_semaphore cl_space{1};
binary_semaphore na_space{1};

// "Ready" semaphores start at 0. The Combiner thread must wait until atoms are generated.
binary_semaphore cl_ready{0};
binary_semaphore na_ready{0};

// Mutex to prevent console output from scrambling
mutex print_mtx;

// --- Thread 1: Chlorine Generator ---
void generate_Cl() {
    // Thread-local random number generator (0 to 5 seconds)
    mt19937 rng(random_device{}() + 1);
    uniform_int_distribution<int> dist(0, 5);

    while (true) {
        cl_space.acquire(); // Wait until there is space for a new Cl atom

        int sleep_time = dist(rng);
        this_thread::sleep_for(chrono::seconds(sleep_time));

        {
            lock_guard<mutex> lock(print_mtx);
            cout << "[+] Chlorine (Cl) atom generated (took " << sleep_time << "s).\n";
        }

        cl_ready.release(); // Signal the combiner that Cl is ready
    }
}

// --- Thread 2: Sodium Generator ---
void generate_Na() {
    // Thread-local random number generator (0 to 5 seconds)
    mt19937 rng(random_device{}() + 2);
    uniform_int_distribution<int> dist(0, 5);

    while (true) {
        na_space.acquire(); // Wait until there is space for a new Na atom

        int sleep_time = dist(rng);
        this_thread::sleep_for(chrono::seconds(sleep_time));

        {
            lock_guard<mutex> lock(print_mtx);
            cout << "[+] Sodium (Na) atom generated (took " << sleep_time << "s).\n";
        }

        na_ready.release(); // Signal the combiner that Na is ready
    }
}

// --- Thread 3: Combiner (Molecule Producer) ---
void make_NaCl() {
    int cycle = 1;
    while (true) {
        // Wait for BOTH atoms to be ready
        cl_ready.acquire();
        na_ready.acquire();

        {
            lock_guard<mutex> lock(print_mtx);
            cout << ">>> NaCl molecule #" << cycle << " created! Restarting process... <<<\n\n";
        }
        cycle++;

        // Release the space semaphores to restart the generators!
        cl_space.release();
        na_space.release();
    }
}

int main() {
    cout << "Starting NaCl Molecule Factory...\n\n";

    // Launch the threads
    thread t_cl(generate_Cl);
    thread t_na(generate_Na);
    thread t_nacl(make_NaCl);

    // Join the threads (they run infinitely in this simulation)
    t_cl.join();
    t_na.join();
    t_nacl.join();

    return 0;
}