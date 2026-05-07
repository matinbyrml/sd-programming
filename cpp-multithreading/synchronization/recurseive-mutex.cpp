#include <iostream>
#include <thread>
#include <mutex>

std::recursive_mutex diary_lock; // this is a smart lock

void write_in_diary(int pages_left){
    // the robot locks this diary, but if the same thread tries to come back it allows it
    std::lock_guard<std::recursive_mutex> robot_guard(diary_lock);
    if(pages_left){
        std::cout << "Writing page... " << pages_left << " left.\n";
        // THE TRICK: We are calling the function inside itself!
        // This thread will loop back to the top and try to lock the diary AGAIN.
        write_in_diary(pages_left - 1);
    }
}

int main(){
    std::thread writer(write_in_diary, 4);
    writer.join();
    return 0;
}