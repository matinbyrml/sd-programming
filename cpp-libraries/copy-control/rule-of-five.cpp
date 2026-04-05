#include <iostream>
#include <algorithm>

using namespace std;

class IntBuffer{
    private:
        int* data;
        size_t size;
    public:
        IntBuffer(size_t s) : size(s) {
            data = new int[size];
            cout << "[+] Primary Constructor: Allocated " << size << " ints." << endl;
        }
        ~IntBuffer() {
            if (data != nullptr) {
                cout << "[-] Destructor: Freed " << size << " ints." << endl;
                delete[] data;
            } else {
                cout << "[-] Destructor: Safely destroyed an empty (moved-from) object." << endl;
            }
        }
        IntBuffer(const IntBuffer& other): size(other.size){
            data = new int[size];
            for(size_t i = 0; i<size; i++){
                data[i] = other.data[i];
            }
            cout << "[C] Copy Constructor: Deep copied " << size << " ints. O(N)" << endl;
        }

        /*
         Copy Assignment Operator (operator=(const C&)):
         Replaces the data of an already existing object with the data from another.
        */
        IntBuffer& operator=(const IntBuffer& other) {
            cout << "[=] Copy Assignment: Deep copying " << other.size << " ints. O(N)" << endl;
            
            // Step A: Protect against self-assignment (e.g., buffer1 = buffer1;)
            if (this == &other) { return *this; }
    
            // Step B: Delete our CURRENT memory so it doesn't leak!
            delete[] data;
    
            // Step C: Allocate new memory and copy the data
            size = other.size;
            data = new int[size];
            for (size_t i = 0; i < size; ++i) {
                data[i] = other.data[i];
            }
    
            return *this; // Return ourselves to allow chaining (a = b = c;)
        }

        // MOVE CONSTRUCTOR

        IntBuffer(IntBuffer&& other) noexcept: size(other.size), data(other.data){
            other.size = 0;
            other.data =  nullptr;
            cout << "[M] Move Constructor: Stole memory! O(1)" << endl;
        }
        IntBuffer& operator=(IntBuffer&& other) noexcept {
            cout << "[=>] Move Assignment: Stealing memory! O(1)" << endl;
            
            // Step A: Protect against self-assignment
            if (this == &other) { return *this; }
    
            // Step B: Delete our CURRENT memory so it doesn't leak!
            delete[] data;
    
            // Step C: Steal the pointer and size
            size = other.size;
            data = other.data;
    
            // Step D: Disconnect the original object
            other.size = 0;
            other.data = nullptr;
    
            return *this;
        }
};

int main() {
    cout << "--- TEST 1: Creation ---" << endl;
    IntBuffer a(100); // Calls Primary Constructor

    cout << "\n--- TEST 2: Copy Construction (L-Value) ---" << endl;
    IntBuffer b = a;  // 'a' is a permanent named variable. Calls Copy Constructor!

    cout << "\n--- TEST 3: Copy Assignment (L-Value) ---" << endl;
    IntBuffer c(50);  // Creates a small buffer
    c = a;            // 'c' already exists. We overwrite it with 'a'. Calls Copy Assignment!

    cout << "\n--- TEST 4: Move Construction (R-Value) ---" << endl;
    // We use std::move to force 'b' to become a temporary R-value
    IntBuffer d = std::move(b); // Calls Move Constructor! 'b' is now empty.

    cout << "\n--- TEST 5: Move Assignment (R-Value) ---" << endl;
    // 'c' already exists. We overwrite it by stealing from 'a'.
    c = std::move(a); // Calls Move Assignment! 'a' is now empty.

    cout << "\n--- END OF MAIN (Destructors will now fire) ---" << endl;
    return 0;
}