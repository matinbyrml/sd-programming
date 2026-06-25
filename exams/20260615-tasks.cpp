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


void threadA(future<int> f_N, promise <vector<double>> p_vec){
    int N = f_N.get();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(0.0, 100.0);

    // 2. Generate and populate the vector
    vector<double> vec(N);
    for (int i = 0; i < N; ++i) {
        vec[i] = dist(gen);
    }
    std::sort(vec.begin(), vec.end());
    p_vec.set_value(std::move(vec));
}

void threadB(future<double> f_V,future<vector<double>> f_vec, promise<int> p_idx ){
    double V = f_V.get();
    vector<double> vec = f_vec.get();
    auto it = std::lower_bound(vec.begin(), vec.end(), V);
    if (it != vec.end() && *it == V) {
        // Value found: pass the index to Thread C
        p_idx.set_value(std::distance(vec.begin(), it));
    } else {
        // Value not found: pass -1 as a sentinel value for "KO"
        p_idx.set_value(-1); 
    }
}

void threadC(future<int> f_idx) {
    // 1. Block until Thread B finishes the search
    int idx = f_idx.get();
    
    // 2. Output the result
    if (idx != -1) {
        cout << idx << endl;
    } else {
        cout << "KO" << endl;
    }
}

int main(){
    promise<int> p_N;
    promise<double> p_V;
    promise<vector<double>> p_vec;
    promise<int> p_idx;

    future<int> f_N = p_N.get_future();
    future<double> f_V = p_V.get_future();
    future<vector<double>> f_vec = p_vec.get_future();
    future<int> f_idx = p_idx.get_future();

    thread tA(threadA, std::move(f_N), std::move(p_vec));
    thread tB(threadB, std::move(f_V), std::move(f_vec), std::move(p_idx));
    thread tC(threadC, std::move(f_idx));
    int N;
    double V;
    if (cin >> N >> V) {
        // Fulfill promises to wake up Thread A and Thread B
        p_N.set_value(N);
        p_V.set_value(V);
    } else {
        // Failsafe: if input is invalid, send dummy data so threads don't deadlock
        p_N.set_value(0); 
        p_V.set_value(0.0);
    }


    tA.join();
    tB.join();
    tC.join();

    return 0;
}