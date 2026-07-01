/*
    Write a program in C++ implementing a producer and consumer scheme using promises and futures. Two
    producers generate real values at random intervals (ranging from 1 to 5 seconds). When both values are ready,
    two consumers get those values, and then the first one displays the sum of those values, and the second one
    shows their product. Once the consumers have done so, the entire cycle in the main function restarts (with the
    producers producing two values, etc.). The program ends when the sum or product of the two values (computed
    by the consumers) equals zero.
*/

/*
    We mainly focus on shared futures and how to use them since 2 consumers should use same generalted values twice
*/
#include <iostream>
#include <thread>
#include <future>
#include <random>
#include <chrono>

using namespace std;

// --- PRODUCER ---
void producer(int id, promise<double> p_val) {
    // Random sleep between 1 and 5 seconds
    mt19937 rng(random_device{}() + id);
    uniform_int_distribution<int> time_dist(1, 5);
    int sleep_time = time_dist(rng);
    
    this_thread::sleep_for(chrono::seconds(sleep_time));

    // Generate a random real value. 
    // We use a small integer range [-3, 3] cast to double to ensure a sum or product 
    // of 0.0 actually occurs reasonably often to end the program!
    uniform_int_distribution<int> val_dist(-3, 3);
    double generated_val = static_cast<double>(val_dist(rng));
    
    cout << "[Producer " << id << "] Generated " << generated_val 
         << " (after " << sleep_time << "s)\n";
         
    // Send the value to the consumers
    p_val.set_value(generated_val);
}

// --- CONSUMER 1: Sum ---
void consumer_sum(shared_future<double> f_val1, shared_future<double> f_val2, promise<double> p_sum) {
    // Blocks until both producers have called set_value
    double val1 = f_val1.get();
    double val2 = f_val2.get();
    
    double sum = val1 + val2;
    cout << "[Consumer 1] Computed Sum: " << val1 << " + " << val2 << " = " << sum << "\n";
    
    // Send the result back to main to check for termination
    p_sum.set_value(sum);
}

// --- CONSUMER 2: Product ---
void consumer_prod(shared_future<double> f_val1, shared_future<double> f_val2, promise<double> p_prod) {
    // Blocks until both producers have called set_value
    double val1 = f_val1.get();
    double val2 = f_val2.get();
    
    double prod = val1 * val2;
    cout << "[Consumer 2] Computed Product: " << val1 << " * " << val2 << " = " << prod << "\n";
    
    // Send the result back to main to check for termination
    p_prod.set_value(prod);
}

int main() {
    int cycle = 1;
    
    while (true) {
        cout << "\n--- Starting Cycle " << cycle << " ---\n";
        
        // 1. Create promises for the producers
        promise<double> p_val1, p_val2;
        
        // 2. Extract futures and convert them to SHARED futures!
        // This is crucial because TWO consumers need to read these same values.
        shared_future<double> f_val1 = p_val1.get_future().share();
        shared_future<double> f_val2 = p_val2.get_future().share();
        
        // 3. Create promises for the consumers to return their results to main
        promise<double> p_sum, p_prod;
        future<double> f_sum = p_sum.get_future();
        future<double> f_prod = p_prod.get_future();
        
        // 4. Launch all 4 threads
        thread t_p1(producer, 1, std::move(p_val1));
        thread t_p2(producer, 2, std::move(p_val2));
        
        thread t_c1(consumer_sum, f_val1, f_val2, std::move(p_sum));
        thread t_c2(consumer_prod, f_val1, f_val2, std::move(p_prod));
        
        // 5. Main thread waits for the consumers to finish their math
        double final_sum = f_sum.get();
        double final_prod = f_prod.get();
        
        // 6. Clean up threads for this cycle
        t_p1.join();
        t_p2.join();
        t_c1.join();
        t_c2.join();
        
        // 7. Check termination condition
        if (final_sum == 0.0 || final_prod == 0.0) {
            cout << "\n>>> Termination condition met (Sum = " << final_sum 
                 << ", Product = " << final_prod << "). Exiting program! <<<\n";
            break;
        }
        
        cycle++;
    }

    return 0;
}