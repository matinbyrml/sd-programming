#include <iostream>
#include <thread>
#include <mutex>

std::mutex mtx;
int shared_data = 0;

void flexible_worker(){
    // Deferred locking: we create a lock but not lock the mutex yet
    // It is like having a vip badge for a room but not entering yet
    std::unique_lock<std::mutex> my_lock(mtx, std::defer_lock);
    std::cout << "Doing some heavy math that DOESN'T need the shared data...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    //now that i need a shared data, i lock
    my_lock.lock();

    // --- CRITICAL SECTION ---
    shared_data++;
    std::cout << "Updated shared data to: " << shared_data << "\n";
    
    // MANUAL UNLOCK: We are done with the data, but we still have more math to do.
    // We unlock early so other threads don't have to wait for us to finish the rest of our function!
    my_lock.unlock(); 
    
    std::cout << "Doing more independent math...\n";
}
int main() {
    std::thread t1(flexible_worker);
    std::thread t2(flexible_worker);
    t1.join(); t2.join();
    return 0;
}