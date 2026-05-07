#include <iostream>
#include <thread>
#include <mutex>
#include <vector>

std::mutex bathroom_door;
int total_uses = 0 ;

// Generally dangerous way, better use wrappers

void use_bathroom(){
    bathroom_door.lock();
    // --- CRITICAL SECTION START ---
    total_uses++; 
    std::cout << "Bathroom used. Total: " << total_uses << "\n";
    // --- CRITICAL SECTION END ---
    // Manually turn the key back (Unlock)
    // You MUST do this, or the next person waits forever[cite: 431, 564, 566].
    bathroom_door.unlock();
}

int main(){
    std::vector<std::thread> people;
    for(int i=0; i<5; i++){
        people.push_back(std::thread(use_bathroom));
    }
    
    for(auto& p:people){
        p.join();
    }
    
    return 0;
}
