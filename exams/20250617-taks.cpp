#include <iostream>
#include <vector>
#include <thread>
#include <future>
#include <algorithm>
#include <random>

// Use explicit declarations instead of namespace std
using std::cout;
using std::cin;
using std::endl;
using std::vector;
using std::thread;
using std::promise;
using std::future;



void multiply(int v1, int v2, promise<int> result){
    int n = v1* v2;
    result.set_value(n);

}


int main(){
    vector<int> v1 = {1,2,3,4};
    vector<int> v2 = {1,2,3,4};
    vector<promise<int>> promises(v1.size());
    vector<future<int>> futures;
    vector<thread> threads;

    for(int n = 0; n<v1.size(); n++){
        futures.push_back(promises[n].get_future());
    }

    for(int n = 0; n<v1.size(); n++){
        threads.emplace_back(
            multiply, v1[n], v2[n], std::move(promises[n]));
    }
     // Stage 2: Aggregate values sequentially on the host 
     int total_dot_product = 0;
     for(int n = 0; n<v1.size(); n++){
        threads.emplace_back(
            multiply, v1[n], v2[n], std::move(promises[n]));
    }
     for (auto& t : threads) {
         t.join();
     }
 
     std::cout << "Calculated Dot Product: " << total_dot_product << "\n"; // Expected: 5+12+21+32 = 70
     return 0;


}