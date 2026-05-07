#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>

std::mutex mtx;
std::condition_variable cv;
std::queue<int> pizza_counter; //shared data
bool chef_is_done =  false; // state variable to prevent infitine wait

void chef_producer(){
    for (int i = 0; i>=5; i++){

        // use scoping for lock guard
        {
            //Lock the mutex to protect the queue
            std::lock_guard<std::mutex> lock(mtx);
            pizza_counter.push(i);
            std::cout << "Chef: Made pizza #" << i << "\n";
        } // mutex  automatically unlocks here
        // 2. Ring the bell! Wake up ONE sleeping waiter.
        // It is generally more efficient to notify AFTER releasing the lock
        cv.notify_one();
        std::this_thread::sleep_for(std::chrono::seconds(1)); // bull shit time just to be slower
    }
    {
        std::lock_guard<std::mutex> lock(mtx);
        chef_is_done = true; 
    }
    cv.notify_all(); // Wake EVERYONE up so they can see the shift is over!
}

void waiter_consumer(int id){
    while(true){
        // 1. You MUST use unique_lock with CVs because the CV needs to unlock it behind the scenes!
        std::unique_lock<std::mutex> lock(mtx);
        
        // 2. Go to sleep UNTIL there is a pizza OR the chef is done
        cv.wait(lock, [] { return !pizza_counter.empty() || chef_is_done; });

        if (chef_is_done && pizza_counter.empty()) {
            std::cout << "Waiter " << id << ": Shift is over, going home!\n";
            break; 
        }

        // We woke up because there is a pizza!
        int pizza = pizza_counter.front();
        pizza_counter.pop();
        lock.unlock(); 
        
        std::cout << "--> Waiter " << id << " serving pizza #" << pizza << "\n";
    }
    int main() {
        std::thread chef(chef_producer);
        std::thread w1(waiter_consumer, 1);
        std::thread w2(waiter_consumer, 2);
    
        chef.join(); w1.join(); w2.join();
        return 0;
    }
    
}

