#include <iostream>
#include <semaphore>
#include <thread>
#include <chrono>

// The absolute minimum synchronization primitives: 3 counting semaphores
std::counting_semaphore<3> sem_p1(1);   // Starts at 1 to allow P1 to begin
std::counting_semaphore<3> sem_p23(0);  // Blocks P2 and P3 until P1 finishes
std::counting_semaphore<3> sem_p4(0);   // Blocks P4 until P2 and both P3s finish

void P1() {
    while (true) {
        sem_p1.acquire(); // Wait for P4 to finish (or initial start)
        
        std::cout << "P1 executing...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Simulate work
        
        // WAKE UP MIDDLE LAYER: Release 3 tokens for P2 and the two P3 threads
        sem_p23.release(3); 
    }
}

void P2() {
    while (true) {
        sem_p23.acquire(); // Wait for P1
        
        std::cout << "P2 executing...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // Notify P4 that P2 is done
        sem_p4.release(); 
    }
}

// We pass an ID just to distinguish the two identical threads in the console
void P3(int id) {
    while (true) {
        sem_p23.acquire(); // Wait for P1
        
        std::cout << "P3 (Instance " << id << ") executing...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // Notify P4 that this instance of P3 is done
        sem_p4.release(); 
    }
}

void P4() {
    while (true) {
        // Wait for all 3 middle threads to finish by acquiring 3 times
        sem_p4.acquire();
        sem_p4.acquire();
        sem_p4.acquire();
        
        std::cout << "P4 executing...\n";
        std::cout << "--- Cycle Complete ---\n\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // RESTART CYCLE: Wake up P1
        sem_p1.release(); 
    }
}

int main() {
    // Spawn the 5 cyclical threads
    std::thread t1(P1);
    std::thread t2(P2);
    std::thread t3_a(P3, 1); // First instance of P3
    std::thread t3_b(P3, 2); // Second instance of P3
    std::thread t4(P4);

    // Join threads (though in an infinite loop, they will run forever)
    t1.join();
    t2.join();
    t3_a.join();
    t3_b.join();
    t4.join();

    return 0;
}