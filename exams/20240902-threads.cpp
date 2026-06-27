#include <iostream>
#include <vector>
#include <thread>
#include <functional> // For std::cref and std::ref

using std::vector;

// ==========================================
// WORKER THREAD: Computes a chunk of rows
// ==========================================
void jacobi_chunk(const vector<vector<double>>& A, 
                  const vector<double>& b, 
                  const vector<double>& x, 
                  vector<double>& x_new, 
                  int start_row, 
                  int end_row) {
    
    int size = A.size();
    
    // Each thread only computes its assigned rows
    for (int i = start_row; i < end_row; ++i) {
        double sum = 0.0;
        
        // Sum all (A[i][j] * x[j]) where j != i
        for (int j = 0; j < size; ++j) {
            if (i != j) {
                sum += A[i][j] * x[j];
            }
        }
        
        // Apply the Jacobi formula
        x_new[i] = (b[i] - sum) / A[i][i];
    }
}

// ==========================================
// MAIN FUNCTION: The Orchestrator
// ==========================================
void jacobi(vector<vector<double>>& A, 
            std::vector<double>& b, 
            vector<double>& x, 
            vector<double>& x_new, 
            int n) {
    
    int size = A.size();
    
    // 1. Determine optimal thread count
    int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) num_threads = 4; // Fallback
    
    int chunk_size = size / num_threads;

    // 2. The Iteration Loop
    for (int k = 0; k < n; ++k) {
        std::vector<std::thread> threads;
        
        // Step A: Launch threads to compute x_new
        for (int t = 0; t < num_threads; ++t) {
            int start_row = t * chunk_size;
            
            // [EXAM TRICK] The Remainder Check ensures the last thread 
            // captures any leftover rows if size isn't perfectly divisible.
            int end_row = (t == num_threads - 1) ? size : start_row + chunk_size;
            
            // Launch the thread, strictly defining read-only and write-only variables
            threads.emplace_back(jacobi_chunk, 
                                 std::cref(A), std::cref(b), std::cref(x), std::ref(x_new), 
                                 start_row, end_row);
        }
        
        // Step B: The Synchronization Barrier
        // We MUST wait for all threads to finish computing x_new 
        // before we can update x for the next iteration.
        for (auto& t : threads) {
            t.join();
        }
        
        // Step C: Prepare for the next iteration
        // Update the current state 'x' with the newly calculated 'x_new'.
        // (We skip copying on the final iteration because x_new holds the final answer).
        if (k < n - 1) {
            x = x_new; 
        }
    }
} 