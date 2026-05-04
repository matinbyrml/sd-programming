#include <iostream>
#include <thread>    // Required for std::thread and std::jthread
#include <chrono>    // Required for time management (sleep_for)
#include <utility>   // Required for std::move

using namespace std;

// --- WORKER FUNCTION 1: The Background Task ---
void independent_task() {
    // get_id() prints the unique internal OS ID of this specific thread
    cout << "[Task 1] Born! My Thread ID is: " << this_thread::get_id() << endl;
    
    // sleep_for pauses ONLY this thread, simulating hard work
    this_thread::sleep_for(chrono::seconds(1)); 
    
    cout << "[Task 1] Finished my independent work!" << endl;
}

// --- WORKER FUNCTION 2: The Reference Task ---
// Notice the '&'. We want to modify the REAL variable in main().
void math_task(int& shared_value) {
    cout << "[Task 2] Doing math on shared variable..." << endl;
    
    // yield() tells the OS: "I'm not busy right now, let another thread use the CPU."
    // This is useful for efficiency in heavy concurrent programs!
    this_thread::yield(); 
    
    shared_value = 42; // Modify the homeowner's data
}

// --- WORKER FUNCTION 3: The Modern Task ---
void modern_task() {
    cout << "[Task 3] I am a C++20 jthread. I am stress-free!" << endl;
}


// ==========================================
// MAIN PROGRAM
// ==========================================
int main() {
    // 1. System Information
    // Find out how many hardware threads (cores) your computer actually has
    unsigned int cores = thread::hardware_concurrency();
    cout << "--- SYSTEM INFO ---" << endl;
    cout << "Your CPU supports " << cores << " concurrent threads." << endl << endl;

    
    // 2. Creating and Moving Threads (std::move)
    cout << "--- TEST 1: Move and Detach ---" << endl;
    std::thread empty_worker; // Creates a completely empty, inactive thread object
    std::thread temp_worker(independent_task); // Creates an active thread
    
    // Threads CANNOT be copied. They must be moved. 
    // We steal the active thread from temp_worker and give it to empty_worker.
    empty_worker = std::move(temp_worker); 
    
    // detach() cuts the thread loose. The main program will NO LONGER wait for it.
    empty_worker.detach(); 


    // 3. Passing by Reference and Joining
    cout << "\n--- TEST 2: Pass by Reference and Join ---" << endl;
    int my_number = 0;
    
    // If a thread function takes a reference, you MUST wrap the variable in std::ref()
    std::thread math_worker(math_task, std::ref(my_number));
    
    // We MUST wait for math_worker to finish, otherwise my_number might still be 0!
    math_worker.join(); 
    cout << "Result from Task 2 is: " << my_number << endl;


    // 4. The C++20 Standard: std::jthread
    cout << "\n--- TEST 3: The Magic of jthread ---" << endl;
    // jthread stands for "Joining Thread". 
    // When main() ends and this variable goes out of scope, it automatically calls join()!
    std::jthread smart_worker(modern_task);


    // Because Task 1 was detached and takes 1 second, we sleep main for a bit 
    // just to ensure we can see its final print statement before the program closes entirely.
    this_thread::sleep_for(chrono::seconds(2));
    
    cout << "\nMain program is exiting gracefully." << endl;
    return 0; // smart_worker automatically joins right here!
}