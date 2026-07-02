/*
    ecursive implementation of fibonacci sequence ising promises and futures
*/
#include <iostream>
#include <future>
#include <thread>

using namespace std;

// Pass 'num' normally, use the promise to return the result
void fibonacci(int num, promise<int> p_result) {
    if (num < 2) {
        p_result.set_value(1);
    } 
    else {
        // 1. Setup channel for the N-1 branch
        promise<int> p1;
        future<int> f1 = p1.get_future();
        
        // 2. HIRE A WORKER: Launch ONE thread to handle the N-1 branch
        thread t1(fibonacci, num - 1, std::move(p1));

        // 3. DO IT YOURSELF: The current thread calculates the N-2 branch right now!
        // We still create a promise/future just to satisfy the function's parameters, 
        // but we execute it synchronously (NO new thread is created here).
        promise<int> p2;
        future<int> f2 = p2.get_future();
        fibonacci(num - 2, std::move(p2)); 

        // 4. Wait for the worker thread (f1) to finish, and grab our own result (f2)
        int result = f1.get() + f2.get();
        
        // 5. Send the combined result back up the chain
        p_result.set_value(result);

        // 6. Dismiss the worker
        t1.join();
    }
}

int main() {
    int N = 10; 

    // Setup the channel for the main thread to receive the final answer
    promise<int> main_promise;
    future<int> main_future = main_promise.get_future();

    cout << "Calculating Fibonacci(" << N << ") concurrently...\n";

    // Launch the root task
    thread root_thread(fibonacci, N, std::move(main_promise));
    
    // Block until the root task fulfills the promise
    int final_answer = main_future.get();
    
    // Clean up
    root_thread.join();

    cout << "Result: " << final_answer << endl;

    return 0;
}