#include <iostream>
#include <thread>
#include <future>
#include <chrono>
#include <random>
#include <cmath>

using std::cout;
using std::endl;
using std::promise;
using std::shared_future;
using std::future;
using std::thread;

void producer(promise<double> p, int id) {
    thread_local std::random_device rd;
    thread_local std::mt19937 gen(rd());
    std::uniform_int_distribution<> time_dist(1, 5);
    
    // We use a small range of integers converted to doubles 
    // to ensure we actually hit 0.0 occasionally to terminate the program.
    std::uniform_int_distribution<> val_dist(-5, 5);

    // Sleep for random interval (1 to 5 seconds)
    int wait_time = time_dist(gen);
    std::this_thread::sleep_for(std::chrono::seconds(wait_time));

    double value = static_cast<double>(val_dist(gen));
    cout << "[Producer " << id << "] Generated value: " << value << " (after " << wait_time << "s)\n";
    
    p.set_value(value);
}

// [EXAM TRICK] We MUST use shared_future here so both consumers can read the same values!
void consumer_sum(shared_future<double> sf1, shared_future<double> sf2, promise<double> p_sum) {
    double v1 = sf1.get();
    double v2 = sf2.get();
    
    double sum = v1 + v2;
    cout << "[Consumer 1] Sum: " << v1 << " + " << v2 << " = " << sum << "\n";
    
    p_sum.set_value(sum);
}

void consumer_product(shared_future<double> sf1, shared_future<double> sf2, promise<double> p_prod) {
    double v1 = sf1.get();
    double v2 = sf2.get();
    
    double product = v1 * v2;
    cout << "[Consumer 2] Product: " << v1 << " * " << v2 << " = " << product << "\n";
    
    p_prod.set_value(product);
}

// ==========================================
// MAIN: The Orchestrator & Cycle Manager
// ==========================================
int main() {
    int cycle_count = 1;

    while (true) {
        cout << "\n--- Starting Cycle " << cycle_count << " ---\n";

        // 1. Declare Promises for Producers
        promise<double> p_v1, p_v2;
        
        // 2. Extract and Convert to SHARED Futures for Consumers
        shared_future<double> sf_v1 = p_v1.get_future().share();
        shared_future<double> sf_v2 = p_v2.get_future().share();

        // 3. Declare Promises for Consumers (so Main knows when to stop)
        promise<double> p_sum, p_prod;
        future<double> f_sum = p_sum.get_future();
        future<double> f_prod = p_prod.get_future();

        // 4. Launch Threads
        thread t_p1(producer, std::move(p_v1), 1);
        thread t_p2(producer, std::move(p_v2), 2);
        
        // Notice we pass sf_v1 and sf_v2 BY VALUE, not by move, because shared_futures are copyable!
        thread t_c1(consumer_sum, sf_v1, sf_v2, std::move(p_sum));
        thread t_c2(consumer_product, sf_v1, sf_v2, std::move(p_prod));

        // 5. Main thread waits for the consumers to return their calculated results
        double final_sum = f_sum.get();
        double final_prod = f_prod.get();

        // 6. Cleanup memory for this cycle
        t_p1.join();
        t_p2.join();
        t_c1.join();
        t_c2.join();

        // 7. Termination Condition
        if (final_sum == 0.0 || final_prod == 0.0) {
            cout << "\n*** Termination condition met! Sum or Product is zero. Exiting. ***\n";
            break;
        }

        cycle_count++;
    }

    return 0;
}