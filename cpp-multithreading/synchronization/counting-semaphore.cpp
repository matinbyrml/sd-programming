#include <iostream>
#include <thread>
#include <vector>
#include <semaphore> // Required for C++20 Semaphores
#include <chrono>

// Create a semaphore with a maximum of 3 concurrent accesses.
std::counting_semaphore<3> parking_lot(3); 

void car_thread(int id) {
    std::cout << "Car " << id << " is waiting at the barrier...\n";
    
    // ACQUIRE: Decrements the counter. Blocks if the counter is 0.
    parking_lot.acquire(); 
    
    // --- ENTERED THE PARKING LOT ---
    std::cout << "--> Car " << id << " ENTERED. Parking for 2 seconds.\n";
    std::this_thread::sleep_for(std::chrono::seconds(2)); // Simulate being parked
    
    std::cout << "<-- Car " << id << " LEAVING.\n";
    
    // RELEASE: Increments the counter. Automatically wakes up the next waiting car!
    parking_lot.release(); 
}

int main() {
    std::vector<std::thread> cars;
    
    // Let's spawn 5 cars. Only 3 can fit in the lot at the same time!
    for(int i = 1; i <= 5; i++) {
        cars.push_back(std::thread(car_thread, i));
    }
    
    for(auto& c : cars) c.join();
    return 0;
}