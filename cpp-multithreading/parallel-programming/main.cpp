#include <iostream>
#include <thread>

using namespace std;

// This is the function our thread will run.
// Notice the '&' - we are passing by reference so the thread modifies the REAL variable!
void calculate_sum(int a, int b, int &result) { //
    cout << "Thread: I am doing the math..." << endl;
    result = a + b;
}

int main(){
    int my_result = 0;
    
    // CREATE THE THREAD
    // We pass the function name, the inputs, and we MUST wrap our reference variable in std::ref()
    thread worker (calculate_sum, 10, 20, ref(my_result));
    cout << "Main: The thread is running in the background!" << endl;
    //  WAIT FOR THE THREAD
    // If we try to print my_result right now, it might still be 0!
    // We must wait for the worker to finish using join().
    worker.join();

    cout << "Main: The thread finished. Result is: " << my_result << endl;
    
    return 0;
}