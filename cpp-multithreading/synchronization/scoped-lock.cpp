#include <iostream>
#include <thread>
#include <mutex>

std::mutex car;
std::mutex remote;

void play_with_toys() {
    // scoped_lock reaches out and grabs BOTH locks using a safe, deadlock-free algorithm!
    // If it can't get both, it patiently waits without holding onto just one.
    std::scoped_lock parent_rule(car, remote); 
    
    std::cout << "I have both toys! I am playing safely.\n";
    
    // Automatically unlocks BOTH when the function ends
}

int main() {
    std::thread child1(play_with_toys);
    std::thread child2(play_with_toys);
    
    child1.join(); child2.join();
    return 0;
}