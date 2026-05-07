#include <iostream>
#include <thread>
#include <mutex>
#include <vector>

std::mutex bathroom_dooor;
int total_uses;

void use_bathroom(){
    std::lock_guard<std::mutex> robot_guard(bathroom_dooor);
    // --- CRITICAL SECTION START ---
    total_uses++; 
    std::cout << "Bathroom used. Total: " << total_uses << "\n";
    // --- CRITICAL SECTION END ---

    // The function ends here. The robot_guard is destroyed and automatically unlocks the door!
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
